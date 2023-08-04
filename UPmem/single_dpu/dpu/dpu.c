#include <mram.h>


#define DPU_BUFFER_SIZE (1 << 4) // 64 elements

__host int pre_determined_pivot;

__mram_noinit uint32_t
buffer[DPU_BUFFER_SIZE];


#include <stdio.h>


typedef enum {
    RANDOM,
    FIRST,
    LAST,
    MEDIAN,
    PREDETERMINED
} pivot_type;

typedef struct pivot {
    uint32_t value;
    int index;
} pivot;


static
void swap(uint32_t *arr, int i, int j) {
    uint32_t temp = arr[i];
    arr[i] = arr[j];
    arr[j] = temp;
}
//
//static
//pivot choose_random_pivot(uint32_t *arr, int low, int high) {
//    int pivot_index = low + rand() % (high - low + 1);
//    return (pivot) {arr[pivot_index], pivot_index};
//}

static
pivot choose_first_pivot(uint32_t *arr, int low, int high) {
    return (pivot) {arr[low], low};
}

static
pivot choose_last_pivot(uint32_t *arr, int low, int high) {
    return (pivot) {arr[high], high};
}

static
pivot choose_predetermined_pivot(uint32_t *arr, int low, int high) {
    return (pivot) {pre_determined_pivot, 0};
}

static
pivot choose_pivot(uint32_t *arr, int low, int high, pivot_type type) {
    switch (type) {
//        case RANDOM:
//            return choose_random_pivot(arr, low, high);
        case FIRST:
            return choose_first_pivot(arr, low, high);
        case LAST:
            return choose_last_pivot(arr, low, high);
        default:
            return choose_predetermined_pivot(arr, low, high);
    }
}

static
int partition(uint32_t *arr, int low, int high, pivot_type type) {
    pivot p = choose_pivot(arr, low, high, type);

    if (type == PREDETERMINED) {
        // the index is not valid for the predetermined pivot. use two directions partitioning algorithm
        int i = low;
        int j = high - 1;
        while (i < j) {
            while (i < high && arr[i] <= p.value) {
                i++;
            }
            while (j > 0 && arr[j] > p.value) {
                j--;
            }
            if (i < j) {
                uint32_t tmp = arr[i];
                arr[i] = arr[j];
                arr[j] = tmp;
            }
        }
        return i;

    } else {
        swap(arr, low, p.index);
        int i = low + 1;
        for (int j = low + 1; j < high; j++) {
            if (arr[j] < p.value) {
                swap(arr, i, j);
                i++;
            }
        }
        swap(arr, low, i - 1);
        return i - 1;
    }
}


void quicksort(uint32_t *arr, int low, int high, pivot_type type) {

    if (type == PREDETERMINED) {
        int pivot_index = partition(arr, low, high, type);
        quicksort(arr, low, pivot_index, FIRST);
        quicksort(arr, pivot_index, high, FIRST);

    } else {
        if (low < high) {
            int pivot_index = partition(arr, low, high, type);
            quicksort(arr, low, pivot_index, type);
            quicksort(arr, pivot_index + 1, high, type);
        }
    }
}


int main(void) {
/* load the data from MRAM */
    __dma_aligned uint32_t work_buffer[DPU_BUFFER_SIZE];
    mram_read(&buffer, &work_buffer, DPU_BUFFER_SIZE * sizeof(uint32_t));
    quicksort(work_buffer, 0, DPU_BUFFER_SIZE, PREDETERMINED);


    mram_write(&work_buffer, &buffer, DPU_BUFFER_SIZE * sizeof(uint32_t));
    return 0;
}