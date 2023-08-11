#include "quicksort.h"
#include <stdio.h>


#define SIZE 16


void populate(uint64_t *arr, int low, int high) {


    for (int i = low; i <= high; i++) {
        arr[i] = i;
    }

    // shuffle
    for (int i = low; i <= high; i++) {
        int j = rand() % (high - low + 1);
        uint64_t temp = arr[i];
        arr[i] = arr[j];
        arr[j] = temp;
    }
}


void sample(uint64_t *arr, int low, int high) {
    for (int i = low; i <= high; i++) {
        printf("%d ", arr[i]);
    }
    printf("\n");
}


int main() {

    // 893 449 699 260 868 353 810 34 728 580 170 505 142 177 434 308
    uint64_t arr[SIZE] = {893, 449, 699, 260, 868, 353, 810, 34, 728, 580, 170, 505, 142, 177, 434, 308};

    sample(arr, 0, SIZE - 1);

    quicksort(arr, 0, SIZE, PREDETERMINED);
    sample(arr, 0, SIZE -1);

    return 0;
}
