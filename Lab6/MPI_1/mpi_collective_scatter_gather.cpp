#include <mpi.h>
#include <iostream>
#include "MakeRandomVector.hpp"

using namespace std;

/*
    MPI colective communication
    Scatter and Gather

    Process 0 generates a random vector and scatters it to others.
    Processes calculate an average,
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

    // Scatter from 0 to all
    int localsize = 25;
    vector<double> randvec;
    vector<double> localvec(localsize, 0);
    if (rank == 0)
    {
        randvec = makeRandomVector(localsize * world_size);
        cout << randvec.size() << '\n';
        for(auto &it : randvec)
            cout << it << endl;
        cout << " === " << endl;
    }

    // Scatter the random numbers to all processes
    MPI_Scatter(&randvec[0], localsize, MPI_DOUBLE, &localvec[0],
                localsize, MPI_DOUBLE, 0, MPI_COMM_WORLD);

    // What processes recived
    for(auto &it : localvec)
        cout << rank << " " << it << endl;

    // Compute the average of subset
    double sub_avg = 0;
    for(auto &it : localvec)
        sub_avg += it;
    sub_avg /= localvec.size();
    // Gather all partial averages down to the root process
    vector<double> sub_avgs;
    if (rank == 0)
    {
        sub_avgs.resize(world_size);
    }
    MPI_Gather(&sub_avg, 1, MPI_DOUBLE, &sub_avgs[0], 1, MPI_DOUBLE, 0, MPI_COMM_WORLD);

    // Process 0 finishes by computing final average.
    if (rank == 0)
    {
        sub_avg = 0;
        for(auto &it : sub_avgs)
            sub_avg += it;
        sub_avg /= sub_avgs.size();

        cout << " The average is " << sub_avg << endl;
    }

    // Finalize the MPI environment.
    MPI_Finalize();
}
