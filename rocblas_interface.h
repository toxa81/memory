#include "acc.hpp"

namespace rocblas {

void create_stream_handles();
void destroy_stream_handles();

void dgemm(char transa, char transb, int32_t m, int32_t n, int32_t k,
           double const* alpha, double const* a, int32_t lda, double const* b,
           int32_t ldb, double const* beta, double* c, int32_t ldc, stream_id sid);

}
