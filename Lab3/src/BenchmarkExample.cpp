#include "MakeRandomVector.hpp"
#include "benchmark/benchmark.h"


static void BM_vector_reduction(benchmark::State& state)
{
    for (auto _ : state)
    {
        state.PauseTiming();
        const auto input  = makeRandomVector< double >(state.range(0));
        state.ResumeTiming();
        double sum = std::accumulate(input.cbegin(), input.cend(), 0, std::plus<>{});
        benchmark::DoNotOptimize(sum);
        benchmark::ClobberMemory();
    }
}
BENCHMARK(BM_vector_reduction)->Name("Vector reduction")->Arg(1e6);