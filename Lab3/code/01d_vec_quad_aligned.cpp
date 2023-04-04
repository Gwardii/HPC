#include "MakeRandomVector.hpp"
#include "benchmark/benchmark.h"
#include <memory>

template < typename T >
requires std::floating_point< T > || std::integral< T >
auto makeRandomAlignedArray(std::size_t size      = 100,
                            std::size_t alignment = 64,
                            T           min       = std::numeric_limits< T >::min(),
                            T           max       = std::numeric_limits< T >::max())
{
    thread_local std::mt19937 prng{std::random_device{}()};
    rand_dist_t< T >          random_dist{min, max};

    std::unique_ptr< T[] > result{new (std::align_val_t{alignment}) T[size]};
    std::generate_n(result.get(), size, [&] { return random_dist(prng); });
    return result;
}

void BM_vec_quad(benchmark::State& state)
{
    const std::size_t size       = state.range(0);
    const auto        src_alloc  = makeRandomAlignedArray< double >(size, 32);
    auto              dest_alloc = makeRandomAlignedArray< double >(size, 32);

    const double* src  = std::assume_aligned< 32 >(src_alloc.get());
    double*       dest = std::assume_aligned< 32 >(dest_alloc.get());

    for (auto _ : state)
    {
        for (ptrdiff_t i = 0; i < size; ++i)
            dest[i] = src[i] * src[i] + src[i] * 3.5;
        benchmark::DoNotOptimize(dest);
        benchmark::ClobberMemory();
    }
    state.SetBytesProcessed(sizeof(double) * size * state.iterations());
}
BENCHMARK(BM_vec_quad)->RangeMultiplier(2)->Range(1 << 8, 1u << 18);
BENCHMARK_MAIN();
