#include "lsb_radix_sequential_extra_space.h"


void rearrange_values_prefix(uint64_t *cnt, const uint64_t *input,
                             uint64_t *output, uint32_t key_num, uint8_t pass) {
    // Loop to iterate on the data
    for (uint32_t it = 0; it < key_num; it++) {
        // Get the byte to compute
        uint64_t shifted_value = input[it] >> (8 * pass);

#ifdef PREFETCH3
        __builtin_prefetch(((char*)(input + it)) + 64 * AHEAD_LINES_SEQ, 0, 1);
#endif

        // get the prefix
        uint64_t prefix_value = cnt[shifted_value & (0xFF)];
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


void histogram_calc(uint64_t *histogram, uint64_t *input_keys,
                    uint32_t key_num, uint8_t pass) {
    // Calculate the histgram
#ifdef _DBG3_
    std::cout << " data: " << pass << std::endl;
#endif
    for (size_t it = 0; it < key_num; it++) {
        uint64_t shifted_value = input_keys[it] >> (BYTE_SIZE * pass);
#ifdef PREFETCH3
        if (it & 1) {
           __builtin_prefetch(((char*)(input_keys + it)) + 64 * AHEAD_LINES_SEQ, 0, 1);
        }
#endif
        shifted_value = shifted_value & ((1 << 8) - 1);
#ifdef _DBG3_
        std::cout << " " << shifted_value;
#endif
        histogram[shifted_value]++;
        //histogram[shifted_value & (HIST_MASK)]++;
    }
}


void prefix_calc(uint64_t *hist) {
    // temporal Values
    uint64_t prev_value;
    uint64_t prefix_value;
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

void msb_radix_sort_sequential_extra_space(uint64_t *input, uint64_t *output, size_t size) {

    size_t buckets_n = (1 << BYTE_SIZE);
    size_t cnt[buckets_n + 1];

    int passes = sizeof(uint64_t) * 8;
    passes = (passes / 8) + ((passes % 8) > 0 ? 1 : 0);


    // We execute the entire algorithm as a way to have a clear idea on
    // how this one works and how the entire algorithm interacts
    uint64_t *src, *dst;

    int pass;
    for (pass = 0; pass < passes; pass++) {
        memset(cnt, 0, (buckets_n + 1) * sizeof(uint64_t));
        histogram_calc(cnt, pass % 2 == 1 ? output : input, size, pass);
        prefix_calc(cnt);
        rearrange_values_prefix(cnt, pass % 2 == 1 ? output : input, pass % 2 == 1 ? input : output, size, pass);
    }
}
