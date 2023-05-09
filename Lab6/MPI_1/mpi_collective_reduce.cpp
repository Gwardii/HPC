#include <mpi.h>
#include <iostream>
#include "MakeRandomVector.hpp"

using namespace std;

/*
    MPI colective communication
    Reduce - result of reduction goes to one process
    AllReduce - all processes get the result

    Processes generate random subvectors
    Process 0 Gathers results from others
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

    // Scatter, Gather
    int localsize = 25;
    vector<double> localvec = makeRandomVector(localsize);

    for(auto &it : localvec)
        cout << rank << " " << it << endl;
    
    // Compute the average of rank's subset
    double sub_avg = 0;
    for(auto &it : localvec)
        sub_avg += it;
    sub_avg /= localvec.size();
    
    MPI_Barrier(MPI_COMM_WORLD);

    double avg = 0;
    // Process 0 will recive the value
    MPI_Reduce(&sub_avg, &avg, 1, MPI_DOUBLE, MPI_SUM, 0, MPI_COMM_WORLD);
    cout << "Hello world from rank " << rank << " value is " << avg << '\n';

    MPI_Barrier(MPI_COMM_WORLD);
    // All processes will recive the value
    MPI_Allreduce(&sub_avg, &avg, 1, MPI_DOUBLE, MPI_SUM, MPI_COMM_WORLD);
    cout << "Hello world from rank " << rank << " value is " << avg << '\n';
    
    // Compute average.
    avg /= world_size;

    MPI_Barrier(MPI_COMM_WORLD);

    cout << " The average at " << rank << "  is " << avg << endl;

    // Finalize the MPI environment.
    MPI_Finalize();
}
