#include <iostream>
#include <vector>
#include <cmath>

#include "benchmark/benchmark.h"

static void BM_SomeFunction(benchmark::State& state) {
    int size = state.range(0);
    
    auto y = std::vector<double>(size);
    double h = 4 * M_PI / (size-1);
    for (auto _ : state) {
      
      for (int i=0; i<y.size(); ++i)
      {
          double x = i*h;
          y[i] = std::sin(pow(x, M_PI)) * std::cos(pow(x, M_PI))
             * std::tan(pow(x, M_PI)) - std::atan(exp(x) /(x+1.0) );
      };
      
   }
   
   state.counters["DPFlops"] = benchmark::Counter{static_cast<double>(state.iterations() * size),
                               benchmark::Counter::kIsRate,
                               benchmark::Counter::kIs1000};
}
// Register the function as a benchmark
BENCHMARK(BM_SomeFunction)->UseRealTime()->Range(2, 2<<25);
// Run the benchmark
BENCHMARK_MAIN();
