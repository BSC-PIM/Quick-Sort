#ifndef DPULEVEL_SORT_PROP_H
#define DPULEVEL_SORT_PROP_H


#include "config.h"


typedef enum sort_memory_type {
    IN_PLACE = 0,
    OUT_OF_PLACE = 1
} sort_memory_type_t;

typedef struct sort_props {
    sort_memory_type_t type;
    T *input;
    T *output;
    size_t element_in_partition;
    double *timer; //time_to_sort
} sort_props_t;





#endif //DPULEVEL_SORT_PROP_H
