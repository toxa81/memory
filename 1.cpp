#include "acc.hpp"

int main(int argn, char** argv)
{
    int nd = acc::num_devices();
    printf("number of devices: %i\n", nd);
    for (int i = 0; i < nd; i++) {
        printf("device: %i\n", i);
        acc::print_device_info(i);
    }
    return 0;
}
