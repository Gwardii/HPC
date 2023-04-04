#include <immintrin.h>

#include <algorithm>
#include <limits>
#include <random>
#include <vector>

constexpr std::size_t panel_width  = 64;
constexpr std::size_t sliver_width = 4;

void multSliverSliver(const double* A_sliver, const double* B_sliver, double* C_pixel)
{
    __m256d A_regs[2];
    __m256d B_regs[6];
    __m256d C_acc[8];

    for (size_t i = 0; i != 8; ++i)
        C_acc[i] = _mm256_setzero_pd();

    for (size_t i = 0; i != panel_width; i += 2)
    {
        A_regs[0] = _mm256_load_pd(A_sliver + i * 4);
        A_regs[1] = _mm256_load_pd(A_sliver + i * 4 + 4);
        for (size_t j = 0; j != 6; ++j)
            B_regs[j] = _mm256_broadcast_sd(B_sliver + i * 4 + j);
        for (size_t j = 0; j != 4; ++j)
            C_acc[j] = _mm256_fmadd_pd(A_regs[0], B_regs[j], C_acc[j]);
        for (size_t j = 4; j != 6; ++j)
            C_acc[j] = _mm256_fmadd_pd(A_regs[1], B_regs[j], C_acc[j]);
        B_regs[0] = _mm256_broadcast_sd(B_sliver + i * 4 + 6);
        B_regs[1] = _mm256_broadcast_sd(B_sliver + i * 4 + 7);
        C_acc[6]  = _mm256_fmadd_pd(A_regs[1], B_regs[0], C_acc[6]);
        C_acc[7]  = _mm256_fmadd_pd(A_regs[1], B_regs[1], C_acc[7]);
    }

    for (size_t i = 0; i != 4; ++i)
        _mm256_store_pd(C_pixel + i * 4, _mm256_add_pd(C_acc[i], C_acc[i + 4]));
}
