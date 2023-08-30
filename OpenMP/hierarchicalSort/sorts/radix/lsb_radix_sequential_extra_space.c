#include "lsb_radix_sequential_extra_space.h"


void rearrange_values_prefix(T *cnt, const T *input,
                             T *output, size_t key_num, uint8_t pass) {
    // Loop to iterate on the data
    for (size_t it = 0; it < key_num; it++) {
        // Get the byte to compute
        T shifted_value = input[it] >> (8 * pass);
        // get the prefix
        T prefix_value = cnt[shifted_value & (0xFF)];
        // Allocate the value at its new position
        output[prefix_value] = input[it];
        // Update the prefix by one element
        cnt[shifted_value & 0xFF]++;

#ifdef PREFETCH1
        __builtin_prefetch(((char*)(output+prefix_value)) +
        (64 * AHEAD_LINES_BUCKET - sizeof(T)), 1, 1);
       // std::cout << "prefetch 1" << std::endl;
#endif

    }
}


void histogram_calc(T *histogram, T *input_keys,
                    size_t key_num, uint8_t pass) {
    for (size_t it = 0; it < key_num; it++) {
        T shifted_value = input_keys[it] >> (BYTE_SIZE * pass);

        shifted_value = shifted_value & ((1 << 8) - 1);
        histogram[shifted_value]++;
    }
}


void prefix_calc(T *hist) {
    // temporal Values
    T prev_value;
    T prefix_value;
    // Execute for the entry 0
    prev_value = hist[0];
    hist[0] = 0;
    // loop to calculate the prefix
    for (int it = 1; it <= (1 << BYTE_SIZE); it++) {
        // First calculate the new value for the entry
        prefix_value = prev_value + hist[it - 1];
        // Backup previous value
        prev_value = hist[it];
        // Update value
        hist[it] = prefix_value;
    }
}

void msb_radix_sort_sequential_extra_space(T *input, T *output, size_t size) {

    size_t buckets_n = (1 << BYTE_SIZE);
    size_t cnt[buckets_n + 1];

    int passes = sizeof(T) * 8;
    passes = (passes / 8) + ((passes % 8) > 0 ? 1 : 0);


    T *src, *dst;
    int pass;
    for (pass = 0; pass < passes; pass++) {
        memset(cnt, 0, (buckets_n + 1) * sizeof(T));
        histogram_calc(cnt, pass % 2 == 1 ? output : input, size, pass);
        prefix_calc(cnt);
        rearrange_values_prefix(cnt, pass % 2 == 1 ? output : input, pass % 2 == 1 ? input : output, size, pass);
    }
}
