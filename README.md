# Concurrent Programming Examples in C++

This repository contains several C++ examples that illustrate different aspects of concurrent programming. The examples cover thread synchronization, deadlock prevention and detection, and inter-process communication using named pipes (FIFOs). Each example builds upon the previous one to demonstrate the evolution from basic (and unsafe) concurrency to robust, deadlock-aware designs.

## Contents

- **phase1.cpp**  
  A basic implementation of deposit and withdraw functions without any synchronization. This file demonstrates potential data races when multiple threads update a shared global balance.

- **phase2.cpp**  
  An improved version of the banking example that uses a mutex to protect the shared bank balance during deposit and withdrawal operations, ensuring thread safety.

- **phase3.cpp**  
  A bank transfer example that implements a deadlock detection mechanism. The program uses resource allocation tracking to detect circular wait conditions between two accounts and recovers by releasing locks when a deadlock is detected.

- **phase4.cpp**  
  An advanced bank transfer example that incorporates both deadlock prevention and recovery strategies. It demonstrates:
  - **Resource Ordering:** Locks are acquired in a consistent order (based on account IDs) to prevent deadlocks.
  - **Timeout Mechanisms:** Each lock attempt has a timeout; if the lock cannot be acquired within a specified time, the thread releases any already-held locks to recover from potential deadlocks.

- **producer.cpp** and **consumer.cpp**  
  A producer-consumer example using named pipes (FIFOs) for inter-process communication:
  - **producer.cpp:** Generates a random number and sends it through a FIFO.
  - **consumer.cpp:** Receives the number, prints a Fibonacci sequence with that many terms, and sends a signal back to the producer.

## Getting Started

### Prerequisites

- A C++ compiler that supports C++11 (or later), such as `g++`.
- A POSIX-compliant operating system (Linux or macOS) for pthread and FIFO functionality.
- Basic familiarity with compiling and running programs from the command line.

### Compilation

Use the following commands from the repository root directory to compile each example:

- **phase1.cpp**
  ```bash
  g++ -pthread -o phase1 phase1.cpp
