#include "MakeRandomVector.hpp"
#include "benchmark/benchmark.h"

#include <bitset>
#include <cmath>
#include <memory>
#include <stdexcept>

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Utils
auto allocateArray(size_t size, size_t alignment = 4096) -> std::unique_ptr< double[] >
{
    if (alignment < alignof(double))
        throw std::runtime_error{"Allocation must be aligned to at least alignof(double)"};
    if (std::bitset< sizeof(size_t) * 8 >{alignment}.count() != 1)
        throw std::runtime_error{"Alignment must be a power of 2"};
    return std::unique_ptr< double[] >{new (std::align_val_t{alignment}) double[size]};
}
void fill0(const std::unique_ptr< double[] >& alloc, size_t size)
{
    std::fill_n(alloc.get(), size, 0.);
}
void fillRandom(const std::unique_ptr< double[] >& alloc, size_t size)
{
    auto random_dist = std::uniform_real_distribution< double >{-1., 1.};
    auto prng        = std::mt19937{std::random_device{}()};
    std::generate_n(alloc.get(), size, [&]() mutable { return random_dist(prng); });
}
void nukeCache()
{
    constexpr size_t nuke_size = 1 << 28;
    const auto       alloc     = std::make_unique< volatile char[] >(nuke_size);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Problem size
static constexpr size_t N = 1 << 10;

// Baseline - triple-nested for loop
static void naiveDgemmSquare(const double* A, const double* B, double* C)
{
    for (size_t col = 0; col != N; ++col)
        for (size_t k = 0; k != N; ++k)
            for (size_t row = 0; row != N; ++row)
                C[row + col * N] += A[row + k * N] * B[k + col * N];
}
static void BM_baseline(benchmark::State& state)
{
    const auto A = allocateArray(N * N);
    const auto B = allocateArray(N * N);
    const auto C = allocateArray(N * N);
    fillRandom(A, N * N);
    fillRandom(B, N * N);
    fill0(C, N * N);
    for (auto _ : state)
    {
        state.PauseTiming();
        fill0(C, N * N);
        nukeCache();
        state.ResumeTiming();
        naiveDgemmSquare(A.get(), B.get(), C.get());
        benchmark::DoNotOptimize(C.get());
        benchmark::ClobberMemory();
    }
    // This is basically setBytesProcessed, just with the custom counter name "(DP)Flops/s"
    state.counters["(DP)Flops/s"] = benchmark::Counter{static_cast< double >(state.iterations() * N * N * N * 2),
                                                       benchmark::Counter::kIsRate,
                                                       benchmark::Counter::kIs1000};
}
BENCHMARK(BM_baseline)->Name("Baseline: triple-loop")->Unit(benchmark::kMillisecond);

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Goto algorithm
static constexpr size_t panel_width  = 32; // tune should be tuned
static constexpr size_t sliver_width = 4;  // this is fixed

// Micro-kernel: (sliver_width x sliver_width) = (sliver_width -by- panel width) x (panel width -by- sliver_width)
// A is packed in column-major order, B is packed in row-major order
static void multSliverSliver(const double* A_sliver, const double* B_sliver, double* C_pixel)
{
    for (size_t i = 0; i != panel_width; ++i)
        for (size_t j = 0; j != sliver_width; ++j)
            for (size_t k = 0; k != sliver_width; ++k)
                C_pixel[k + j * sliver_width] += A_sliver[k + i * sliver_width] * B_sliver[j + i * sliver_width];
}
static void BM_MicroKernel(benchmark::State& state)
{
    const auto A = allocateArray(sliver_width * panel_width);
    const auto B = allocateArray(sliver_width * panel_width);
    const auto C = allocateArray(sliver_width * sliver_width);
    fillRandom(A, sliver_width * panel_width);
    fillRandom(B, sliver_width * panel_width);
    fill0(C, sliver_width * sliver_width);
    for (auto _ : state)
    {
        multSliverSliver(A.get(), B.get(), C.get());
        benchmark::DoNotOptimize(C.get());
        benchmark::ClobberMemory();
    }
    state.counters["(DP)Flops/s"] =
        benchmark::Counter{static_cast< double >(state.iterations() * sliver_width * sliver_width * panel_width * 2),
                           benchmark::Counter::kIsRate,
                           benchmark::Counter::kIs1000};
}
BENCHMARK(BM_MicroKernel)->Name("Micro-kernel");

void packPanel(const double* B, size_t panel, const std::unique_ptr< double[] >& pack_alloc)
{
    static const size_t no_sliver_elems = panel_width * sliver_width;
    static const size_t no_panel_elems  = panel_width * N;
    for (size_t i = 0; i != no_panel_elems; i++)
    {
        size_t j      = (i % sliver_width) * N + i / sliver_width;
        pack_alloc[i] = B[(i / no_sliver_elems) * N * sliver_width + j % (no_sliver_elems) + panel * panel_width];
    }
}
void packBlock(const double* A, size_t block, size_t panel, const std::unique_ptr< double[] >& pack_alloc)
{
    static const size_t magic_divisior = N * (panel_width - 1UL) + panel_width;
    for (size_t i = 0; i != panel_width * panel_width; i++)
    {
        size_t j = i % sliver_width + (i / sliver_width) * N;
        pack_alloc[i] =
            A[j % magic_divisior + (j / magic_divisior) * sliver_width + panel_width * block + N * panel * panel_width];
    }
}
size_t computeABlockOffset(size_t a_sliver)
{
    return a_sliver * sliver_width * panel_width;
}
size_t computeBPanelOffset(size_t b_sliver)
{
    return b_sliver * sliver_width * panel_width;
}
void scatterCPixel(const double* C_pixel, size_t panel, size_t block, size_t a_sliver, size_t b_sliver, double* C)
{
    for (size_t i = 0; i != sliver_width * sliver_width; i++)
    {
        C[a_sliver * sliver_width + b_sliver * sliver_width * N + block * panel_width + panel * panel_width * N + i] =
            C_pixel[i];
    }
}
static void gotoDgemmSquare(const double* A, const double* B, double* C)
{
    const auto B_packed_panel = allocateArray(N * panel_width);
    const auto A_packed_block = allocateArray(panel_width * panel_width);
    for (size_t panel = 0; panel != N / panel_width; ++panel)
    {
        packPanel(B, panel, B_packed_panel);
        for (size_t block = 0; block != N / panel_width; ++block)
        {
            packBlock(A, block, panel, A_packed_block);
            for (size_t b_sliver = 0; b_sliver != N / sliver_width; ++b_sliver)
                for (size_t a_sliver = 0; a_sliver != panel_width / sliver_width; ++a_sliver)
                {
                    const auto A_offs = computeABlockOffset(a_sliver);
                    const auto B_offs = computeBPanelOffset(b_sliver);
                    double     C_pixel[sliver_width * sliver_width];
                    std::fill_n(C_pixel, sliver_width * sliver_width, 0.);
                    multSliverSliver(A_packed_block.get() + A_offs, B_packed_panel.get() + B_offs, C_pixel);
                    // scatterCPixel(C_pixel, panel, block, a_sliver, b_sliver, C);
                }
        }
    }
}

static void BM_Goto(benchmark::State& state)
{
    const auto A = allocateArray(N * N);
    const auto B = allocateArray(N * N);
    const auto C = allocateArray(N * N);
    fillRandom(A, N * N);
    fillRandom(B, N * N);
    fill0(C, N * N);
    for (auto _ : state)
    {
        state.PauseTiming();
        fill0(C, N * N);
        nukeCache();
        state.ResumeTiming();
        gotoDgemmSquare(A.get(), B.get(), C.get());
        benchmark::DoNotOptimize(C.get());
        benchmark::ClobberMemory();
    }
    state.counters["(DP)Flops/s"] = benchmark::Counter{static_cast< double >(state.iterations() * N * N * N * 2),
                                                       benchmark::Counter::kIsRate,
                                                       benchmark::Counter::kIs1000};
}
BENCHMARK(BM_Goto)->Name("Goto algorithm")->Unit(benchmark::kMillisecond);
