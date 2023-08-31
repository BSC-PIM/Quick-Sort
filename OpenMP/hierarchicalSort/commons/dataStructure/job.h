//
// Created by halfblood on 8/8/23.
//

#ifndef MERGEIDEA_JOB_H
#define MERGEIDEA_JOB_H

#include <stdint.h>

typedef struct sort_job {
    size_t start;
    size_t end;
} sort_job_t;

typedef struct merge_job {
    uint16_t w_first;
    uint16_t w_last;
    size_t start; // start index of the first worker
    size_t end;   // last index of the last worker
} merge_job_t;

#endif //MERGEIDEA_JOB_H
