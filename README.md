# Multithreading Concurrency and IPC in C++

## Overview
This project demonstrates various techniques in multithreading, concurrency, and inter-process communication (IPC) using C++. It is divided into several parts:

- **Phase 1 & Phase 2:** Illustrate basic bank operations (deposit and withdraw) with and without proper mutex protection.
- **Phase 3 & Phase 4:** Implement account transfers with different strategies to handle deadlocks. Phase 3 uses deadlock detection, while Phase 4 employs resource ordering and timeout-based locking for deadlock prevention and recovery.
- **Producer-Consumer Example:** Uses POSIX named pipes (FIFOs) to implement a simple producer-consumer system. The producer generates a random number, and the consumer computes and displays a Fibonacci sequence based on that number.

## Dependencies
- **C++ Compiler:** A C++11 (or later) compliant compiler (e.g., `g++`).
- **POSIX Threads (pthreads):** Required for multithreading support.
- **POSIX FIFO Support:** Necessary for the producer-consumer example (most Unix-like systems provide this by default).

## Installation
1. **Install g++ (if not already installed):**
   - **Ubuntu/Debian:**
     ```bash
     sudo apt update
     sudo apt install g++
     ```
   - **Fedora:**
     ```bash
     sudo dnf install gcc-c++
     ```
   - **macOS (with Homebrew):**
     ```bash
     brew install gcc
     ```

2. **Ensure pthread library is available:**  
   This library is typically included on Unix-like systems.

3. **Clone the Repository:**
   ```bash
   git clone https://github.com/yourusername/your-repository.git
   cd your-repository
