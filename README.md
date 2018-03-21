# sched_sim

此进程调度模拟器结合了Round Robin和Priority调度算法，可自定义CPU核心个数、Quantum size(Time Unit)。
输入文件需要定义进程个数、所需CPU时间、I/O时间、实际占用CPU时间转至I/O的时间间隔。

# 示例输出

```
Simulation  1
------------
Input:
       2 CPU, 10 process, quantumsize = 2

       PID 1, prio = 0, submit = 0, totCPU = 20, CPU = 5, I/O = 50
       PID 2, prio = 1, submit = 1, totCPU = 20, CPU = 5, I/O = 50
       PID 3, prio = 1, submit = 1, totCPU = 20, CPU = 5, I/O = 50
       PID 4, prio = 2, submit = 1, totCPU = 20, CPU = 5, I/O = 50
       PID 5, prio = 1, submit = 1, totCPU = 20, CPU = 5, I/O = 50
       PID 6, prio = 1, submit = 1, totCPU = 20, CPU = 5, I/O = 50
       PID 7, prio = 3, submit = 1, totCPU = 20, CPU = 5, I/O = 50
       PID 8, prio = 1, submit = 1, totCPU = 20, CPU = 5, I/O = 50
       PID 9, prio = 1, submit = 1, totCPU = 20, CPU = 5, I/O = 50
       PID 10, prio = 1, submit = 1, totCPU = 20, CPU = 5, I/O = 50
Output:
       PID 7 completed execution at 177, turnaround time = 170
       PID 4 completed execution at 182, turnaround time = 179
       PID 9 completed execution at 187, turnaround time = 177
       PID 10 completed execution at 194, turnaround time = 182
       PID 8 completed execution at 200, turnaround time = 192
       PID 6 completed execution at 205, turnaround time = 199
       PID 5 completed execution at 209, turnaround time = 205
       PID 3 completed execution at 212, turnaround time = 210
       PID 2 completed execution at 214, turnaround time = 213
       PID 1 completed execution at 215, turnaround time = 215
       Average process turnaround time = 194
       CPU 1: Running time = 133, Idle time = 83, Utilization = 61.57%
       CPU 2: Running time = 67, Idle time = 149, Utilization = 31.02%
```
