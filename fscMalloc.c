
#include "fscMalloc.h"
#include <assert.h>
//#include <sys/mman.h>
#include <stdlib.h> /* for rand() */
#include <time.h> /* for the init of rand */
#include <stddef.h> /* for size_t */
#define FSC_MAGIC 1234567

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

    //Walk through the list to see if there are a free space big enough for requested size
    while(current != 0 && current->size + sizeof(fsc_free_node_t) < spaceNeeded)
    {
        previous = current;
        current = current->next;
    }

    //If there is not a space big enough
    if (current == 0)
    {
        return 0;
    }

    //This section saves what is needed from the node before overwriting it
    size_t blockTotal = current->size + sizeof(fsc_free_node_t);
    fsc_free_node_t* nextNode = current->next;
    size_t leftOver = blockTotal - spaceNeeded;

    fsc_alloc_header_t* header = (fsc_alloc_header_t*) current;
    fsc_free_node_t* replacement;

    //Split the node and have the new free node after the block
    if(leftOver >= sizeof(fsc_free_node_t))
    {
        replacement = (fsc_free_node_t*) ((char*) current + spaceNeeded);
        replacement->size = leftOver - sizeof(fsc_free_node_t);
        replacement->next = nextNode;
        header->size = requestedSizeInBytes;
    }
    //leftOver is too small to hold the node
    else
    {
        replacement = nextNode;
        header->size = blockTotal - sizeof(fsc_alloc_header_t);
    }

    header->magicNumber = FSC_MAGIC;

    if(previous == 0)
    {
        m->head = replacement;
    }
    else
    {
        previous->next = replacement;
    }

    return (char*) header + sizeof(fsc_alloc_header_t);
}

void fscFree(memoryStructure* m, void * returnedMemory) {
    if(returnedMemory == 0)
    {
        return;
    }

    //Step back from pointer and find the header
    fsc_alloc_header_t* header =
        (fsc_alloc_header_t*) ((char*)returnedMemory - sizeof(fsc_alloc_header_t));

    assert(header->magicNumber == FSC_MAGIC);

    //This section turns the header found into a free node
    size_t blockTotal = header->size + sizeof(fsc_alloc_header_t);
    fsc_free_node_t* node = (fsc_free_node_t*) header;
    node->size = blockTotal - sizeof(fsc_free_node_t);

    //This section makes sure the list stays sorted by address
    fsc_free_node_t* previous = 0;
    fsc_free_node_t* current = m->head;
    while(current != 0 && current < node)
    {
        previous = current;
        current = current->next;
    }

    node->next = current;
    if(previous == 0)
    {
        m->head = node;
    }
    else
    {
        previous->next = node;
    }

    //Coalesce with the block after the new freed block
    if(current != 0
        && (char*) node + sizeof(fsc_free_node_t) + node->size == (char*) current)
    {
        node->size += sizeof(fsc_free_node_t) + current->size;
        node->next = current->next;
    }

    //Coalesce with the block before the new freed block
    if(previous != 0
        && (char*) previous + sizeof(fsc_free_node_t) + previous->size == (char*) node)
    {
        previous->size += sizeof(fsc_free_node_t) + node->size;
        previous->next = node->next;
    }
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