#include <assert.h>
#include <time.h>
#include <stdio.h>
#include "mlpt.h"
#include "config.h"

#define TEST_PASSED "\033[0;32m[PASSED]\033[0m"
#define TEST_FAILED "\033[0;31m[FAILED]\033[0m"
#define TEST_INFO  "\033[0;34m[INFO]\033[0m"

// Constants
#define PAGE_SIZE (1 << (POBITS))
#define VPN_BITS (POBITS - 3)
#define ENTRIES (1 << (VPN_BITS))
#define SEGFAULT (~0UL)

void print_page_table_info(size_t va, size_t pa) {
    printf("Virtual Address: 0x%lx -> Physical Address: 0x%lx\n", va, pa);
}

int main(void) {
    printf("\n%s Starting Memory Management Tests\n\n", TEST_INFO);
    
    // Test 1: Basic Allocation
    printf("Test 1: Basic Page Allocation\n");
    {
        size_t test_addr = 0x1000;
        page_allocate(test_addr);
        size_t phys_addr = translate(test_addr);
        
        if (phys_addr != SEGFAULT) {
            printf("%s Basic allocation successful\n", TEST_PASSED);
            print_page_table_info(test_addr, phys_addr);
        } else {
            printf("%s Basic allocation failed\n", TEST_FAILED);
            return 1;
        }
    }
    printf("\n");

    // Test 2: Multiple Allocations
    printf("Test 2: Multiple Page Allocations\n");
    {
        size_t addresses[] = {0x2000, 0x3000, 0x4000};
        int success = 1;
        
        for (int i = 0; i < 3; i++) {
            page_allocate(addresses[i]);
            size_t phys_addr = translate(addresses[i]);
            
            if (phys_addr != SEGFAULT) {
                printf("%s Allocated page at 0x%lx\n", TEST_PASSED, addresses[i]);
                print_page_table_info(addresses[i], phys_addr);
            } else {
                printf("%s Failed to allocate page at 0x%lx\n", TEST_FAILED, addresses[i]);
                success = 0;
                break;
            }
        }
        
        if (!success) return 1;
    }
    printf("\n");

    // Test 3: Deallocation
    printf("Test 3: Page Deallocation\n");
    {
        size_t test_addr = 0x2000;
        size_t before_free = translate(test_addr);
        
        printf("%s Before free: ", TEST_INFO);
        print_page_table_info(test_addr, before_free);
        
        page_free(test_addr);
        size_t after_free = translate(test_addr);
        
        if (after_free == SEGFAULT) {
            printf("%s Deallocation successful\n", TEST_PASSED);
        } else {
            printf("%s Deallocation failed\n", TEST_FAILED);
            return 1;
        }
    }
    printf("\n");

    // Test 4: Boundary Cases
    printf("Test 4: Boundary Cases\n");
    {
        // Test minimum address
        page_allocate(0x0);
        if (translate(0x0) != SEGFAULT) {
            printf("%s Minimum address allocation successful\n", TEST_PASSED);
        } else {
            printf("%s Minimum address allocation failed\n", TEST_FAILED);
            return 1;
        }

        // Test maximum valid address
        size_t max_addr = (1UL << (POBITS + (LEVELS * VPN_BITS))) - PAGE_SIZE;
        page_allocate(max_addr);
        if (translate(max_addr) != SEGFAULT) {
            printf("%s Maximum address allocation successful\n", TEST_PASSED);
        } else {
            printf("%s Maximum address allocation failed\n", TEST_FAILED);
            return 1;
        }
    }
    printf("\n");

    // Test 5: Performance Test
    printf("Test 5: Performance Test\n");
    {
        clock_t start = clock();
        const int num_pages = 1000;
        
        // Allocate many pages
        for (int i = 0; i < num_pages; i++) {
            page_allocate(i * PAGE_SIZE);
        }
        
        // Free all pages
        for (int i = 0; i < num_pages; i++) {
            page_free(i * PAGE_SIZE);
        }
        
        clock_t end = clock();
        double cpu_time = ((double) (end - start)) / CLOCKS_PER_SEC;
        printf("%s Allocated and freed %d pages in %f seconds\n", 
               TEST_INFO, num_pages, cpu_time);
    }
    printf("\n");

    printf("%s All tests completed successfully!\n\n", TEST_PASSED);
    return 0;
}