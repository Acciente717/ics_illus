#include <immintrin.h>
void fp_add_vector_double(const double *src1, const double *src2, double *dst)
{
    __m256d x, y;
    x = _mm256_load_pd(src1);
    y = _mm256_load_pd(src2);
    y = x + y;
    _mm256_store_pd(dst, y);
}
