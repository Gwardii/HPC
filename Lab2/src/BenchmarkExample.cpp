#include "MakeRandomVector.hpp"
#include "benchmark/benchmark.h"

#include <cmath>
#include <numbers>

static void BM_compute_sine(benchmark::State& state)
{
    const auto input  = makeRandomVector< double >(100, 0.0, std::numbers::pi/2);
    // auto       output = input;
    for (auto _ : state)
    {
        // std::transform(input.cbegin(), input.cend(), output.begin(), [](auto x) { return std::sin(x); });
        double sum = std::accumulate(input.cbegin(), input.cend(), 0, [](auto _sum, auto _to_add){return _sum + _to_add * (std::sin(_to_add) > 0.5);});
        // benchmark::DoNotOptimize(output.data());
        benchmark::DoNotOptimize(sum);
        benchmark::ClobberMemory();
    }
    state.SetBytesProcessed(state.iterations() * input.size() * sizeof(double));
}
static void BM_compute_sine_presorted(benchmark::State& state)
{
    auto input  = makeRandomVector< double >(100, 0.0, std::numbers::pi/2);
    // auto       output = input;
    for (auto _ : state)
    {
        std::sort(input.begin(), input.end());
        // std::transform(input.cbegin(), input.cend(), output.begin(), [](auto x) { return std::sin(x); });
        double sum = std::accumulate(input.cbegin(), input.cend(), 0, [](auto _sum, auto _to_add){return _sum + _to_add * (std::sin(_to_add) > 0.5);});
        // benchmark::DoNotOptimize(output.data());
        benchmark::DoNotOptimize(sum);
        benchmark::ClobberMemory();
    }
    state.SetBytesProcessed(state.iterations() * input.size() * sizeof(double));
}
BENCHMARK(BM_compute_sine)->Name("Compute sine");
BENCHMARK(BM_compute_sine_presorted)->Name("Compute sine presorted");
