# OperatingSystemsProject3
 A C program for Linux that provides searching over a directory structure, and finding prime numbers in files within that structure.

Job organization
It is necessary to implement two types of threads within the homework that are created and stopped as needed during the application's runtime. These two threads are: watcher and worker. Each watcher thread will bind to a directory, and monitor it for changes. When a new file is found, or a change to a file is detected, a new worker thread will be started for that file. Each worker thread will bind to a file, and will do the job of counting primes within that file. The results of this enumeration are placed in a forest that has the same shape as the directory structure - the roots of the trees are the starting directories, the leaves contain the numbers of primes in the individual files, and the nodes contain the sums for the directories.

Watcher threads
The watcher thread is bound to a directory, and should scan that directory in an infinite loop, as well as block for 10 seconds. The following checks are performed in one scan:
Did a new file appear? If so, you should employ a worker thread that will find how many prime numbers there are in that file, and report the result.
Has any file for which we have a result changed since the last count. For each file for which we have a result, the time of the last modification of that file should be stored. If that time has changed, then the file should be reprocessed by a worker thread.
Has a new directory appeared. If so, a new watcher thread should be created to monitor that directory. If a watcher thread already exists for a directory, you do not need to create a new watcher thread.
Has any file been removed. If so, its result should be removed and all nodes above the removed leaf should be refreshed.
Has any subdirectory been removed. If so, all nodes above that node should be refreshed.
Has our directory been removed. If so, the watcher thread should be killed.

Worker threads
The application should have a fixed number of worker threads that is set at application startup by asking the user to enter a value. Every time a watcher thread finds a new or modified file, it will employ one of these worker threads to perform a count on the file. If there are no worker threads available, the watcher thread must not block. A job that was not started because there were no free workers must not be lost, but should be done as soon as a worker thread becomes free. Make sure that no work is duplicated due to the rescanning of the directory by the watcher thread. The worker thread should be blocked if it is not doing some counting.

Files processed will always be ASCII encoded files, and will contain arbitrary text. The job of the worker thread is to read the entire file and find all the prime numbers in it. As a result of the work, the number of prime numbers in the file should be obtained. We will assume that these files can be very large, so it is necessary for these threads to report partial results as well. For every 1KB of data processed, the file should report the result as partial. When the worker thread finishes counting, it should write the result as final. A sheet representing a file should contain the number of primes within that file, while the nodes above it should contain the sum of primes for the directory they represent.

Interaction with the user
The user should be able to interact with the system completely freely, regardless of whether any jobs are performed in the watcher and worker threads. The user can issue the following commands to the system:
add_dir <dir>
The argument to this command is a directory, given as a relative or absolute path. This directory is the new root for our system, and this command should create a watcher thread for it. If this directory is already present in the system, an error should be reported. There is no need to go deeper into this directory at this point, because its watcher thread will create watcher threads for subdirectories.
remove_dir <dir>
The argument to this command is a directory that is the root of one of the trees being viewed. If a path is specified where this is not the case, an error should be reported. The specified tree is removed from the results, and all watcher threads for the directories in that tree are killed.
result [path]
The argument to this command is the path within the result forest - it can be a directory or a file. If the path is for a file, then the result should be printed for that file, and if it is a directory, the number of prime numbers in that directory should be printed in a structured way, as well as for all its files and subdirectories. If the directory DirA contains the files FileA1, FileA2 and a subdirectory DirB with the files FileB1 and FileB2, the output might be of the form:
-DirA: 30
--DirB: 20
---FileB1: 10
---FileB2: 10
--FileA1: 5
--FileA2: 5

Partial results (ie results where some worker thread is still processing a file) are printed with an asterisk (*) next to the number. This asterisk will also typically be next to any directories above the file, as their results will also be partial at that point. For example, if the count is currently being performed for FileB1, and only 8 primes have been found so far, we will get the printout:


-DirA: 28*
--DirB: 18*
---FileB1: 8*
---FileB2: 10
--FileA1: 5
--FileA2: 5

The argument for the result command is optional, and if it is omitted, then all results should be printed, i.e. the command behaves as if it were invoked for all roots


Competitiveness
Worker threads should be created at application startup and typically be in a blocked state. They are activated as needed by the watcher thread.

Watcher threads are created when the add_dir command is executed, or when a new directory is found by another watcher thread. These threads are killed when deleting a directory (outside the application), or when executing the remove_dir command for the root, when all watcher threads in that tree are killed.

The result tree should always be consistent - it must not happen that phallic results are written into it at any time, or that the tree is not validly formed and that some thread can access an invalid pointer. It is not allowed to use one mutex to control the entire tree, but it is necessary to lock only those elements that are really necessary to be locked at a given moment.
