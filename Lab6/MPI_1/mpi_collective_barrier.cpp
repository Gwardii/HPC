#include <mpi.h>
#include <iostream>

using namespace std;

/*
    MPI colective communication
    MPIBarrier()

    Show deadlock
*/

int main(int argc, char** argv) {
    // Initialize the MPI environment
    MPI_Init(NULL, NULL);

    // Get the number of processes
    int world_size;
    MPI_Comm_size(MPI_COMM_WORLD, &world_size);

    // Get the rank of the process
    int rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    // Print off a hello world message
    cout << "Hello world from rank " << rank << " out of " << world_size << '\n';
    // Synchronization
    //if(rank == 0) // Deadlock!
    //MPI_Barrier(MPI_Comm MPI_COMM_WORLD);
    cout << "Process with rank " << rank << " Passed barrier" << '\n';

    // Finalize the MPI environment.
    MPI_Finalize();
}
