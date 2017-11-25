/*! \file frame.h
 *
 */

#ifndef VM_FRAME_H
#define VM_FRAME_H

#include <debug.h>
#include <stdint.h>
#include "threads/vaddr.h"
#include "threads/loader.h"

struct frame_table_entry {
    void* page;     /* pointer to page currently occupying entry */
};


struct frame_table_entry** frame_table;

void init_frame_table(void);
uint32_t get_frame(bool user);
uint32_t evict(void);

/*! Returns frame number at which kernel virtual address VADDR
    is mapped. */
static inline uint32_t vtof(const void *vaddr) {
    ASSERT(is_kernel_vaddr(vaddr));

    return ((uintptr_t) vaddr - (uintptr_t) PHYS_BASE) >> PGBITS;
}

/*! Returns kernel virtual address at which frame number refers to. */
static inline void * ftov(uint32_t frame_number) {
    ASSERT(frame_number < init_ram_pages);

    return (void *) ((frame_number << PGBITS) + PHYS_BASE);
}



#endif /* vm/frame.h */
