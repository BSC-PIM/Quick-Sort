#include <assert.h>
#include <dpu.h>

#ifndef DPU_BINARY
#define DPU_BINARY "dpu"
#endif

/* Size of the buffer that we want to pivot: 1KByte. */
#define BUFFER_SIZE (1 << 6)

/* Number of DPUs to use. */
#define NB_DPUS 4

/* Range of the random numbers. */
#define MAX_RANDOM 1000

/* Fill a buffer with random numbers between 1 and MAX_RANDOM. */
void fill_random_buffer(uint32_t *buffer, size_t size) {
    srand(42);
    for (size_t i = 0; i < size; i++) {
        buffer[i] = rand() % MAX_RANDOM + 1;
    }
}

void populate_mram(struct dpu_set_t set, int *buffer) {
    struct dpu_set_t dpu;
    uint32_t each_dpu;

    /* Distribute the buffer across the DPUs. */
    DPU_FOREACH(set, dpu, each_dpu)
    {
        DPU_ASSERT(
                dpu_prepare_xfer(dpu, &buffer[each_dpu * BUFFER_SIZE / NB_DPUS]));
    }
    DPU_ASSERT(dpu_push_xfer(set, DPU_XFER_TO_DPU, "buffer", 0,
                             (BUFFER_SIZE) * sizeof(uint32_t) / NB_DPUS,
                             DPU_XFER_DEFAULT));
}


void print_buffer(uint32_t *buffer, size_t size) {
    for (size_t i = 0; i < size; i++) {
        printf("%d ", buffer[i]);
    }
    printf("\n");
}


int main() {
    struct dpu_set_t set;

    /* Generate random data */
    uint32_t buffer[BUFFER_SIZE];
    fill_random_buffer(buffer, BUFFER_SIZE);


    /* Initialize and run */
    DPU_ASSERT(dpu_alloc(NB_DPUS, NULL, &set));
    DPU_ASSERT(dpu_load(set, DPU_BINARY, NULL));
    populate_mram(set, buffer);

    uint32_t pivots[NB_DPUS];
    for (int i = 0; i < NB_DPUS; i++) {
        pivots[i] = buffer[i * BUFFER_SIZE / NB_DPUS];
    }

    struct dpu_set_t dpu;
    uint32_t each_dpu;

    DPU_FOREACH(set, dpu, each_dpu)
    {
        DPU_ASSERT(dpu_prepare_xfer(dpu, &pivots[each_dpu]));
    }

    DPU_ASSERT(dpu_push_xfer(set, DPU_XFER_TO_DPU, "pre_determined_pivot", 0,
                             sizeof(uint32_t),
                             DPU_XFER_DEFAULT));


    print_buffer(buffer, BUFFER_SIZE / NB_DPUS);
    print_buffer(buffer + BUFFER_SIZE / NB_DPUS, BUFFER_SIZE / NB_DPUS);
    print_buffer(buffer + 2 * BUFFER_SIZE / NB_DPUS, BUFFER_SIZE / NB_DPUS);
    print_buffer(buffer + 3 * BUFFER_SIZE / NB_DPUS, BUFFER_SIZE / NB_DPUS);

    DPU_ASSERT(dpu_launch(set, DPU_SYNCHRONOUS));

    /* add logger */
    DPU_FOREACH(set, dpu)
    {
        DPU_ASSERT(dpu_log_read(dpu, stdout));
    }

    /* retrieve results */
    each_dpu;
    DPU_FOREACH(set, dpu, each_dpu)
    {
        DPU_ASSERT(dpu_prepare_xfer(dpu, &buffer[each_dpu * BUFFER_SIZE / NB_DPUS]));
    }

    DPU_ASSERT(dpu_push_xfer(set, DPU_XFER_FROM_DPU, "buffer", 0,
                             (BUFFER_SIZE) * sizeof(uint32_t) / NB_DPUS,
                             DPU_XFER_DEFAULT));

    printf("Sorted array:\n");
    print_buffer(buffer, BUFFER_SIZE / NB_DPUS);
    print_buffer(buffer + BUFFER_SIZE / NB_DPUS, BUFFER_SIZE / NB_DPUS);
    print_buffer(buffer + 2 * BUFFER_SIZE / NB_DPUS, BUFFER_SIZE / NB_DPUS);
    print_buffer(buffer + 3 * BUFFER_SIZE / NB_DPUS, BUFFER_SIZE / NB_DPUS);
    DPU_ASSERT(dpu_free(set));


    // merge the sorted arrays into one
    int *sub_buffer_pointer[NB_DPUS];
    for (int i = 0; i < NB_DPUS; i++) {
        sub_buffer_pointer[i] = buffer + i * BUFFER_SIZE / NB_DPUS;
    }

    // merge the sorted arrays into one
    uint32_t *sorted_buffer = (uint32_t *) malloc(BUFFER_SIZE * sizeof(uint32_t));

    for (int i = 0; i < BUFFER_SIZE; i++) {
        int min = MAX_RANDOM + 1;
        int min_index = -1;
        for (int j = 0; j < NB_DPUS; j++) {
            if (sub_buffer_pointer[j] != NULL && *sub_buffer_pointer[j] < min) {
                min = *sub_buffer_pointer[j];
                min_index = j;
            }
        }
        sorted_buffer[i] = min;
        sub_buffer_pointer[min_index]++;
    }

    printf("Merged sorted array:\n");
    print_buffer(sorted_buffer, BUFFER_SIZE);


    return 0;
}