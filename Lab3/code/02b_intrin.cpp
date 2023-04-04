#include "MakeRandomVector.hpp"
#include "benchmark/benchmark.h"
#include "immintrin.h"
#include <memory>

template < typename T >
requires std::floating_point< T > || std::integral< T >
auto makeRandomAlignedArray(std::size_t size      = 100,
                            std::size_t alignment = 64,
                            T           min       = std::numeric_limits< T >::min(),
                            T           max       = std::numeric_limits< T >::max())
{
    thread_local std::mt19937 prng{std::random_device{}()};
    rand_dist_t< T >          random_dist{min, max};

    std::unique_ptr< T[] > result{new (std::align_val_t{alignment}) T[size]};
    std::generate_n(result.get(), size, [&] { return random_dist(prng); });
    return result;
}

void BM_vec_quad(benchmark::State& state)
{
    const std::size_t size = state.range(0);
    const auto        src  = makeRandomAlignedArray< double >(size);
    auto              dest = makeRandomAlignedArray< double >(size);

    const __m256d coef     = _mm256_set1_pd(3.5); // 256b vector register {3.5, 3.5, 3.5, 3.5}
    const __m256d constant = _mm256_set1_pd(1.);  // 256b vector register {1., 1., 1., 1.}
    __m256d       x;
    __m256d       temp;

    for (auto _ : state)
    {
        for (std::ptrdiff_t i = 0; i < size; i += 4)
        {
            x    = _mm256_load_pd(src.get() + i);
            temp = _mm256_add_pd(x, coef);
            x    = _mm256_fmadd_pd(x, temp, constant);
            _mm256_store_pd(dest.get() + i, x);
        }
        benchmark::DoNotOptimize(dest.get());
        benchmark::ClobberMemory();
    }
    state.SetBytesProcessed(sizeof(double) * size * state.iterations());
}
BENCHMARK(BM_vec_quad)->RangeMultiplier(2)->Range(1 << 10, 1 << 18);
BENCHMARK_MAIN();
