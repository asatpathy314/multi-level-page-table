#define _XOPEN_SOURCE 700
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <errno.h>
#include "mlpt.h"
#include "config.h"

// Constants
#define PAGE_SIZE (1 << (POBITS))
#define VPN_BITS (POBITS - 3)
#define ENTRIES (1 << (VPN_BITS))
#define SEGFAULT (~0UL)
#define INVALID_BIT (0)
#define VALID_BIT (1)
#define VPN_MASK ((1 << VPN_BITS) - 1)
#define ADDR_MASK (~1UL)

// Page table base register
size_t ptbr;

// Helper functions for bit manipulation
int is_entry_valid(size_t entry) {
    return (entry & 1) == VALID_BIT;
}

size_t clear_valid_bit(size_t entry) {
    return entry & ADDR_MASK;
}

size_t set_valid_bit(size_t addr) {
    return addr | VALID_BIT;
}

// Memory allocation helpers
size_t* allocate_new_page(void) {
    size_t* new_page;
    if (posix_memalign((void**) &new_page, PAGE_SIZE, PAGE_SIZE) != 0) {
        perror("posix_memalign failed");
        exit(EXIT_FAILURE);
    }
    memset(new_page, 0, PAGE_SIZE);
    return new_page;
}

void initialize_page_table(void) {
    if (ptbr == 0) {
        ptbr = (size_t) allocate_new_page();
    }
}

// VPN calculation
void populate_vpn_offsets(size_t va, int *vpn_offset) {
    for (int level = 0; level < LEVELS; ++level) {
        vpn_offset[level] = (va >> (POBITS + ((LEVELS - 1 - level) * VPN_BITS))) & VPN_MASK;
    }
}

int calculate_page_offset(size_t va) {
    int offset_mask = (1 << POBITS) - 1;
    return va & offset_mask;
}

// Translation helper
size_t page_walk(int *vpn_offset, int offset) {
    size_t *base_addr = (size_t *) ptbr;
    
    for (int level = 0; level < LEVELS; ++level) {
        if (base_addr == NULL) {
            return SEGFAULT;
        }

        size_t entry = base_addr[vpn_offset[level]];
        if (!is_entry_valid(entry)) {
            return SEGFAULT;
        }

        entry = clear_valid_bit(entry);
        if (level + 1 == LEVELS) {
            return entry + offset;
        }
        base_addr = (size_t*)entry;
    }
    return SEGFAULT;
}

// Page table traversal for allocation
void allocate_page_tables(int *vpn_offset) {
    size_t *base_addr = (size_t*)ptbr;
    
    for (int level = 0; level < LEVELS; ++level) {
        size_t entry = base_addr[vpn_offset[level]];
        
        if (!is_entry_valid(entry)) {
            size_t *new_page = allocate_new_page();
            base_addr[vpn_offset[level]] = set_valid_bit((size_t)new_page);
        }
        
        base_addr = (size_t*)clear_valid_bit(base_addr[vpn_offset[level]]);
    }
}

// Page free helpers
void populate_base_address_array(int *vpn_offset, size_t **base_addr_array) {
    base_addr_array[0] = (size_t*)ptbr;
    
    for (int level = 0; level < LEVELS - 1; ++level) {
        size_t entry = base_addr_array[level][vpn_offset[level]];
        if (!is_entry_valid(entry)) {
            return;
        }
        base_addr_array[level + 1] = (size_t*)clear_valid_bit(entry);
    }
}

int is_page_table_empty(size_t *page_table) {
    for (int entry = 0; entry < ENTRIES; ++entry) {
        if (is_entry_valid(page_table[entry])) {
            return 0;
        }
    }
    return 1;
}

void cleanup_empty_tables(int *vpn_offset, size_t **base_addr_array) {
    for (int level = LEVELS - 1; level > 0; --level) {
        if (!is_page_table_empty(base_addr_array[level])) {
            break;
        }
        free(base_addr_array[level]);
        base_addr_array[level - 1][vpn_offset[level - 1]] = INVALID_BIT;
    }
}

// Main interface functions
size_t translate(size_t va) {
    int vpn_offset[LEVELS];
    populate_vpn_offsets(va, vpn_offset);
    int offset = calculate_page_offset(va);
    return page_walk(vpn_offset, offset);
}

void page_allocate(size_t va) {
    initialize_page_table();
    
    int vpn_offset[LEVELS];
    populate_vpn_offsets(va, vpn_offset);
    allocate_page_tables(vpn_offset);
}

void page_free(size_t va) {
    if (ptbr == 0) {
        return;
    }

    int vpn_offset[LEVELS];
    populate_vpn_offsets(va, vpn_offset);
    
    size_t *base_addr_array[LEVELS];
    populate_base_address_array(vpn_offset, base_addr_array);
    
    // Free the leaf page
    size_t leaf_entry = base_addr_array[LEVELS - 1][vpn_offset[LEVELS - 1]];
    if (is_entry_valid(leaf_entry)) {
        free((void*)clear_valid_bit(leaf_entry));
        base_addr_array[LEVELS - 1][vpn_offset[LEVELS - 1]] = INVALID_BIT;
    }
    
    cleanup_empty_tables(vpn_offset, base_addr_array);
}