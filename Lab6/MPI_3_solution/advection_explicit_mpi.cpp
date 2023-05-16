#include <cmath>
#include <fstream>
#include <iostream>
#include <mpi.h>
#include <vector>

using namespace std;

/*
    Solve linear advection problem with MPI
*/

// First order, backward FD
void explicit_euler_bd_1(vector<double> &u, double c, double dt, double dx,
                         int rank, int world_size) {
  // In backward Euler I need the last value of the previous process
  double val;
  int source = rank - 1 < 0 ? world_size - 1 : rank - 1;
  int destin = rank + 1 == world_size ? 0 : rank + 1;

  // Set a nonblocking send / recive
  MPI_Request req_send, req_reci;
  MPI_Status stat_send, stat_reci;
  MPI_Irecv(&val, 1, MPI_DOUBLE, source, 0, MPI_COMM_WORLD, &req_reci);
  MPI_Isend(&(u.back()), 1, MPI_DOUBLE, destin, 0, MPI_COMM_WORLD, &req_send);

  int lnx = u.size();
  vector<double> unew(u);
  double c1 = c * dt / dx;

  for (int i = 1; i < lnx; ++i)
    unew[i] = u[i] - c1 * (u[i] - u[i - 1]);

  MPI_Wait(&req_reci, &stat_reci);

  unew[0] = u[0] - c1 * (u[0] - val);

  u.swap(unew);

  MPI_Wait(&req_send, &stat_send);
}

// Second order, backward FD
void explicit_euler_bd_2(vector<double> &u, double c, double dt, double dx,
                         int rank, int world_size) {
  // In backward Euler of 2'nd order I need the last two values of the previous
  // process
  double val[2];
  int source = rank - 1 < 0 ? world_size - 1 : rank - 1;
  int destin = rank + 1 == world_size ? 0 : rank + 1;

  // Set a nonblocking send / recive
  MPI_Request req_send, req_reci;
  MPI_Status stat_send, stat_reci;
  MPI_Irecv(&val, 2, MPI_DOUBLE, source, 0, MPI_COMM_WORLD, &req_reci);
  MPI_Isend(&(u[u.size() - 2]), 2, MPI_DOUBLE, destin, 0, MPI_COMM_WORLD,
            &req_send);

  int lnx = u.size();
  vector<double> unew(u);
  double c1 = c * dt / dx;

  for (int i = 2; i < lnx; ++i)
    unew[i] =
        u[i] - c1 * (3.0 / 2.0 * u[i] - 2 * u[i - 1] + 1.0 / 2.0 * u[i - 2]);

  MPI_Wait(&req_reci, &stat_reci);

  // 0
  unew[0] = u[0] - c1 * (3.0 / 2.0 * u[0] - 2 * val[1] + 1.0 / 2.0 * val[0]);
  // 1
  unew[1] = u[1] - c1 * (3.0 / 2.0 * u[1] - 2 * u[0] + 1.0 / 2.0 * val[1]);

  u.swap(unew);

  MPI_Wait(&req_send, &stat_send);
}

// Perform a dump in order, note this is a blocking operation
void dump_solution(vector<double> &x, vector<double> &u, int n, int rank,
                   int world_size) {
  string fileName;
  fileName = "data_" + to_string(n) + ".csv";
  ofstream outfile;
  if (rank == 0) {
    outfile.open(fileName);
    outfile << "x, u, rank" << endl;
  } else {
    MPI_Recv(&n, 1, MPI_INT, rank - 1, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    outfile.open(fileName, std::ios_base::app);
  }

  for (int i = 0; i < x.size(); ++i) {
    outfile << x[i] << ", " << u[i] << ", " << rank << endl;
  }

  outfile.close();
  if (rank != world_size - 1) {
    MPI_Send(&rank, 1, MPI_INT, rank + 1, 0, MPI_COMM_WORLD);
  }
}

int main() {
  // Initialize the MPI environment
  MPI_Init(NULL, NULL);
  int world_size, rank;
  MPI_Comm_size(MPI_COMM_WORLD, &world_size);
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);

  int nt = 10000;
  double dt = 0.0001;
  int c = 1.0;

  int nx = 4096;
  int lnx = nx / world_size;
  double dx = 1.0 / (nx - 1);

  double t = 0;
  vector<double> x(lnx);
  vector<double> u(lnx, 0);

  for (int i = 0; i < lnx; ++i) {
    x[i] = rank * lnx * dx + i * dx;
    if (0.4 <= x[i] && x[i] <= 0.6)
      u[i] = 0.5 * (-cos(10 * M_PI * x[i]) + 1.0);
  }

  dump_solution(x, u, 0, rank, world_size);
  for (int i = 1; i < nt; ++i) {
    t = i * dt;
    // explicit_euler_bd_1(u, c, dt, dx, rank, world_size);
    explicit_euler_bd_1(u, c, dt, dx, rank, world_size);
    if (i % 100 == 0)
      dump_solution(x, u, i, rank, world_size);
  }

  MPI_Finalize();
}
