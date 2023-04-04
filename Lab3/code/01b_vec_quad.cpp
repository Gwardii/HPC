#include "MakeRandomVector.hpp"
#include "benchmark/benchmark.h"

void BM_vec_quad(benchmark::State& state)
{
    const auto            src = makeRandomVector< double >(state.range(0));
    std::vector< double > dest(src.size());
    for (auto _ : state)
    {
        for (std::ptrdiff_t i = 0; auto v : src)
            dest[i++] = v * v + v * 3.5;
        benchmark::DoNotOptimize(dest.data());
        benchmark::ClobberMemory();
    }
    state.SetBytesProcessed(sizeof(double) * src.size() * state.iterations());
}
BENCHMARK(BM_vec_quad)->RangeMultiplier(2)->Range(1 << 8, 1u << 18);
BENCHMARK_MAIN();
