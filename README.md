# 🔒 Memory Fencing Optimization for LLVM

[![LLVM](https://img.shields.io/badge/LLVM-Compiler_Pass-blue.svg)](https://llvm.org/)
[![C++17](https://img.shields.io/badge/C++-17-blue.svg)](https://isocpp.org/)
[![CMake](https://img.shields.io/badge/CMake-3.12+-green.svg)](https://cmake.org/)
[![License](https://img.shields.io/badge/License-MIT-Green.svg)](LICENSE)

A sophisticated LLVM compiler pass that implements **memory fencing optimization** for concurrent programs under different **relaxed memory models**. This project provides automated fence insertion and optimization techniques for **Total Store Ordering (TSO)** and **Partial Store Ordering (PSO)** memory models.

**Sequentially-Consistent (SC) fences** enforce a happens-before relationship between operations preceding and following the fence, guaranteeing that all operations before the fence complete prior to executing those after it, both at hardware and compiler levels. Fences are inserted during the LLVM IR stage, allowing the backend to convert them to target architecture instructions or ignore them when unnecessary (e.g., SC fences are essentially ignored on x86).

## 🎯 Project Overview

This project implements:

- **🚧 Fence Insertion Passes**: Automatically insert memory fences to ensure program correctness under TSO and PSO memory models
- **⚡ Fence Optimization**: Use max-flow min-cut algorithms to remove redundant fences while maintaining correctness
- **🧪 Litmus Test Suite**: Comprehensive testing with classical concurrent programming patterns

### Memory Models Supported

| Memory Model | Description | Use Case |
|--------------|-------------|----------|
| **TSO** (Total Store Ordering) | Allows load-load, load-store, and store-store reordering, but prohibits store-load reordering | x86/x64 architectures |
| **PSO** (Partial Store Ordering) | More relaxed than TSO, allows store-store reordering | SPARC architectures |

### 🔧 Fence Insertion Rules

The passes insert fences based on consecutive atomic memory operation pairs. **Key insight**: Fences are only introduced between two relaxed atomic memory operations, as C11 memory ordering for other operations already provides the required guarantees.

| Op1 | Order1 | Op2 | Order2 | TSO | PSO |
|-----|--------|-----|--------|----- |-----|
| R | RLX | R | RLX | R-**Fence**-R | R-**Fence**-R |
| R | RLX | W | RLX | R-**Fence**-W | R-**Fence**-W |
| W | RLX | R | RLX | R-W | R-W |
| W | RLX | W | RLX | W-**Fence**-W | W-W |
| R | ACQ | * | * | R-Op2 | R-Op2 |
| * | * | W | REL | Op1-W | Op1-W |
| W | REL | R | ACQ | W-R | W-R |
| * | SEQ_CST | * | SEQ_CST | Op1-Op2 | Op1-Op2 |
| * | ACQ_REL | * | ACQ_REL | Op1-Op2 | Op1-Op2 |

> **Note**: The main difference between TSO and PSO is that PSO allows Write-Write reordering while TSO does not.

## 🏗️ Architecture

The project consists of three main LLVM passes:

```
📁 fencing/
├── 🔧 FenceTSO.cpp          # TSO fence insertion pass
├── 🔧 FencePSO.cpp          # PSO fence insertion pass  
├── ⚡ FenceOptimization.cpp # Max-flow based fence optimization
└── 📋 FencingPasses.h       # Pass interface definitions
```

### 🧠 Algorithm Overview

**Fence Insertion Algorithm**: Both TSO and PSO passes traverse each function starting from the initial basic block, following all possible control flows while tracking the last atomic memory operation. When a consecutive operation pair that is "too relaxed" appears, the algorithm inserts **two fences per operation pair**: one after the initial operation and one before the second operation.

**Fence Optimization Algorithm**: 
1. **Graph Construction**: Build a special graph by removing fence instructions and inserting nodes before/after their locations
2. **Network Flow**: Create source/sink connections to form a network flow graph
3. **Min-Cut Calculation**: Apply Ford-Fulkerson max-flow algorithm to find the minimum cut
4. **Optimal Placement**: Insert fences only at min-cut locations for optimal performance

This approach ensures **minimal fence overhead** while maintaining program correctness.

### 🧪 Litmus Tests

The project includes implementations of classical concurrent programming patterns used to validate fence synthesis:

- **MP** (Message Passing): Producer-consumer synchronization pattern
- **SB** (Store Buffering): Symmetric store-load pattern testing write-read reordering
- **LB** (Load Buffering): Circular dependency pattern testing read-write reordering  
- **IRIW** (Independent Reads of Independent Writes): Multi-reader consistency pattern
- **Branch Control Flow**: Tests fence insertion across conditional branches

Each test covers different consecutive memory operation types (read-write, write-write, write-read) with various memory ordering combinations to ensure comprehensive validation of both TSO and PSO fence synthesis methods.

## 🚀 Getting Started

### Prerequisites

- **LLVM 14+** with development headers
- **CMake 3.12+**
- **C++17** compatible compiler
- **llvm-lit** for running tests

### 🔨 Building the Project

```bash
# Configure with CMake
cmake -DLLVM_LIT=$(which lit) -DLLVM_DIR=/usr/lib64/cmake/llvm -S . -B build

# Build the passes
cmake --build build

# Navigate to build directory
cd build && make
```

> **💡 Tip**: Adjust `LLVM_DIR` to point to your LLVM installation's cmake directory.

### 🎮 Running the Passes

#### TSO Fence Insertion
```bash
opt -load-pass-plugin ../build/fencing/FencingPass.so -S -passes=fence-tso input.ll
```

#### PSO Fence Insertion  
```bash
opt -load-pass-plugin ../build/fencing/FencingPass.so -S -passes=fence-pso input.ll
```

#### Fence Optimization
```bash
opt -load-pass-plugin ../build/fencing/FencingPass.so -S -passes=fence-tso,fence-opt input.ll
```

### 🧪 Testing

Run the comprehensive test suite:

```bash
# Run all tests
make test

# Run LLVM lit tests specifically  
make run-lit-tests
```
## 🎓 Educational Value

This project demonstrates several important concepts:

- **🔄 Relaxed Memory Models**: Understanding how modern processors reorder memory operations
- **🛡️ Correctness Preservation**: Ensuring concurrent programs behave as expected
- **📈 Optimization Theory**: Applying graph algorithms (max-flow min-cut) to compiler optimization
- **🏗️ LLVM Development**: Building production-quality compiler passes

## 📚 Research Background

**Authors**: Mihnea Bernevig, Yigit Colakoglu

This implementation builds upon established research in memory fencing optimization:

- **Sequential Consistency**: Ensuring program order is preserved across threads
- **Fence Insertion Algorithms**: Systematic approaches to placing memory barriers  
- **Min-Cut Optimization**: Based on "Partially redundant fence elimination for x86, arm, and power processors" (Morisset & Zappa Nardelli, 2017)
- **Relaxed Memory Models**: Understanding hardware-level memory reordering behaviors

The project demonstrates practical application of **graph theory** (max-flow min-cut) to **compiler optimization**, bridging theoretical computer science with systems programming.

## 📄 Project Structure

```
cs4560_fencing/
├── 📁 fencing/           # Core LLVM passes
├── 📁 litmus_tests/      # C++ concurrent test programs  
├── 📁 tests/            # LLVM IR test cases
├── 📄 CMakeLists.txt    # Build configuration
└── 📄 README.md         # This file
```

## References
 - Morisset, R., & Zappa Nardelli, F. (2017). *Partially redundant fence elimination for x86, arm, and power processors*. In **Proceedings of the 26th International Conference on Compiler Construction (CC 2017)** (pp. 1–10). Austin, TX, USA: Association for Computing Machinery. https://doi.org/10.1145/3033019.3033021

