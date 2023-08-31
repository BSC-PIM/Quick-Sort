#ifndef MERGEIDEA_UTILS_H
#define MERGEIDEA_UTILS_H


#define MAX(...) \
    ({ \
        typeof(__VA_ARGS__) values[] = {__VA_ARGS__}; \
        int num_values = sizeof(values) / sizeof(values[0]); \
        typeof(__VA_ARGS__) max; \
        do { \
            max = values[0]; \
            for (int max_itr = 1; max_itr < num_values; ++max_itr) { \
                if (values[max_itr] > max) { \
                    max = values[max_itr]; \
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
            for (int min_itr = 1; min_itr < num_values; ++min_itr) { \
                if (values[min_itr] < min) { \
                    min = values[min_itr]; \
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

#define CEILING(x, y) (((x) + (y) - 1) / (y))

#endif //MERGEIDEA_UTILS_H
