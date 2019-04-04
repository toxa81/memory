#include <sys/time.h>
#include <stdlib.h>
#include "memory.hpp"

using namespace sddk;

/// Wall-clock time in seconds.
inline double wtime()
{
    timeval t;
    gettimeofday(&t, NULL);
    return double(t.tv_sec) + double(t.tv_usec) / 1e6;
}

double test_alloc(size_t n)
{
    double t0 = wtime();
    /* time to allocate + fill */
    char* ptr = (char*)std::malloc(n);
    std::fill(ptr, ptr + n, 0);
    double t1 = wtime();
    /* time fo fill */
    std::fill(ptr, ptr + n, 0);
    double t2 = wtime();
    /* harmless: add zero to t0 to prevent full optimization of the code with GCC */
    t0 += ptr[0];
    std::free(ptr);
    double t3 = wtime();

    return (t3 - t0) - 2 * (t2 - t1);
}

double test_alloc(size_t n, memory_pool& mp)
{
    double t0 = wtime();
    /* time to allocate + fill */
    char* ptr = mp.allocate<char>(n);
    std::fill(ptr, ptr + n, 0);
    double t1 = wtime();
    /* time fo fill */
    std::fill(ptr, ptr + n, 0);
    double t2 = wtime();
    /* harmless: add zero to t0 to prevent full optimization of the code with GCC */
    t0 += ptr[0];
    mp.free(ptr);
    double t3 = wtime();

    return (t3 - t0) - 2 * (t2 - t1);
}

void test6()
{
    double t0{0};
    for (int k = 0; k < 8; k++) {
        for (int i = 10; i < 30; i++) {
            size_t sz = size_t(1) << i;
            t0 += test_alloc(sz);
        }
    }
    memory_pool mp(memory_t::host);
    double t1{0};
    for (int k = 0; k < 8; k++) {
        for (int i = 10; i < 30; i++) {
            size_t sz = size_t(1) << i;
            t1 += test_alloc(sz, mp);
        }
    }
    std::cout << "std::malloc time: " << t0 << ", sddk::memory_pool time: " << t1 << "\n";
}

void test6a()
{
    double t0{0};
    for (int k = 0; k < 500; k++) {
        for (int i = 2; i < 1024; i++) {
            size_t sz = i;
            t0 += test_alloc(sz);
        }
    }
    memory_pool mp(memory_t::host);
    double t1{0};
    for (int k = 0; k < 500; k++) {
        for (int i = 2; i < 1024; i++) {
            size_t sz = i;
            t1 += test_alloc(sz, mp);
        }
    }
    std::cout << "std::malloc time: " << t0 << ", sddk::memory_pool time: " << t1 << "\n";
}

void test7()
{
    memory_pool mp(memory_t::host);

    int N = 10000;
    std::vector<double*> v(N);
    for (int k = 0; k < 30; k++) {
        for (int i = 0; i < N; i++) {
            auto n = (rand() & 0b1111111111) + 1;
            v[i] = mp.allocate<double>(n);
        }
        std::random_shuffle(v.begin(), v.end());
        for (int i = 0; i < N; i++) {
            mp.free(v[i]);
        }
        if (mp.free_size() != mp.total_size()) {
            throw std::runtime_error("wrong free size");
        }
        if (mp.num_blocks() != 1) {
            throw std::runtime_error("wrong number of blocks");
        }
        if (mp.num_stored_ptr() != 0) {
            throw std::runtime_error("wrong number of stored pointers");
        }
    }
}

int main(int argn, char** argv)
{
    /* small number of big allocations */
    test6();
    /* large number of small allocations */
    test6a();
    /* correctness */
    test7();
}
