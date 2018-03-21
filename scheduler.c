/*
 * If you compile this file with gcc, Please append -std=gnu99 as an extra compilation option.
 * e.g $ gcc -std=gnu99 scheduler.c -o shceduler
 * 
 * This program view the higher priority number as the higer priority.
 * The program combines the RoundRobin and Priority Scheduling Algorithm.
 * 
 * You must run this program under a unix-like operating system, you should give it a input file which
 * was redirected as standard input.
 * e.g ./scheduler < testData.input
 * 
 * Input Example:
 * 2 4 4           -> CPU numbers, Process numbers, Quantum size
 * 1 0 0 20 5 50   -> PID, Priority, SubmitTime, Total CPU time, CPU time, I/O
 * 2 1 1 20 5 50   -> PID, Priority, SubmitTime, Total CPU time, CPU time, I/O
 * 
 * 
 * You can verify the correctness of this program by opening the DEBUG_MODE.
 * e.g //#define DEBUG_MODE -> #define DEBUG_MODE
 */


#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>

#define MAX_SIZE 1000
/*#define DEBUG_MODE*/
#ifdef DEBUG_MODE
 #define DEBUG if(1)
#else
 #define DEBUG if(0)
#endif

int processNum = 0;
int cpuNum = 0;
int quantumSize = 0;
int clocks = 0;
int TurnaroundTime = 0;

/****** Process Struct *****/
typedef struct{
    int pid;
    int prio;
    int submit;
    int totcpu;
    int cpu;
    int io;
    int startTime;//A process start running at this time
    int endTime;//A process end at this time
    int currentburst;//For preemption
    int burst;//For switching from running to waiting
    int totburst;//Sync with clocks
    int waitTime;//I.O time
}process;
typedef process* process_p;

/***** Queue Struct *****/
typedef struct{
    process_p process[MAX_SIZE];
    int front, rear;
    int size;
}queue;
typedef queue* queue_p;

/***** CPU Struct *****/
typedef struct{
    int idle; 
    int idleTime;
    int works;
    process_p running; 
}cpu;
typedef cpu* cpu_p;

/***** Make and destory processes *****/
process_p makeProcess(int p, int pr, int subm, int totc, int cpu, int io);
void destroyProcess(process_p pre_ready[]);

/***** Make, initiate, destory queues *****/
queue_p makeQueue();
void initQueue(queue_p ptr);
void destroyQueue(queue_p ptr);

/***** Make and iniate CPU *****/
cpu_p makeCPU();
void initCPU(cpu_p CPU);

/***** Queue ADT *****/
bool IsEmpty(queue_p ptr);
bool IsFull(queue_p ptr);
void Enqueue(process_p proc, queue_p ptr);
void Dequeue(queue_p ptr);
process_p Front(queue_p ptr);

/***** The next process was scheduled *****/
process_p nextSchd(queue_p readyQueue);

/***** The process would be removed from waitingQueue *****/
process_p nextWaitRemove(queue_p waitingQueue);

/***** Update the running status and waiting status *****/
void updateWaitQueue(process_p waitingQueue[]);
void updateRunningQueue(cpu_p CPU[]);

void initWaitQueue(process_p waitingQueue[]);
void moveToWaitQueue(process_p waitingQueue[], process_p running);
void waitQueueToReady(process_p waitingQueue[], queue_p readyQueue);
int waitQueueSize(process_p waitingQueue[]);

/***** Print Functions *****/
void printInfo(int simn, int cpuNum, int processNum, int quantumSize, process_p pre_ready[], queue_p completedQueue, cpu_p CPU[]);
void CPUInfo(cpu_p CPU[]);


