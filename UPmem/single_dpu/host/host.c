#include <assert.h>
#include <dpu.h>

#ifndef DPU_BINARY
#define DPU_BINARY "build/dpu"
#endif

/* Size of the buffer that we want to pivot: 1KByte. */
#define BUFFER_SIZE (1 << 4)

/* Number of DPUs to use. */
#define NB_DPUS 1

/* Range of the random numbers. */
#define MAX_RANDOM 1000

/* Fill a buffer with random numbers between 1 and MAX_RANDOM. */
void fill_random_buffer(uint32_t *buffer, size_t size) {
    srand(42);
    for (size_t i = 0; i < size; i++) {
        buffer[i] = rand() % MAX_RANDOM + 1;
    }
}

void populate_mram(struct dpu_set_t set, uint32_t *buffer) {
    struct dpu_set_t dpu;
    uint32_t each_dpu;

    /* Distribute the buffer across the DPUs. */
    DPU_FOREACH(set, dpu, each_dpu)
    {
        DPU_ASSERT(
                dpu_prepare_xfer(dpu, &buffer[each_dpu * BUFFER_SIZE]));
    }
    DPU_ASSERT(dpu_push_xfer(set, DPU_XFER_TO_DPU, "buffer", 0,
                             (BUFFER_SIZE) * sizeof(uint32_t),
                             DPU_XFER_DEFAULT));
}


void print_buffer(uint32_t *buffer, size_t size) {
    for (size_t i = 0; i < size; i++) {
        printf("%d ", buffer[i]);
    }
    printf("\n");
}


int main() {
    srand(42);
    struct dpu_set_t set;

    /* Generate random data */
    uint32_t buffer[BUFFER_SIZE];
    fill_random_buffer(buffer, BUFFER_SIZE);

    int pivot = rand() % MAX_RANDOM + 1;
    printf("Pivot: %d\n", pivot);

    /* Initialize and run */
    DPU_ASSERT(dpu_alloc(NB_DPUS, NULL, &set));
    DPU_ASSERT(dpu_load(set, DPU_BINARY, NULL));
    populate_mram(set, buffer);
    dpu_broadcast_to(set, "pre_determined_pivot", 0, &pivot, sizeof(pivot), DPU_XFER_DEFAULT);

    print_buffer(buffer, BUFFER_SIZE);
    DPU_ASSERT(dpu_launch(set, DPU_SYNCHRONOUS));

    /* Retrieve the results */
    struct dpu_set_t dpu;

    DPU_FOREACH(set, dpu)
    {
        DPU_ASSERT(dpu_copy_from(dpu, "buffer", 0, buffer, BUFFER_SIZE * sizeof(uint32_t)));
    }

    print_buffer(buffer, BUFFER_SIZE);
    DPU_ASSERT(dpu_free(set));
    return 0;
}