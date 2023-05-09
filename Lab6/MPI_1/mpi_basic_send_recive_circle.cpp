#include <mpi.h>
#include <iostream>

using namespace std;

/*
    The basic MPI program that
    uses blocing send, recive to pass a message from
    process to process.
    This ezample may be used to show a deadlock.
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

    // Processes send message in a circle
    int message = 0;
    int tag = 0;
    int send_to     = rank + 1 == world_size ? 0              : rank + 1;
    int recive_from = rank     == 0          ? world_size - 1 : rank - 1;
    if (rank == 0)
    {
        // Mind the deadlock here, make sure the first sends, than recives
        MPI_Send(&message, 1, MPI_INT, send_to, tag, MPI_COMM_WORLD);
        cout << "Process " << rank << " send " << message << " to process " << send_to << endl;

        MPI_Recv(&message, 1, MPI_INT, recive_from, tag, MPI_COMM_WORLD,
                MPI_STATUS_IGNORE);
        cout << "Process " << rank << " received " << message << " from process " << recive_from << endl;
    }
    else
    {
        // Mind the order here, make sure processes first recive, than send
        cout << "Process " << rank << " receiving from " << recive_from << endl;
        MPI_Recv(&message, 1, MPI_INT, recive_from, tag, MPI_COMM_WORLD,
                MPI_STATUS_IGNORE);
        cout << "Process " << rank << " received " << message << " from process " << recive_from << endl;
        
        message++;
        MPI_Send(&message, 1, MPI_INT, send_to, tag, MPI_COMM_WORLD);
        cout << "Process " << rank << " send " << message << " to process " << send_to << endl;
    }

    // Finalize the MPI environment.
    MPI_Finalize();
}
