#include "memory.hpp"
#include <pthread.h>
#include <omp.h>
#include <thread>

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
    //mdarray<double, 2> a(10, 10);
    //a.allocate(memory_t::device);
    //a.copy_to(memory_t::device);


    std::vector<pthread_t> threads(4);

    int id{1};
    printf("creating pthreads\n");
    for (int i = 0; i < 4; i++) {
        if (pthread_create(&threads[i], NULL, f1, &id)) {
            printf("Error creating thread\n");
            return 1;
        }
    }

    printf("joining pthreads\n");
    for (int i = 0; i < 4; i++) {
        if (pthread_join(threads[i], NULL)) {
            printf("Error joining thread\n");
            return 2;
        }
    }

    printf("creating pthreads\n");
    for (int i = 0; i < 4; i++) {
        if (pthread_create(&threads[i], NULL, f2, nullptr)) {
            printf("Error creating thread\n");
            return 1;
        }
    }

    printf("joining pthreads\n");
    for (int i = 0; i < 4; i++) {
        if (pthread_join(threads[i], NULL)) {
            printf("Error joining thread\n");
            return 2;
        }
    }

    printf("1st omp section\n");
    #pragma omp parallel
    {
        #pragma omp critical
        {
            acc::set_device_id(1);
            int id1 = acc::get_device_id();
            if (id1 != 1) {
                printf("wrong device id\n");
            }
        }
    }

    printf("2nd omp section\n");
    #pragma omp parallel
    {
        int id = acc::get_device_id();
        printf("omp thread: current device id : %i\n", id);
    }

    std::vector<std::thread> std_threads;
    for (int i = 0; i < 4; i++) {
        std_threads.push_back(std::thread([]()
        {
            acc::set_device_id(1);
            int id1 = acc::get_device_id();
            if (id1 != 1) {
                printf("wrong device id\n");
            }
        }));
    }
    for (auto& thread: std_threads) {
        thread.join();
    }
    std_threads.clear();

    for (int i = 0; i < 4; i++) {
        std_threads.push_back(std::thread([]()
        {
            int id = acc::get_device_id();
            printf("std::thread: current device id : %i\n", id);
        }));
    }
    for (auto& thread: std_threads) {
        thread.join();
    }
    std_threads.clear();

    return 0;
}
