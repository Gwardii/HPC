#include <iostream>
#include <vector>
#include <cmath>
#include <atomic>

#include "benchmark/benchmark.h"

#include <tbb/parallel_for.h>
#include <tbb/global_control.h>

static void BM_SomeFunction(benchmark::State& state) {
    int size = state.range(0);
    int no_threds = state.range(1);
    tbb::global_control c(tbb::global_control::max_allowed_parallelism, no_threds);
    double *x = new double[no_threds];
    auto y = std::vector<double>(size);
    double h = 4 * M_PI / (size-1);
    std::atomic<int> thread_counter{0};
    
    for (auto _ : state) {
      
      //for (int i=0; i<y.size(); ++i)
      tbb::parallel_for( tbb::blocked_range<int>(0,y.size()),
                       [&](tbb::blocked_range<int> r)
      {
          thread_local const int my_id = thread_counter++; 
          for (int i=r.begin(); i<r.end(); ++i)
          {
            x[my_id] = i*h;
            y[i] = std::sin(pow(x[my_id], M_PI)) 
                 * std::cos(pow(x[my_id], M_PI))
                 * std::tan(pow(x[my_id], M_PI))
                 - std::atan(exp(x[my_id]) /(x[my_id]+1.0) );
          }
      });
   }
   
   delete []x;
   state.counters["DPFlops"] = benchmark::Counter{static_cast<double>(state.iterations() * size),
                               benchmark::Counter::kIsRate,
                               benchmark::Counter::kIs1000};
}
// Register the function as a benchmark
BENCHMARK(BM_SomeFunction)->UseRealTime()->Ranges({{2, 2<<20}, {1, 2}})
                                         ->Ranges({{2, 2<<20}, {4, 8}});
// Run the benchmark
BENCHMARK_MAIN();
