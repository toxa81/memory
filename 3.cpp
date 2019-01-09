#include "memory.hpp"
#include <pthread.h>
#include <omp.h>
#include <thread>
#include "rocblas_interface.h"

using namespace sddk;

void* f1(void* arg)
{
    printf("in f1()\n");
    int id = *((int*)arg);
    acc::set_device_id(id);
    int id1 = acc::get_device_id();
    if (id1 != id) {
        printf("wrong device id\n");
    }
    return nullptr;
}

void* f2(void* arg)
{
    printf("in f2()\n");
    int id = acc::get_device_id();
    printf("pthread: current device id : %i\n", id);
    return nullptr;
}


int main(int argn, char** argv)
{
    //int nd = acc::num_devices();
    //printf("number of devices: %i\n", nd);
    //for (int i = 0; i < nd; i++) {
    //    printf("device: %i\n", i);
    //    acc::print_device_info(i);
    //}
    int n{4096};

    mdarray<double, 2> a(n, n);
    mdarray<double, 2> b(n, n);
    mdarray<double, 2> c(n, n);

    a.allocate(memory_t::device);
    b.allocate(memory_t::device);
    c.allocate(memory_t::device);

    rocblas::create_stream_handles();

    double alpha{1};
    double beta{0};

    double t = -omp_get_wtime();
    rocblas::dgemm('n', 'n', n, n, n, &alpha, a.at(memory_t::device), a.ld(), b.at(memory_t::device), b.ld(), &beta,
                   c.at(memory_t::device), c.ld(), stream_id(-1));
    
    c.copy_to(memory_t::host);
    t += omp_get_wtime();

    t = -omp_get_wtime();
    rocblas::dgemm('n', 'n', n, n, n, &alpha, a.at(memory_t::device), a.ld(), b.at(memory_t::device), b.ld(), &beta,
                   c.at(memory_t::device), c.ld(), stream_id(-1));
    c.copy_to(memory_t::host);
    t += omp_get_wtime();

    printf("performance : %f GFlops\n", 2e-9 * n * n * n / t);

    rocblas::destroy_stream_handles();

}
