#include <mpi.h>
#include <iostream>

using namespace std;

/*
    The basic MPI program that initializez the enviroment
    and shows how to get the number of processes and the rank, id of this process
*/

int main(int argc, char** argv)
{
    // Initialize the MPI environment
    MPI_Init(NULL, NULL);

    // Get the number of processes
    int world_size;
    MPI_Comm_size(MPI_COMM_WORLD, &world_size);

    // Get the rank of this process
    int rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    // Get the name of the processor this is running on
    char processor_name[MPI_MAX_PROCESSOR_NAME];
    int name_len;
    MPI_Get_processor_name(processor_name, &name_len);

    // Print off a hello world message
    cout << "Hello world from processor " << processor_name << ", rank " << rank << " out of " << world_size << " processors" << '\n';

    // Finalize the MPI environment.
    MPI_Finalize();
}
