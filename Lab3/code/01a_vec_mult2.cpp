#include "MakeRandomVector.hpp"
#include "benchmark/benchmark.h"

void BM_vec_mult2(benchmark::State& state)
{
    const auto            src = makeRandomVector< double >(state.range(0));
    std::vector< double > dest(src.size());
    for (auto _ : state)
    {
        for (std::ptrdiff_t i = 0; auto v : src)
            dest[i++] = v * 2;
        benchmark::DoNotOptimize(dest.data());
        benchmark::ClobberMemory();
    }
    state.SetBytesProcessed(sizeof(double) * src.size() * state.iterations());
}
BENCHMARK(BM_vec_mult2)->RangeMultiplier(2)->Range(1 << 8, 1 << 18);
BENCHMARK_MAIN();
