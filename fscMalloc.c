
#include "fscMalloc.h"
#include <assert.h>
//#include <sys/mman.h>
#include <stdlib.h> /* for rand() */
#include <time.h> /* for the init of rand */
#include <stddef.h> /* for size_t */

/* student written memory test */
int memoryTestThree() {
    /* If you want to add a test, feel free to add it here.
     * Returning 0 = success
     * Returning non-zero = failure
     */
}
void* fscMemorySetup(memoryStructure* m, fscAllocationMethod am, size_t sizeInBytes) {
    void *base = malloc(sizeInBytes);

    if(base == 0)
    {
        return 0;
    }

    m->base = base;
    m->head = (fsc_free_node_t*) base;
    m->head->size = sizeInBytes - sizeof (fsc_free_node_t);
    m->head->next = 0;

    return base;
}

/* very clearly for FIRST_FIT_RETURN_FIRST */
void* fscMalloc(memoryStructure* m, size_t requestedSizeInBytes) {
    fsc_free_node_t* current = m->head;
    fsc_free_node_t* previous = 0;
    size_t spaceNeeded = requestedSizeInBytes + sizeof(fsc_alloc_header_t);

    if(current->size + sizeof (fsc_free_node_t) >=
        requestedSizeInBytes + sizeof (fsc_free_node_t))
    {
        m->head->size = current->size;
    }

    return 0;
}

void fscFree(memoryStructure* m, void * returnedMemory) {
    return *returnedMemory;
} // fscfree()

void fscMemoryCleanup(memoryStructure* m)
{
    free(m->base);
    m->base = 0;
    m->head = 0;
}

/* keep for students */
void printFreeList(FILE * out, fsc_free_node_t* head) {
    /* given a node, prints the list. */
    fsc_free_node_t* current = head;
    fprintf(out, "About to dump the free list:\n");
    while (0 != current) {
        fprintf(out,
                "Node address: %'u\t Node size (stored): %'u\t Node size (actual) %'u\t Node next:%'u\n",
                current,
                current->size,
                current->size + sizeof (fsc_free_node_t),
                current->next);
        current = current->next;
    }
}