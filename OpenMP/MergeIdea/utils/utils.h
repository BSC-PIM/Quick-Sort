//
// Created by halfblood on 8/9/23.
//

#ifndef MERGEIDEA_UTILS_H
#define MERGEIDEA_UTILS_H


#define MAX(a, b) ((a) > (b) ? (a) : (b))

#define MIN(a, b) ((a) > (b) ? (b) : (a))


#define POPULATE_ARR(arr, size) \
    do{                         \
        for (size_t i = 0; i < size; i++) { \
        arr[i] = rand() % 100; \
        }                           \
    } while(0)

#define PRINT_ARR(arr, size) \
    do{                      \
        for (size_t i = 0; i < size; i++) { \
            printf("%lu ", arr[i]); \
        } \
        printf("\n"); \
    } while(0)


#define SWAP(a, b) \
    do{            \
        typeof(a) tmp = a; \
        a = b; \
        b = tmp; \
    } while(0)

#endif //MERGEIDEA_UTILS_H
