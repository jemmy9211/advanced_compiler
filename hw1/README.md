# Homework 1: LLVM Pass for Dependence Analysis

This homework implements an LLVM pass that performs dependence analysis on C code. The pass identifies three types of data dependencies:

*   **Flow Dependence (Read-after-Write):** Occurs when an instruction reads a value written by a previous instruction.
*   **Anti-Dependence (Write-after-Read):** Occurs when an instruction writes to a location that was previously read by another instruction.
*   **Output Dependence (Write-after-Write):** Occurs when two instructions write to the same memory location.

The analysis is performed by iterating through the instructions within basic blocks of a function. The pass specifically looks for patterns in `for` loops to determine loop bounds and array access indices.

A core part of the analysis involves solving Diophantine equations to determine if dependencies exist between different loop iterations. The `diophantine` function in `hw1.cpp` is responsible for this calculation, considering array access patterns like `A[i]` and `A[i-c]`.

The pass outputs the identified dependencies, indicating the type of dependency and the involved instructions (or statements, represented as `S<index>`).

## How to Build and Run

(Instructions on how to build the LLVM pass and run it on test C files would typically go here. This might involve using `opt` with the compiled pass and providing an LLVM IR file as input.)

## Test Cases

The `test1.c` and `test2.c` files provide examples of C code that can be used to test the dependence analysis pass.
