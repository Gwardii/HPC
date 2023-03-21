#include "benchmark/benchmark.h"
#include "MakeRandomVector.hpp"
#include <chrono>
#include <thread>
#include <cmath>
#include <algorithm>

void noop(benchmark::State& state) // 1. Function defining the benchmark
{
    // 2. Setup code

    for(auto _ : state) // 3. Benchmark loop
    {
    }

    // 5. Tear-down code
}
void bench_wait(benchmark::State& state) // 1. Function defining the benchmark
{
    // 2. Setup code

    for(auto _ : state) // 3. Benchmark loop
    {
        using namespace std::chrono_literals;
        std::this_thread::sleep_for(1s); // waits 1 second
    }

    // 5. Tear-down code
}
void sin_benchmark(benchmark::State& state) // 1. Function defining the benchmark
{
    // 2. Setup code

    for(auto _ : state) // 3. Benchmark loop
    {
        const double y = std::sin(12.);
        benchmark::DoNotOptimize(y);
    }

    // 5. Tear-down code
}
void sines_benchmark(benchmark::State& state) // 1. Function defining the benchmark
{
    // 2. Setup code

    for(auto _ : state) // 3. Benchmark loop
    {
        auto y = makeRandomVector<double>(state.range(0));
        std::transform(y.begin(), y.end(), y.begin(), [](double x){return std::sin(x);});
        benchmark::DoNotOptimize(y.data());
        benchmark::ClobberMemory();
    }
    state.SetBytesProcessed(state.iterations() * 8 * state.range(0));

    // 5. Tear-down code
}

template<typename T>
void sort_benchmark(benchmark::State& state){
    for(auto _ : state){
        state.PauseTiming();
        auto y = makeRandomVector<T>(state.range(0));
        benchmark::DoNotOptimize(y.data());
        benchmark::ClobberMemory();
        state.ResumeTiming();
        std::sort(y.begin(), y.end());
    }
    // state.SetBytesProcessed(state.iterations() * 8 * state.range(0));
}
// BENCHMARK(noop)->Name("noop");
// BENCHMARK(bench_wait)->Unit(benchmark::kSecond)->Name("Wait 1 second");
// BENCHMARK(sin_benchmark)->Name("Sine of a value");
// BENCHMARK(sines_benchmark)->Name("Sine of multiple values")->RangeMultiplier(4)->Range(256,65536);
BENCHMARK(sort_benchmark<double>)->Name("Sort")->RangeMultiplier(4)->Range(256,65536);
BENCHMARK_MAIN();