#include <omp.h>
#include <stdio.h>

int main(int argn, char** argv)
{

    #pragma omp parallel
    {
        #pragma omp critical
        printf("OMP thread id: %i\n", omp_get_thread_num());
    }

    return 0;
}
