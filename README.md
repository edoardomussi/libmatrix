# Matrix Library API Reference

## 1. Memory Lifecycle
1. **Initialization:** Use `mnew(rows, cols, &m_ptr)` to create a matrix. This handles both the metadata allocation and the aligned data buffer.
2. **Deallocation:** You must use `mfree(&m_ptr)`. Passing a pointer to an improperly initialized structure or a non-aligned buffer will lead to undefined behavior.
3. **Ownership:** The caller owns the `matrix_s` structure. The library owns the `data` buffer *during* the execution of operations. Do not manually free the `data` member.

## 2. Memory Layout & Constraints
The high-performance loops rely on strict physical memory expectations:

+ **Stride Alignment:**
    All matrices are padded to a 64-byte `stride`. This ensures that every row starts at a memory address that is a multiple of `BLK_SIZE`, which is critical for cache-line alignment.
+ **Aliasing Restrictions:**
    For all arithmetic operations (e.g., `mmul`, `madd`), the output buffer must **not** point to the same address as any input buffer.
    This prevents read-after-write hazards and allows the compiler to fully utilize SIMD vectorization without needing to account for overlapping memory regions.



## 3. Cache & Performance Strategy
The core engine is designed for CPU cache efficiency.

+ **Tile-Based Processing:**
    Multiplication is performed using $32 \times 32$ tiles. This keeps data active in the L1/L2 caches for longer, minimizing expensive main memory round-trips.
+ **SIMD vectorization:**
    Where possible, the library uses SIMD vectorization to further optimize CPU usage during potentially intense calculation.
    At current, AVX2 is the only implementation supported, other implementations are WIP.



## 4. Error Handling
The library uses explicit integer return codes to signal success or failure.

| Code | Meaning |
| :--- | :--- |
| `MAT_SUCCESS` | Operation completed successfully. |
| `MAT_ERROR_NULL_POINTER` | A passed `matrix_s` pointer was invalid. |
| `MAT_ERROR_NULL_DATA_POINTER` | A passed `matrix_s -> data` pointer was invalid. |
| `MAT_ERROR_OUT_OF_BOUNDS` | Trying to access memory outside of the matrix' scope. |
| `MAT_ERROR_DIMENSION_MISMATCH` | Operands cannot be combined due to the mathematical requirements of the operation. (i.e. different inner dimensions in `mmul`) |
| `MAT_ERROR_NOT_SQUARE` | Expected a square matrix, but found a rectangular instead. |
| `MAT_ERROR_ALLOCATION_FAILED` | Matrix allocation failed due to unknown reasons. |
| `MAT_ALIASING_NOT_ALLOWED` | Input/Output buffers overlap. |
