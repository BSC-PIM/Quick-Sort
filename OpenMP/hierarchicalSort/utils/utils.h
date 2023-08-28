#ifndef MERGEIDEA_UTILS_H
#define MERGEIDEA_UTILS_H


#define MAX(...) \
    ({ \
        typeof(__VA_ARGS__) values[] = {__VA_ARGS__}; \
        int num_values = sizeof(values) / sizeof(values[0]); \
        typeof(__VA_ARGS__) max; \
        do { \
            max = values[0]; \
            for (int i = 1; i < num_values; ++i) { \
                if (values[i] > max) { \
                    max = values[i]; \
                } \
            } \
        } while (0); \
        max; \
    })

#define MIN(...) \
    ({ \
        typeof(__VA_ARGS__) values[] = {__VA_ARGS__}; \
        int num_values = sizeof(values) / sizeof(values[0]); \
        typeof(__VA_ARGS__) min; \
        do { \
            min = values[0]; \
            for (int i = 1; i < num_values; ++i) { \
                if (values[i] < min) { \
                    min = values[i]; \
                } \
            } \
        } while (0); \
        min; \
    })

#define POPULATE_ARR(arr, size, limit) \
    do{                         \
        for (size_t pop_arr_it = 0; pop_arr_it < size; pop_arr_it++) { \
        arr[pop_arr_it] = rand() % limit; \
        }                           \
    } while(0)


#define COPY_ARR(src, dest, size) \
    do{                           \
        for (size_t i = 0; i < size; i++) { \
            dest[i] = src[i]; \
        } \
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
