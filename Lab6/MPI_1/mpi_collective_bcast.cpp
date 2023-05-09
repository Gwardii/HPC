#include <mpi.h>
#include <iostream>

using namespace std;

/*
    MPI colective communication
    Show broadcast, where one sends to many
    
    MPI_Bcast()
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

    // One to many - Bcast
    int message = rank;
    cout << "Hello world from rank " << rank << " message before BCast is " << message << '\n';

    // Send message from one to many
    MPI_Bcast(&message, 1, MPI_INT, 1, MPI_COMM_WORLD);

    cout << "Hello world from rank " << rank << " message after BCast is " << message << '\n';

    // Finalize the MPI environment.
    MPI_Finalize();
}
