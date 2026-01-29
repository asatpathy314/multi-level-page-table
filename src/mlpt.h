#ifndef MLPT_H
#define MLPT_H
/**
 * Given a virtual address, return the physical address.
 * Return a value consisting of all 1 bits
 * if this virtual address does not have a physical address.
 */
size_t translate(size_t va);

/**
 * Use posix_memalign to create page tables and other pages sufficient
 * to have a mapping between the given virtual address and some physical address.
 * If there already is such a page, does nothing.
 */
void page_allocate(size_t va);

/**
 * Use free to free the page at va. free any intermediate page tables 
 * if they exists other than the base page table pointed to by ptbr. does 
 * nothing if va doesn't have a physical address
 */
void page_free(size_t va);
#endif