#include <mpi.h>
#include <iostream>

using namespace std;

/*
    The basic MPI program that
    shows the blocking send recive operation
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

    // Print hello
    cout << "Hello world from rank " << rank << " out of " << world_size << '\n';

    int number;
    if (rank == 0) {
        number = -1;
        MPI_Send(&number, 1, MPI_INT, 1, 0, MPI_COMM_WORLD);
    } else if (rank == 1) {
        MPI_Recv(&number, 1, MPI_INT, 0, 0, MPI_COMM_WORLD,
                MPI_STATUS_IGNORE);
        cout << "Process 1 received number " << number << " from process 0\n";
    }

    // Finalize the MPI environment.
    MPI_Finalize();
}
