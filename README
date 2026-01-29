# Multi-Level Page Table Implementation

This library implements a multi-level page table system for virtual memory address translation. It provides functionality for address translation and page allocation using a configurable number of page table levels.

## Configuration Guide

The system behavior is controlled through `config.h`, which contains two key parameters, along with a couple fo constants used by the program:

### LEVELS
- Defines the number of page table levels used for address translation
- Recommended values: 2-4
- Supported Range: 1-6
- Higher values create more levels of indirection but allow for larger address spaces
- Example settings:
  - 2: Suitable for small address spaces (32-bit)
  - 4: Suitable for large address spaces (64-bit)

### POBITS
- Defines the number of bits used for the page offset
- Determines the page size: Page Size = 2^POBITS
- Recommended values: 12-16
- Supported Range: 4-18
- Trade-offs:
  - Larger values (>12) mean larger pages, reducing page table size but increasing internal fragmentation
  - Smaller values (<12) mean smaller pages, decreasing internal fragmentation but increasing page table size
- Example settings:
  - 12: 4KB pages (standard size)
  - 16: 64KB pages

## Usage Example

```c
#include "mlpt.h"

int main() {
    // Allocate a page for virtual address 0x1000
    page_allocate(0x1000);
    
    // Translate virtual address to physical address
    size_t physical_addr = translate(0x1000);
    
    // Check if translation was successful
    if (physical_addr == ~0UL) {
        printf("Translation failed\n");
    } else {
        printf("Physical address: 0x%lx\n", physical_addr);
    }

    // Free the allocated page
    page_free(0x1000);
    
    return 0;
}
```

## Implementation Details

### Time Complexity
- `translate()`: $\Theta(LEVELS)$ - performs one memory access per level
- `page_allocate()`: $\Theta(LEVELS)$ - allocates one page table per level if needed
- `page_free()`: $\Theta(LEVELS + ENTRIES)$ - traverses the page table hierarchy and checks for empty tables

### Space Complexity
- Each page table and each page requires $2^{\texttt{POBITS}}$ entries
- Total memory usage depends on address space coverage and sparsity
- In the worst case, the page table can potentially take up $((\frac{2^{\texttt{POBITS}}}{8}) ^ {\texttt{LEVELS}}) \ast 8$ bytes

## Memory Management Features

### Page Allocation
- Can dynamically allocate page tables as needed
- Maintains valid/invalid bits for entry tracking
- Aligns all allocations to page boundaries

### Page Deallocation
- Can free physical pages when no longer needed
- Recursively cleans up empty page tables
- Maintains page table hierarchy integrity
- Handles edge cases:
  - Unallocated pages
  - Already freed pages
  - Empty page tables

## Limitations

1. **Address Space Constraints**
   - Maximum virtual address size is limited by LEVELS and POBITS
   - Formula: Maximum addressable bits = (LEVELS $\ast$ (POBITS - 3) + POBITS)

2. **Memory Management**
   - No reference counting for shared pages
   - No protection against double-free errors
   - Memory fragmentation possible

3. **Concurrency**
   - Not thread-safe
   - Requires external synchronization for multi-threaded access

## Testing

The implementation includes a basic test suite (`test.c`) that verifies:

1. **Basic Operations**
   - Page allocation
   - Address translation
   - Page deallocation
   - Boundary conditions

2. **Memory Management**
   - Proper page freeing
   - Page table cleanup
   - Memory leak prevention
   - Multiple allocation/deallocation cycles

3. **Edge Cases**
   - Unallocated page access
   - Maximum address handling
   - Empty page table cleanup
   - Invalid address handling

To run the tests:
```bash
cd src
make test
```

## Future Enhancements

1. **Memory Management**
   - Implement reference counting for shared pages
   - Add page replacement algorithms
   - Implement memory compaction

2. **Performance Optimizations**
   - Add TLB simulation
   - Implement huge page support
   - Add page compression

3. **Testing Improvements**
   - Add stress testing
   - Implement concurrent access testing
   - Add memory leak detection
   - Improve test coverage

## Known Issues

1. **Memory Management**
   - No protection against use-after-free
   - Potential memory leaks in error conditions
   - No handling of fragmentation

2. **Performance**
   - Sequential page table traversal
   - No implementation of TLB.

3. **Testing**
   - Limited edge case coverage
   - No automated performance testing
   - No concurrent access testing
