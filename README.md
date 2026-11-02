# Unix Client-Server Communication — IPC & Process Synchronization

A multi-process client-server application developed in **C** as part of a Systems Programming project at **Grenoble INP – Phelma** (November 2024).

The project focuses on Unix inter-process communication (IPC), concurrent execution, shared memory management, and process synchronization.

## Project Overview

The application implements a client capable of connecting to a server, retrieving data from two independent channels, and forwarding it to a display module.

The main challenge was ensuring reliable communication between multiple concurrent processes while preventing data corruption and synchronization issues.

## Architecture

The client follows a modular architecture with six main components:

| Component | Responsibility |
|---|---|
| `CL.c` | Main process, initialization, resource management and cleanup |
| `moniteur.c` | Server connection, process creation and supervision |
| `lecteur.c` | Reading data from shared memory buffers |
| `redacteur.c` | Processing and forwarding data through pipes |
| `driver.c` | Displaying received data in batches |
| `sem.c` | Semaphore management and synchronization |

The monitor creates four child processes using `fork()`:

- Two **Reader processes**, one for each data channel.
- Two **Writer processes**, responsible for forwarding the corresponding data.

Readers and writers communicate through Unix pipes, while semaphores coordinate access to shared resources.

## Key Features

### Concurrent Process Management

- Multi-process architecture based on `fork()`.
- Independent reader-writer pairs for each data channel.
- Process lifecycle management and resource cleanup.
- Comparison of processes and threads to guide architectural decisions.

### Inter-Process Communication

- **Shared memory:** Access to data buffers maintained by the server.
- **Semaphores:** Synchronization and mutual exclusion between concurrent processes.
- **Unix pipes:** Communication between readers, writers, and the display module.
- **Circular buffers:** Structured data storage and retrieval.
- **Message queues:** Initial communication and access coordination with the server.

### Synchronization and Reliability

- Protection of critical sections using semaphores.
- Signal handling and signal masks using `sigprocmask()`.
- Error handling for IPC operations, memory access, and process management.
- Debugging of concurrency and synchronization issues.

## Technologies

- **Language:** C
- **Environment:** Unix / Linux
- **System programming:** `fork()`, pipes, shared memory, semaphores
- **Concurrency:** Process synchronization, critical sections, signal handling
- **Architecture:** Modular software design and IPC

## Technical Challenges

### Process Synchronization

Coordinating readers and writers required careful management of shared resources and execution order.

Different synchronization strategies were evaluated, including `sleep()`, `pause()`, and signal masking.

The final implementation primarily uses semaphore-based synchronization, with fixed delays for certain operations and signal masks to protect critical sections.

### Architecture and Debugging

Integrating the display driver revealed several dependencies between processes and communication mechanisms.

Debugging involved tracing execution across the client and server to identify synchronization and data transmission issues.

### Performance Trade-offs

The implementation prioritizes reliability, modularity, and error handling over maximum performance.

Potential improvements include:

- Replacing fixed delays with event-driven synchronization.
- Reducing redundant error-handling code.
- Improving buffer management.
- Reducing process and communication overhead.

## Learning Outcomes

This project strengthened my understanding of:

- Unix systems programming.
- Concurrent and multi-process architectures.
- Inter-process communication mechanisms.
- Shared memory and synchronization.
- Modular C programming.
- Debugging and reliability in concurrent systems.

## Academic Context

**Institution:** Grenoble INP – Phelma  
**Program:** Embedded Systems and Connected Devices (SEOC)  
**Course:** Client-Server Systems Programming  
**Date:** November 2024

---

*Academic project focused on Unix systems programming and inter-process communication.*
