CSCI3120 Assignment 5: CPU Scheduling – PSJF and NSJF – Readme
================================
By  : Sanghani Jay N.
B00 : B00937799

Description:
-------------
This project implements a CPU scheduler simulation in C. The simulation supports two scheduling algorithms:
• Non-preemptive Shortest-Job-First (NSJF): Tasks are scheduled based on the length of their next CPU burst. Once a task is started, it runs to completion without
	being preempted.
• Preemptive Shortest-Job-First (PSJF): Tasks are scheduled based on the length of their next CPU burst. If a new task arrives with a CPU burst shorter than
	the remaining burst time of the currently executing task, the current task is preempted.

Files Included:
-------------
    1) main.c: The main C source file containing the simulation code, including the implementation of both NSJF and
        PSJF algorithms.
    2) TaskSpec.txt: The input file specifying the tasks. Each line follows the format:
        [task name],[arrival time],[burst time]
    3) Output.txt: The output file where the scheduling results are written. The file contains the execution sequence
        for each algorithm, the waiting time for each task, and the average waiting time.

Compilation:
-------------
To compile the program run the following command:
    gcc main.c

A file named a.out should be generated in the same directory as the main.c.

Execution:
----------
After successful compilation, run the program by executing the command:
    ./a.out

Upon running, the program will read the input from the file named "TaskSpec.txt" present in the same directory.

Then the sorted list will be written in the file named "Output.txt", also in the same directory.
