#include <iostream>
#include <vector>
#include <cmath>

#include "benchmark/benchmark.h"

#include <tbb/parallel_for.h>
#include <tbb/parallel_reduce.h>
#include <tbb/global_control.h>

static void BM_SomeFunction(benchmark::State& state) {
    int size = state.range(0);
    int no_threds = state.range(1);
    tbb::global_control c(tbb::global_control::max_allowed_parallelism, no_threds);
    
    auto y = std::vector<double>(size);
    double h = 4 * M_PI / (size-1);
    double avg{0};
    for (auto _ : state) {
      
      //for (int i=0; i<y.size(); ++i)
      tbb::parallel_for( tbb::blocked_range<int>(0,y.size()),
                       [&](tbb::blocked_range<int> r)
      {
          for (int i=r.begin(); i<r.end(); ++i)
          {
            double x = i*h;
            y[i] = std::sin(pow(x, M_PI)) * std::cos(pow(x, M_PI))
               * std::tan(pow(x, M_PI)) - std::atan(exp(x) /(x+1.0) );
          }
      });
      
      auto sum = tbb::parallel_reduce( 
                 tbb::blocked_range<int>(0,y.size()), 0.0,
                 [&](tbb::blocked_range<int> r, double tmp)
                 {
                    for (int i=r.begin(); i<r.end(); ++i)
                    {
                        tmp += y[i];
                    }

                    return tmp;
                 }, std::plus<double>()
                 );
      avg = sum / y.size();
   }
   std::cout << avg << std::endl;
   
   state.counters["DPFlops"] = benchmark::Counter{static_cast<double>(state.iterations() * size),
                               benchmark::Counter::kIsRate,
                               benchmark::Counter::kIs1000};
}
// Register the function as a benchmark
BENCHMARK(BM_SomeFunction)->UseRealTime()->Ranges({{2, 2<<20}, {1, 2}})
                                         ->Ranges({{2, 2<<20}, {4, 8}});
// Run the benchmark
BENCHMARK_MAIN();
