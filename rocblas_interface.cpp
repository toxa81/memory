#include <rocblas.h>
#include "acc.hpp"

#define CALL_ROCBLAS(func__, args__)                                                \
{                                                                                   \
    rocblas_status status;                                                          \
    if ((status = func__ args__) != rocblas_status_success) {                       \
        printf("error calling rocblas\n");                                          \
        char nm[1024];                                                              \
        gethostname(nm, 1024);                                                      \
        printf("hostname: %s\n", nm);                                               \
        printf("Error in %s at line %i of file %s\n", #func__, __LINE__, __FILE__); \
        stack_backtrace();                                                          \
    }                                                                               \
}

namespace rocblas {

inline rocblas_operation get_rocblas_operation(char c)
{
    switch (c) {
        case 'n':
        case 'N': {
            return rocblas_operation_none;
        }
        case 't':
        case 'T': {
            return rocblas_operation_transpose;
        }
        case 'c':
        case 'C': {
            return rocblas_operation_conjugate_transpose;
        }
        default: {
            throw std::runtime_error("get_rocblas_operation(): wrong operation");
        }
    }
    return rocblas_operation_none; // make compiler happy
}


inline rocblas_handle& null_stream_handle()
{
    static rocblas_handle handle_;
    return handle_;
}

inline std::vector<rocblas_handle>& stream_handles()
{
    static std::vector<rocblas_handle> stream_handles_;
    return stream_handles_;
}

void create_stream_handles()
{
    CALL_ROCBLAS(rocblas_create_handle, (&null_stream_handle()));

    stream_handles() = std::vector<rocblas_handle>(acc::num_streams());
    for (int i = 0; i < acc::num_streams(); i++) {
        CALL_ROCBLAS(rocblas_create_handle, (&stream_handles()[i]));

        CALL_ROCBLAS(rocblas_set_stream, (stream_handles()[i], acc::stream(stream_id(i))));
    }
}

void destroy_stream_handles()
{
    CALL_ROCBLAS(rocblas_destroy_handle, (null_stream_handle()));
    for (int i = 0; i < acc::num_streams(); i++) {
        CALL_ROCBLAS(rocblas_destroy_handle, (stream_handles()[i]));
    }
}

inline rocblas_handle stream_handle(stream_id sid__)
{
    return (sid__() == -1) ? null_stream_handle() : stream_handles()[sid__()];
}

inline void zgemm(char transa, char transb, int32_t m, int32_t n, int32_t k, 
                  rocblas_double_complex const* alpha, rocblas_double_complex const* a, int32_t lda,
                  rocblas_double_complex const* b, int32_t ldb, rocblas_double_complex const* beta,
                  rocblas_double_complex* c, int32_t ldc, stream_id sid)
{
    //CALL_ROCBLAS(rocblas_zgemm, (stream_handle(sid), get_rocblas_operation(transa), get_rocblas_operation(transb),
    //                             m, n, k, alpha, a, lda, b, ldb, beta, c, ldc));
}

void dgemm(char transa, char transb, int32_t m, int32_t n, int32_t k,
           double const* alpha, double const* a, int32_t lda, double const* b,
           int32_t ldb, double const* beta, double* c, int32_t ldc, stream_id sid)
{
    CALL_ROCBLAS(rocblas_dgemm, (stream_handle(sid), get_rocblas_operation(transa), get_rocblas_operation(transb), 
                                 m, n, k, alpha, a, lda, b, ldb, beta, c, ldc));
}

}
