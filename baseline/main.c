#include "quicksort.h"
#include <stdio.h>


#define SIZE 10


void populate(uint32_t *arr, int low, int high) {


    for (int i = low; i <= high; i++) {
        arr[i] = i;
    }

    // shuffle
    for (int i = low; i <= high; i++) {
        int j = rand() % (high - low + 1);
        uint32_t temp = arr[i];
        arr[i] = arr[j];
        arr[j] = temp;
    }
}


void sample(uint32_t *arr, int low, int high) {
    for (int i = low; i <= high; i++) {
        printf("%d ", arr[i]);
    }
    printf("\n");
}


int main() {

    //167 741 882 242 13 759 22 941 536 744 875 144 718 505 697 763
    uint32_t arr[SIZE] = {167, 741, 882, 242, 13, 759, 22, 941, 536, 744};

    sample(arr, 0, SIZE - 1);

    quicksort(arr, 0, SIZE, PREDETERMINED);
    sample(arr, 0, SIZE -1);

    return 0;
}
