#include "memory.hpp"
#include <pthread.h>

using namespace sddk;

void* f1(void* arg)
{
    int id = *((int*)arg);
    acc::set_device_id(id);
    printf("setting id: %i\n", id);
    return nullptr;
}

void* f2(void* arg)
{
    int id = acc::get_device_id();
    printf("current id : %i\n", id);
    return nullptr;
}


int main(int argn, char** argv)
{
    int nd = acc::num_devices();
    printf("number of devices: %i\n", nd);
    for (int i = 0; i < nd; i++) {
        printf("device: %i\n", i);
        acc::print_device_info(i);
    }
    mdarray<double, 2> a(10, 10);
    a.allocate(memory_t::device);
    a.copy_to(memory_t::device);


    std::vector<pthread_t> threads(10);

    int id{1};
    for (int i = 0; i < 10; i++) {
        if (pthread_create(&threads[i], NULL, f1, &id)) {
            printf("Error creating thread\n");
            return 1;
        }
    }

    for (int i = 0; i < 10; i++) {
        if (pthread_join(threads[i], NULL)) {
            printf("Error joining thread\n");
            return 2;
        }
    }

    for (int i = 0; i < 10; i++) {
        if (pthread_create(&threads[i], NULL, f2, nullptr)) {
            printf("Error creating thread\n");
            return 1;
        }
    }

    for (int i = 0; i < 10; i++) {
        if (pthread_join(threads[i], NULL)) {
            printf("Error joining thread\n");
            return 2;
        }
    }


    return 0;
}
