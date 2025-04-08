# Instruction Reordering Visualization

## Overview

This project demonstrates how modern CPUs use Reorder Buffers (ROB) to optimize execution by reordering independent instructions and reducing latency. It benchmarks various code patterns—some with independent operations and others with data dependencies—to show the performance impact of instruction-level parallelism.

## How It Works

Modern CPUs use out-of-order execution to maximize efficiency. At the core of this is the Reorder Buffer (ROB), a hardware structure that allows instructions to execute in a different order than they appear in code—provided there are no data dependencies. Once execution completes, the ROB ensures that results are committed in program order.

This project compares different scenarios:

1. Independent arithmetic operations  
2. Independent memory accesses  
3. Mixed latency operations (CPU + memory)  
4. Sequentially dependent operations  

By measuring execution time for each case, we can observe how the ROB overlaps independent instructions to reduce total runtime. The last case (sequential dependencies) serves as the baseline where no reordering is possible.

## Build & Run

Follow these steps to build and execute the project:

```bash
# Clone the repository
git clone https://github.com/AniDashyan/instruction_reordering_visualization.git
cd instruction_reordering_visualization

# Configure the project with CMake
cmake -S . -B build

# Build the project
cmake --build build --config Release

# Run the benchmark with the desired number of iterations (default is 5,000,000)
./build/rob --iters [N]
```

Replace [N] with the number of iterations you want to use.

## Example Output

```
Warning: --iters option is not provided using the default value 5000000 

=== Performance Results ===
Case Description               Time (us) ROB Status          % Diff
----------------------------------------------------------------- 
1. Independent Arithmetic         17897 Triggered            34.0%
2. Independent Memory             24457 Triggered             9.8%
3. Mixed Latency                   8781 Triggered            67.6%
4. Sequential Dependent           27100 Not Triggered            -
----------------------------------------------------------------- 
```

In this example, you can see that independent operations execute faster due to ROB optimization, while sequentially dependent code cannot be optimized as easily.
