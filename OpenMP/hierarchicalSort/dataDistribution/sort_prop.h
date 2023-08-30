//
// Created by halfblood on 8/30/23.
//

#ifndef DPULEVEL_SORT_PROP_H
#define DPULEVEL_SORT_PROP_H


typedef enum sort_memory_type {
    IN_PLACE = 0,
    OUT_OF_PLACE = 1
} sort_memory_type_t;

typedef struct sort_props {
    sort_memory_type_t type;
    uint64_t *input;
    uint64_t *output;
    size_t partition_size;
    double *timer; //time_to_sort
} sort_props_t;





#endif //DPULEVEL_SORT_PROP_H
