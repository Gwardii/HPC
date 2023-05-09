#include <iostream>
#include <vector>
#include <cmath>
#include <mpi.h>

using namespace std;

int main()
{
  // Initialize the MPI environment
  MPI_Init(NULL, NULL);
  int world_size, rank;
  MPI_Comm_size(MPI_COMM_WORLD, &world_size);
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);

  int size = 1000000;
  int local_size = size / world_size;
    
  auto y = std::vector<double>(local_size);
  double h = 4 * M_PI / (size-1);
  double x0 = rank * local_size * h;
  for (int i=0; i<y.size(); ++i)
  {
      double x = x0 + i*h;
      y[i] = std::sin(pow(x, M_PI)) * std::cos(pow(x, M_PI))
          * std::tan(pow(x, M_PI)) - std::atan(exp(x) /(x+1.0) );
  };
    
  double sub_avg = 0;
  for (int i=0; i<y.size(); ++i)
  {
    sub_avg += y[i];
  }
  sub_avg = sub_avg / y.size();

  double avg = 0;
  // All processes will recive the value
  MPI_Allreduce(&sub_avg, &avg, 1, MPI_DOUBLE, MPI_SUM, MPI_COMM_WORLD);
   
  // Compute average.
  avg /= world_size;
  cout << "Calcualted average is for size " << size << "and local_size " << local_size << " is " << avg << endl;

  MPI_Finalize();
}