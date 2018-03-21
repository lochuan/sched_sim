# sched_sim

此进程调度模拟器结合了Round Robin和Priority调度算法，可自定义CPU核心个数、Quantum size(Time Unit)。
输入文件需要定义进程个数、所需CPU时间、I/O时间、实际占用CPU时间转至I/O的时间间隔。

# Input Data
The input, which is to be read from the standard input, contains descriptions of one or more simulations; the simulations are numbered sequentially starting with 1.

The input for each simulation begins with a line containing three integers. In order, they are:
the number of CPUs in the system. ncpu (1 ≤ ncpu ≤ 4),
the number of processes, np (1 ≤ np ≤ 25), and
the quantum size, quant (quant ≥ 1).

Following the first line there will appear one line for each of the np processes. Each line contains six integers. In order, they are:
the unique process identification, pid (1 ≤ pid ≤ 999),
the process priority, prio (1 ≤ prio ≤ 10),
the time of submission, submit (submit ≥ 0),
the total CPU time required for the process to complete, totcpu (1 ≤ totcpu ≤ 1000),
the compute time required in each cycle before input/output is needed, cpu (1 ≤ cpu ≤ 100), and
the input/output time required in each cycle, io (1 ≤ io ≤ 1000).

The end of the input data is indicated by the end of file immediately after the input for the last simulation.


