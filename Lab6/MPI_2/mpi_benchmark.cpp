#include <benchmark/benchmark.h>
#include <chrono>
#include <cmath>
#include <mpi.h>
#include <thread>

namespace {
// The unit of code we want to benchmark
void BM_SomeFunction(benchmark::State &state) {
  int size = state.range(0);

  int world_size, rank;
  MPI_Comm_size(MPI_COMM_WORLD, &world_size);
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);

  int local_size = size / world_size;

  auto y = std::vector<double>(local_size);
  double h = 4 * M_PI / (size - 1);
  double x0 = rank * local_size * h;
  for (int i = 0; i < y.size(); ++i) {
    double x = x0 + i * h;
    y[i] = std::sin(pow(x, M_PI)) * std::cos(pow(x, M_PI)) *
               std::tan(pow(x, M_PI)) -
           std::atan(exp(x) / (x + 1.0));
  }

  double sub_avg = 0;
  for (int i = 0; i < y.size(); ++i) {
    sub_avg += y[i];
  }
  sub_avg = sub_avg / y.size();

  double avg = 0;
  // All processes will recive the value
  MPI_Allreduce(&sub_avg, &avg, 1, MPI_DOUBLE, MPI_SUM, MPI_COMM_WORLD);

  // Compute average.
  avg /= world_size;
}

void mpi_benchmark(benchmark::State &state) {
  double max_elapsed_second;
  while (state.KeepRunning()) {
    // Do the work and time it on each proc
    auto start = std::chrono::high_resolution_clock::now();

    BM_SomeFunction(state);

    auto end = std::chrono::high_resolution_clock::now();

    MPI_Barrier(MPI_COMM_WORLD);
    // Now get the max time across all procs:
    // for better or for worse, the slowest processor is the one that is
    // holding back the others in the benchmark.
    auto const duration =
        std::chrono::duration_cast<std::chrono::duration<double>>(end - start);
    auto elapsed_seconds = duration.count();
    MPI_Allreduce(&elapsed_seconds, &max_elapsed_second, 1, MPI_DOUBLE, MPI_MAX,
                  MPI_COMM_WORLD);
    state.SetIterationTime(max_elapsed_second);
  }
}
} // namespace

BENCHMARK(mpi_benchmark)->UseManualTime()->Range(64, 8 << 22);

// This reporter does nothing.
// We can use it to disable output from all but the root process
class NullReporter : public ::benchmark::BenchmarkReporter {
public:
  NullReporter() {}
  virtual bool ReportContext(const Context &) { return true; }
  virtual void ReportRuns(const std::vector<Run> &) {}
  virtual void Finalize() {}
};

// Init MPI, silence all but 0
int main(int argc, char **argv) {
  MPI_Init(NULL, NULL);

  int rank;
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);

  if (rank == 0)
    // root process will use a reporter from the usual set provided by
    // ::benchmark
    benchmark::RunSpecifiedBenchmarks();
  else {
    // reporting from other processes is disabled by passing a custom reporter
    NullReporter nullrep;
    benchmark::RunSpecifiedBenchmarks(&nullrep);
  }

  MPI_Finalize();
  return 0;
}
