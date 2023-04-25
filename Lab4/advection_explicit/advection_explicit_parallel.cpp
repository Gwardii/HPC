#include <iostream>
#include <cmath>
#include <fstream>
#include <vector>

#include "benchmark/benchmark.h"

#include <tbb/global_control.h>
#include <tbb/parallel_for.h>


using namespace std;

// First order, backward FD
void explicit_euler_bd_1(vector<double> &u, double c, double dt, double dx)
{
  int nx = u.size();
  vector<double> unew(u);
  double c1 = c * dt / dx;
  
  // 0
  unew[0] = u[0] - c1 * ( u[0] - u[nx-1]);
  
  tbb::parallel_for( tbb::blocked_range<int>(1, nx), [&](tbb::blocked_range<int> r){
  for(int i=r.begin(); i< r.end(); ++i){
    unew[i] = u[i] - c1 * ( u[i] - u[i-1]);
    }
  });

  u.swap(unew);
}

// Second order, backward FD
void explicit_euler_bd_2(vector<double> &u, double c, double dt, double dx)
{
  int nx = u.size();
  vector<double> unew(u);
  double c1 = c * dt / dx;
  // 0
  unew[0] = u[0] - c1 * ( 3.0/2.0*u[0] - 2*u[nx-1-1] +1.0/2.0*u[nx-1-2]);
  // 1
  unew[1] = u[1] - c1 * ( 3.0/2.0*u[1] - 2*u[0] +1.0/2.0*u[nx-1-1]);

  tbb::parallel_for( tbb::blocked_range<int>(2, nx), [&](tbb::blocked_range<int> r){
  for(int i=r.begin(); i< r.end(); ++i)
    unew[i] = u[i] - c1 * ( 3.0/2.0*u[i] - 2*u[i-1] +1.0/2.0*u[i-2]);
	});
  u.swap(unew);
}

void dump_solution(vector<double> &x, vector<double> &u, int n)
{
  string fileName;
  fileName = "data_" + to_string(n) + ".csv";
	ofstream outfile (fileName);
  for(int i=0; i< x.size(); ++i)
  {
    outfile << x[i] << ", " << u[i] << endl;
  }
  outfile.close();
}

void solve_advection(int nx, int nt, double dt = 0.001)
{
  int c = 1.0;
  double dx = 1.0 / ( nx - 1 );
  
  double t = 0;
  vector<double> x(nx);
  vector<double> u(nx, 0);
  
  tbb::parallel_for( tbb::blocked_range<int>(0, nx), [&](tbb::blocked_range<int> r){
  for(int i=r.begin(); i< r.end(); ++i)
  {
    x[i]=i*dx;
    if( 0.4 <= x[i] && x[i] <= 0.6 )
      u[i] = 0.5*(-cos(10*M_PI*x[i])+1.0);
  }
  });

  // dump_solution(x, u, 0);
  for(int i=1; i<nt; ++i)
  {
    t = i*dt;
    explicit_euler_bd_2(u, c, dt, dx);
    //explicit_euler_bd_1(u, c, dt, dx);
    // if(i % 100 == 0)
    //   dump_solution(x, u, i);
  }
}

void BM_advection(benchmark::State& state)
{
  int size = state.range(0);
  tbb::global_control c(tbb::global_control::max_allowed_parallelism, state.range(1));

  for (auto _ : state)
  {
    solve_advection(size, 100, 1e-4);
  }

  state.counters["SPD"] = benchmark::Counter{static_cast<double>(state.iterations() * size * 100),
                              benchmark::Counter::kIsRate,
                              benchmark::Counter::kIs1000};

}

// Register the function as a benchmark
BENCHMARK(BM_advection)->UseRealTime()->Ranges({{2, 2<<20}, {1, 2}})
                                      ->Ranges({{2, 2<<20}, {4, 8}});
// Run the benchmark
BENCHMARK_MAIN();
