# Operating Systems Project 2 - Thread Simulation & Synchronization

This project simulates process execution using threads and demonstrates synchronization using the Producer-Consumer problem in C.

## Features
- Process simulation using pthreads
- CPU burst time simulated using sleep()
- Producer-Consumer synchronization
- Uses mutex and semaphores
- Bounded buffer implementation

## Files
- project2.c → main source code
- processes.txt → input file with process IDs and burst times

## How to Run
```bash
gcc project2.c -o project2 -pthread
./project2
