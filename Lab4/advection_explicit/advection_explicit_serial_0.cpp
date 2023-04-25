#include <iostream>
#include <cmath>
#include <fstream>
#include <vector>

using namespace std;

// First order, backward FD
void explicit_euler_bd_1(vector<double> &u, double c, double dt, double dx)
{
  int nx = u.size();
  vector<double> unew(u);
  double c1 = c * dt / dx;
  
  // 0
  unew[0] = u[0] - c1 * ( u[0] - u[nx-1]);
  for(int i=1; i< nx; ++i)
    unew[i] = u[i] - c1 * ( u[i] - u[i-1]);

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
  for(int i=2; i< nx; ++i)
    unew[i] = u[i] - c1 * ( 3.0/2.0*u[i] - 2*u[i-1] +1.0/2.0*u[i-2]);

  u.swap(unew);
}

void dump_solution(vector<double> &x, vector<double> &u, int n)
{
  string fileName;
  fileName = "data_" + to_string(n) + ".csv";
	ofstream outfile (fileName);
  outfile << "#x, u" << endl;
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
  
  for(int i=0; i< nx; ++i)
  {
    x[i]=i*dx;
    if( 0.4 <= x[i] && x[i] <= 0.6 )
      u[i] = 0.5*(-cos(10*M_PI*x[i])+1.0);
  }

  dump_solution(x, u, 0);
  for(int i=1; i<nt; ++i)
  {
    t = i*dt;
    explicit_euler_bd_2(u, c, dt, dx);
    //explicit_euler_bd_1(u, c, dt, dx);
    if(i % 100 == 0)
      dump_solution(x, u, i);
  }
}

int main()
{
  int size = 1000;
  solve_advection(size, 1000000, 1e-4);
}