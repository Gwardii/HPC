#include <iostream>
#include <cmath>
#include <fstream>
#include <vector>

using namespace std;

int main()
{
  int nx = 101;
  int nt = 1000;
  int c = 1.0;
  double dx = 1.0 / ( nx - 1 );
  double dt = 0.001;//1.0 / nt;
  double c1 = c * dt / dx;
  double t = 0;
  vector<double> x(nx);
  for(int i=0; i< nx; ++i)
    x[i]=i*dx;
  vector<double> u(nx, 0), unew(nx, 0);

  // initial condition
  for(int i=0; i< nx; ++i)
  {
    if( 0.4 <= x[i] && x[i] <= 0.6 )
      u[i] = 0.5*(-cos(10*M_PI*x[i])+1.0);
  }

  // Euler iteration  
  for(int j=0; j<nt; ++j)
  {
    t=j*dt;
    // 0
    unew[0] = u[0] - c1 * ( 3.0/2.0*u[0] - 2*u[nx-1-1] +1.0/2.0*u[nx-1-2]);
    // 1
    unew[1] = u[1] - c1 * ( 3.0/2.0*u[1] - 2*u[0] +1.0/2.0*u[nx-1-1]);
    for(int i=2; i< nx; ++i)
      unew[i] = u[i] - c1 * ( 3.0/2.0*u[i] - 2*u[i-1] +1.0/2.0*u[i-2]);

    for(int i=0; i< nx; ++i)
        u[i] = unew[i];
  }

  // print solution
	ofstream outfile ("data.dat");
  for(int i=0; i< nx; ++i)
  {
    outfile << i << ", " << x[i] << ", " << u[i] << endl;
  }
  outfile.close();	
  return 0;
}