process_p makeProcess(int p, int pr, int subm, int totc, int cpu, int io)
{
    process_p P = (process_p)malloc(sizeof(process));
    P->pid = p;
    P->prio = pr;
    P->submit = subm;
    P->totcpu = totc;
    P->cpu = cpu;
    P->io = io;
    P->startTime = 0;
    P->endTime = 0;
    P->burst = 0;
    P->totburst = 0;
    P->waitTime = 0;
    P->currentburst = 0;
    return P;
}
queue_p makeQueue()
{
    queue_p Q = (queue_p)malloc(sizeof(queue));
    initQueue(Q);
    return Q;
}
cpu_p makeCPU()
{
    cpu_p C = (cpu_p)malloc(sizeof(cpu));
    initCPU(C);
    return C;
}
void destroyProcess(process_p pre_ready[])
{
    for(int i = 0; i < processNum; i++){
		free(pre_ready[i]);
	}
}
void destroyQueue(queue_p ptr)
{
    free(ptr);
}
void initQueue(queue_p ptr)
{
    ptr->front = -1;
    ptr->rear = -1;
    ptr->size = 0;
}
bool IsEmpty(queue_p ptr)
{
    return (ptr->front == -1 && ptr->rear == -1);
}
bool IsFull(queue_p ptr)
{
    return ((ptr->rear)+1)%MAX_SIZE == ptr->front ? true : false;
}
void Enqueue(process_p proc, queue_p ptr)
{
    if(IsFull(ptr))
    {
        printf("Queue is full\n");
        return;
    }
    if (IsEmpty(ptr))
    {
        ptr->front = ptr->rear = 0;
    }
    else
    {
        ptr->rear = ((ptr->rear)+1)%MAX_SIZE;
    }
    ptr->process[ptr->rear] = proc;
    (ptr->size)++;
}
void Dequeue(queue_p ptr)
{
    if(IsEmpty(ptr))
    {
        printf("Queue is empty\n");
        printf("Clocks %d\n", ptr->size);
        return;
    }
    else if(ptr->front == ptr->rear )
    {
        ptr->rear = ptr->front = -1;
        ptr->size = 0;
    }
    else
    {
        ptr->front = ((ptr->front)+1)%MAX_SIZE;
        (ptr->size)--;
    }
}
process_p Front(queue_p ptr)
{
    if((ptr->front) == -1)
    {
        printf("Can't return from front\n");
        return NULL;
    }
    return ptr->process[ptr->front];
}
process_p nextSchd(queue_p readyQueue)
{
    if(IsEmpty(readyQueue))
        return NULL;
    else
        return Front(readyQueue);
}
process_p nextWaitRemove(queue_p waitingQueue)
{
    if(waitingQueue->size == 0)
        return NULL;
    else
        return Front(waitingQueue);
}
void updateWaitQueue(process_p waitingQueue[])
{
    for(int i = 0; i < MAX_SIZE; i++){
        if(waitingQueue[i] != NULL){
            (waitingQueue[i]->waitTime)++;
            DEBUG printf("PID %d: I/O in progress (WaitTime %d)\n", waitingQueue[i]->pid, waitingQueue[i]->waitTime);
        }
    }
}
void updateRunningQueue(cpu_p CPU[])
{
    for(int i = 0; i < cpuNum; i++){
        if(CPU[i]->idle == 0){
            (CPU[i]->works)++;
            (CPU[i]->running->burst)++;
            (CPU[i]->running->totburst)++;
            (CPU[i]->running->currentburst)++;
            DEBUG printf("PID %d: running on CPU %d, burst++\n", CPU[i]->running->pid, i+1);
        }
        if(CPU[i]->idle == 1)
            (CPU[i]->idleTime)++;
    }
}
void initCPU(cpu_p CPU)
{
        CPU->idle = 1;
        CPU->idleTime = 0;
        CPU->works = 0;
        CPU->running = NULL;
}
void initWaitQueue(process_p waitingQueue[])
{
    for(int i = 0; i < MAX_SIZE; i++){
        waitingQueue[i] = NULL;
    }
}
void moveToWaitQueue(process_p waitingQueue[], process_p running)
{
    for(int i = 0; i < MAX_SIZE; i++){
        if(waitingQueue[i] == NULL){
            waitingQueue[i] = running;
            break;
        }
    }
}
void waitQueueToReady(process_p waitingQueue[], queue_p readyQueue)
{
    for(int i = 0; i < MAX_SIZE; i++)
    {
        if(waitingQueue[i] != NULL && waitingQueue[i]->waitTime != 0 && waitingQueue[i]->waitTime % waitingQueue[i]->io == 0){
            waitingQueue[i]->waitTime = 0;
            waitingQueue[i]->burst = 0;
            Enqueue(waitingQueue[i], readyQueue);
            DEBUG printf("PID %d is moving from waitingQueue to readyQueue.\n", waitingQueue[i]->pid);
            waitingQueue[i] = NULL;
        }        
    }   
}
int waitQueueSize(process_p waitingQueue[])
{
    int num = 0;
    for(int i = 0; i < MAX_SIZE; i++){
        if(waitingQueue[i] != NULL)
            num++;
    }

    return num;
}
void printInfo(int simn, int cpuNum, int processNum, int quantumSize, process_p pre_ready[], queue_p completedQueue, cpu_p CPU[])
{
    process_p tmp;
    printf("Simulation # %d\n", simn);
    puts("------------");
    puts("Input:");
    printf("       %d CPU, %d process, quantumsize = %d\n\n", cpuNum, processNum, quantumSize);
    for(int i = 0; i < processNum; i++){
        printf("       PID %d, prio = %d, submit = %d, totCPU = %d, CPU = %d, I/O = %d\n", pre_ready[i]->pid, pre_ready[i]->prio, pre_ready[i]->submit, pre_ready[i]->totcpu, pre_ready[i]->cpu, pre_ready[i]->io);
    }
    puts("Output:");
    while(!IsEmpty(completedQueue)){
        tmp = Front(completedQueue);
        printf("       PID %d completed execution at %d, turnaround time = %d\n", tmp->pid, tmp->endTime, tmp->endTime - tmp->startTime);
        TurnaroundTime += (tmp->endTime - tmp->startTime);
        Dequeue(completedQueue);
    }
    printf("       Average process turnaround time = %d\n", TurnaroundTime/processNum);
    CPUInfo(CPU);
    printf("\n");
}
void CPUInfo(cpu_p CPU[])
{
    for(int i = 0; i < cpuNum; i++){
        printf("       CPU %d: Running time = %d, Idle time = %d, Utilization = %4.2f%%\n", i+1, CPU[i]->works, CPU[i]->idleTime, ((float)CPU[i]->works/(float)(CPU[i]->works + CPU[i]->idleTime))*100);
    }
}
int main(void)
{
    queue_p readyQueue = makeQueue();
    process_p waitingQueue[MAX_SIZE];
    queue_p completedQueue = makeQueue();

    int simn = 1;
    while(scanf("%d %d %d\n", &cpuNum, &processNum, &quantumSize) != EOF){
        process_p pre_ready[processNum];
        cpu_p CPU[cpuNum];
        for(int i = 0; i < cpuNum; i++){
            CPU[i] = makeCPU();
            initCPU(CPU[i]);
        }
        initQueue(readyQueue);
        initWaitQueue(waitingQueue);
        initQueue(completedQueue);
        clocks = 0;
        for(int i = 0; i < processNum; i++){
            int pid, prio, submit, totcpu, cpu, io;
            scanf("%d %d %d %d %d %d\n", &pid, &prio, &submit, &totcpu, &cpu, &io);
            pre_ready[i] = makeProcess(pid, prio, submit, totcpu, cpu, io);
        }
        while(1){
            DEBUG printf("----------------------------------------------------------------------Clocks: %d\n", clocks);
            for(int i = 0; i < processNum; i++){
                if(pre_ready[i]->submit == clocks){
                    Enqueue(pre_ready[i], readyQueue);
                    DEBUG printf("Clocks: %d, PID %d's submitTime %d. Now, push it to readyQueue\n", clocks, pre_ready[i]->pid, pre_ready[i]->submit);
                }
            }
            for(int i = 0; i < cpuNum; i++){ //ready to running
                process_p tmp = nextSchd(readyQueue);
                if(CPU[i]->idle == 1 && tmp != NULL){
                    CPU[i]->running = tmp;
                    CPU[i]->idle = 0;
                    DEBUG printf("PID %d start to running.\n", CPU[i]->running->pid);
                    if(CPU[i]->running->totburst == 0){
                        CPU[i]->running->startTime = clocks;
                        DEBUG printf("                       StartTime: %d\n", clocks);
                    }
                    Dequeue(readyQueue);
                }
                if(CPU[i]->running != NULL && CPU[i]->running->totburst == CPU[i]->running->totcpu){ // running to completed
                        CPU[i]->running->endTime = clocks;
                        DEBUG printf("PID %d was completed.\n", CPU[i]->running->pid);
                        DEBUG printf("                       EndTime: %d\n", clocks);
                        Enqueue(CPU[i]->running, completedQueue);
                        CPU[i]->idle = 1;
                        CPU[i]->running = NULL;
                    }
                    if(CPU[i]->running != NULL && CPU[i]->running->currentburst != 0 && CPU[i]->running->currentburst % quantumSize == 0){ 
                        if(tmp != NULL){
                            if((CPU[i]->running->prio) > (tmp->prio)){
                                CPU[i]->idle = 0;
                                CPU[i]->running->currentburst = 0;
                                DEBUG printf("Running: PID %d, priority %d.\n", CPU[i]->running->pid, CPU[i]->running->prio);
                                DEBUG printf("Next   : PID %d, priority %d.\n", tmp->pid, tmp->prio);
                                DEBUG printf("@@@----> Keep PID %d running!\n\n", CPU[i]->running->pid);
                                }
                        
                            if((CPU[i]->running->prio) <= (tmp->prio)){ // running to ready
                                Enqueue(CPU[i]->running, readyQueue);
                                DEBUG printf("Running: PID %d, priority %d.\n", CPU[i]->running->pid, CPU[i]->running->prio);
                                DEBUG printf("Next   : PID %d, priority %d.\n", tmp->pid, tmp->prio);
                                DEBUG printf("###====> PID %d is going to run!\n\n", tmp->pid);
                                CPU[i]->idle = 1;
                                CPU[i]->running->currentburst = 0;
                                CPU[i]->running = NULL; 
                            }
                            
                        }
                    }
                    if(CPU[i]->running != NULL && CPU[i]->running->burst == CPU[i]->running->cpu){ // running to waiting
                        CPU[i]->running->currentburst = 0;
                        moveToWaitQueue(waitingQueue,CPU[i]->running); 
                        DEBUG printf("PID %d is moving to waitingQueue.\n", CPU[i]->running->pid);
                        CPU[i]->idle = 1;
                        CPU[i]->running = NULL;                    
                    }
                    
                    if(CPU[i]->idle == 1 && tmp != NULL){
                    CPU[i]->running = tmp;
                    CPU[i]->idle = 0;
                    DEBUG printf("PID %d start to running.\n", CPU[i]->running->pid);
                    if(CPU[i]->running->totburst == 0){
                        CPU[i]->running->startTime = clocks;
                        DEBUG printf("                       StartTime: %d\n", clocks);
                    }
                    Dequeue(readyQueue);
                }
            }          
            updateRunningQueue(CPU);
            updateWaitQueue(waitingQueue);
            waitQueueToReady(waitingQueue, readyQueue);        
            clocks++;
            if(completedQueue->size == processNum){
                break;
            }
        }
        printInfo(simn, cpuNum, processNum, quantumSize, pre_ready, completedQueue, CPU);
        TurnaroundTime = 0;
        simn++;
        destroyProcess(pre_ready);
    }
    destroyQueue(readyQueue);
    destroyQueue(completedQueue);
}
