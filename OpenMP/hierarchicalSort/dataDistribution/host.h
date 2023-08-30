#ifndef DPULEVEL_HOST_H
#define DPULEVEL_HOST_H

#include <stdint.h>
#include <stddef.h>

typedef struct host {
    size_t host_mem_size;
    uint64_t thread_count;

    uint16_t worker_count;
    uint8_t tasklet_count;
    size_t worker_mem_size;

    double timer[2];
} host_t;


#endif //DPULEVEL_HOST_H
