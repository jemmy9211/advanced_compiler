# Homework 2: LLVM Pass for Points-To and Dataflow Analysis

This homework implements an LLVM pass that appears to perform points-to analysis and gather various dataflow information within C functions. This type of analysis is crucial for understanding memory aliasing and for enabling various compiler optimizations.

The pass analyzes LLVM Intermediate Representation (IR) and outputs the following information for each relevant instruction (statement):

*   **`TREF` (Target References):** Variables or memory locations referenced by the instruction. This can include direct variable access or dereferenced pointers (e.g., `*p`, `**pp`).
*   **`TGEN` (Target Generations/Definitions):** Variables or memory locations defined (written to) by the instruction. This also handles pointer dereferences.
*   **`DEP` (Dependencies):** Identifies data dependencies between the current instruction and previous definitions. It seems to distinguish between:
    *   **Flow Dependencies (`--->`):** Read-after-write.
    *   **Output Dependencies (`-O->`):** Write-after-write.
*   **`TDEF` (Target Definitions):** A set of active definitions, mapping variables/memory locations to the statement that defined them.
*   **`TEQUIV` (Target Equivalences):** Tracks equivalences between expressions or memory locations, often involving pointers. For example, if `p = &x`, then `*p` is equivalent to `x`.

The analysis seems to process instructions sequentially within basic blocks, updating the dataflow sets (`TREF`, `TGEN`, `TDEF`, `TEQUIV`) and identifying dependencies based on these sets.

## Purpose of Analysis

The information gathered by this pass can be used for:

*   **Alias Analysis:** Determining if different pointer expressions can refer to the same memory location.
*   **Compiler Optimizations:** Enabling optimizations like dead code elimination, constant propagation, and instruction scheduling by providing accurate data dependency and aliasing information.
*   **Program Understanding:** Helping to understand complex pointer manipulations and data flow within a program.

## How to Build and Run

(Instructions on how to build the LLVM pass and run it on test C files would typically go here. This might involve using `opt` with the compiled pass and providing an LLVM IR file as input.)

## Test Cases

The directory includes several test files (`test1.c`, `test2.c`, `test3.c`, `test4.c`) that demonstrate C code with various pointer manipulations and assignments, which can be used to test the analysis capabilities of this LLVM pass.
