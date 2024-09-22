# OperatingSystemsProject3

This project implements a C program for Linux that allows searching through a directory structure and finding prime numbers in the files within that structure.

## Job Organization
The application utilizes two types of threads: **watcher** and **worker** threads. These threads are created and managed dynamically during the application's runtime.

- **Watcher Threads:** Each watcher thread is bound to a directory and monitors it for changes. When a new file is detected or a file is modified, a new worker thread is spawned for that file. The results are organized in a tree structure that mirrors the directory hierarchy, with the roots representing starting directories, the leaves containing the counts of prime numbers, and the nodes containing sums for directories.

- **Worker Threads:** The application initializes a fixed number of worker threads based on user input at startup. When a watcher thread identifies a new or modified file, it assigns the task to an available worker thread. If no workers are free, the watcher does not block; instead, it queues the task for execution when a worker becomes available. Worker threads perform the prime number counting and report partial results as they process the files.

## Watcher Threads
Watcher threads operate in an infinite loop, scanning their bound directory every 10 seconds. Each scan checks for:
- **New files:** A worker thread is initiated to count prime numbers in the newly added file.
- **Modified files:** If a previously processed file has been modified, it is reprocessed by a worker thread.
- **New directories:** If a new directory is detected, a new watcher thread is created for it (unless one already exists).
- **Removed files or directories:** The results for removed files are deleted, and the nodes above them are refreshed. If a directory is removed, the corresponding watcher thread is terminated.

## Worker Threads
Worker threads are initially blocked and become active as needed. They read entire ASCII-encoded files and identify prime numbers. The results are reported in two stages:
1. **Partial results:** Reported for every 1KB of data processed.
2. **Final result:** Reported once the entire file has been processed.

The structure of the result tree ensures that files contain the number of primes found, while nodes above them reflect the sum of primes for their respective directories.

## User Interaction
Users can issue commands to interact with the system freely, regardless of ongoing tasks in the watcher and worker threads.

### Commands
- **`add_dir <dir>`**
  - Adds a directory (specified as a relative or absolute path) and creates a watcher thread for it. An error is reported if the directory already exists.

- **`remove_dir <dir>`**
  - Removes the specified directory and its corresponding tree, terminating all associated watcher threads. An error is reported if the directory is not a root of any tree.

- **`result [path]`**
  - Displays results for the specified path within the result tree. If the path is for a file, the count is printed; if it's a directory, the counts for all files and subdirectories are displayed in a structured format. For example:
    ```
    -DirA: 30
    --DirB: 20
    ---FileB1: 10
    ---FileB2: 10
    --FileA1: 5
    --FileA2: 5
    ```

  Partial results are indicated with an asterisk (`*`) next to the count, signifying ongoing processing.

## Competitiveness
- Worker threads are created at startup and typically remain blocked until activated by watcher threads.
- Watcher threads are initiated via the `add_dir` command or when new directories are detected.
- The result tree must maintain consistency, avoiding invalid pointers and ensuring that only necessary elements are locked during operations.

This implementation allows efficient directory monitoring and prime number counting, providing users with structured and real-time results.
