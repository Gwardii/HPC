#include <iostream>
#include <cmath>
#include <fstream>
#include <vector>

#include "benchmark/benchmark.h"

#include <tbb/global_control.h>
#include <tbb/parallel_reduce.h>
#include <tbb/parallel_for.h>

using namespace std;

double nrm(vector<double> &u, vector<double> &unew)
{
  double v = fabs(u[0] - unew[0]);
  v = tbb::parallel_reduce( 
    tbb::blocked_range<int>(1,u.size()), v,
    [&](tbb::blocked_range<int> r, double tmp){
  for(int i=r.begin(); i< r.end(); ++i)
  {
    tmp += fabs(u[i] - unew[i]);
  }
  return tmp;}, std::plus<double>());
  return v;
}

// Second order, central FD
void implicit_euler_cn_2(vector<double> &u, vector<double> &b, vector<double> &un, double c, double dt, double dx, int N=10000, double eps=1.0e-6)
{
  int nx = u.size();
  double c1 = 0.5 * c * dt / dx;
  double err;
  
  // Jacobi iterations
  for(int i=0; i<N; ++i)
  {
    // 1/a_ii(u_i - sum a_ij uj)
    // 0
    un[0] = b[0] - (-c1 * u[nx-1] + c1*u[1]);
    // nx-1
    un[nx-1] = b[nx-1] - (- c1 * u[nx-2] + c1*u[0] );
    tbb::parallel_for( tbb::blocked_range<int>(1,nx-1), [&](tbb::blocked_range<int> r)
    {
    for(int i=r.begin(); i<r.end(); ++i)
      un[i] = b[i] - (-c1 * u[i-1] + c1 * u[i+1]);
      }
    );

    err = nrm(u, un);
    
    if(err < eps)
    {
      // cout << "Converged in " <<  i << " with err " << err << endl;
      u.swap(un);
      return;
    }
    u.swap(un);
  }
  cout << "Failed to conerge in " << N << " with err " << err << endl;
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
  dt = 0.4*2*dx/c;
  double t = 0;
  vector<double> x(nx);
  vector<double> u(nx, 0);
  // initial condition
  tbb::parallel_for(tbb::blocked_range<int>(0,nx), [&](tbb::blocked_range<int> r){
    for(int i=r.begin(); i< r.end(); ++i)
      {
        x[i]=i*dx;
        if( 0.4 <= x[i] && x[i] <= 0.6 )
          u[i] = 0.5*(-cos(10*M_PI*x[i])+1.0);
      }
  });
    
  vector<double> b(u);
  vector<double> un(u);

  dump_solution(x, u, 0);
  for(int i=1; i<1000; ++i)
  {
    t = i*dt;
    implicit_euler_cn_2(u, b, un, c, dt, dx);
    b = u;
    // if(i % 25 == 0)
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
BENCHMARK(BM_advection)->UseRealTime()->Ranges({{8, 2<<15},{1,2}})->Ranges({{8, 2<<15},{4,8}});
// Run the benchmark
BENCHMARK_MAIN();