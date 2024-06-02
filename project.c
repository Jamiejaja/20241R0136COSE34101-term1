/* Library
<stdio.h> -> to use the functions addressing I/O
<stdlib.h> -> utilities
*/
#include <stdio.h>
#include <stdlib.h>

/* Macro
- time_slice for RR, time period each of the process takes part in CPU
- io_duration, when there's an I/O interrupt occured the very process has to be waited as much
*/
#define MAX_PROCESSES 10
#define TIME_SLICE 2
#define IO_DURATION 3

/* Define Process
 */
typedef struct
{
    int pid;
    int burst_time;
    int arrival_time;
    int remaining_time;
    int io_interrupt;
    int waiting_time;
    int turnaround_time;
    int io_waiting_time;
    int in_io;
    int priority;
} Process;

/* define the functions below
 */
void create_processes(Process processes[], int n);
void fcfs(Process processes[], int n);
void sjf(Process processes[], int n);
void priority_scheduling(Process processes[], int n);
void round_robin(Process processes[], int n);
void preemptive_sjf(Process processes[], int n);
void preemptive_priority(Process processes[], int n);
void evaluate_performance(Process processes[], int n, const char *scheduler_name);

/* let user produce processes(n) and choose one of the algoritms(choice)
 */
int main()
{
    Process processes[MAX_PROCESSES];
    int n, choice;

    printf("몇 개의 프로세스를 생성하시겠습니까?: ");
    scanf("%d", &n);

    create_processes(processes, n);

    printf("스케줄링 방법을 선택하세요:\n");
    printf("1. FCFS\n");
    printf("2. SJF\n");
    printf("3. Priority\n");
    printf("4. Round Robin\n");
    printf("5. Preemptive SJF\n");
    printf("6. Preemptive Priority\n");
    scanf("%d", &choice);

    switch (choice)
    {
    case 1:
        fcfs(processes, n);
        break;
    case 2:
        sjf(processes, n);
        break;
    case 3:
        priority_scheduling(processes, n);
        break;
    case 4:
        round_robin(processes, n);
        break;
    case 5:
        preemptive_sjf(processes, n);
        break;
    case 6:
        preemptive_priority(processes, n);
        break;
    default:
        printf("Invalid choice\n");
        break;
    }

    return 0;
}

/* Create processes
 */
void create_processes(Process processes[], int n)
{
    for (int i = 0; i < n; i++)
    {
        processes[i].pid = i + 1;
        printf("burst time for process %d: ", i + 1);
        scanf("%d", &processes[i].burst_time);
        printf("arrival time for process %d: ", i + 1);
        scanf("%d", &processes[i].arrival_time);
        printf("I/O interrupt time for process %d (enter -1 if none): ", i + 1);
        scanf("%d", &processes[i].io_interrupt);
        printf("priority for process %d: ", i + 1);
        scanf("%d", &processes[i].priority);
        processes[i].remaining_time = processes[i].burst_time;
        processes[i].waiting_time = 0;
        processes[i].turnaround_time = 0;
        processes[i].io_waiting_time = 0;
        processes[i].in_io = 0;
    }
}

/* I/O handler
reduce I/O waiting time(time period undergoing I/O interrupt), if it becomes 0, then It's done
*/
void handle_io(Process processes[], int n, int time)
{
    for (int i = 0; i < n; i++)
    {
        if (processes[i].in_io)
        {
            processes[i].io_waiting_time--;
            if (processes[i].io_waiting_time == 0)
            {
                processes[i].in_io = 0;
            }
        }
    }
}

/* evaluate performance
with total waiting time and turnaround time
*/
void evaluate_performance(Process processes[], int n, const char *scheduler_name)
{
    int total_waiting_time = 0;
    int total_turnaround_time = 0;

    for (int i = 0; i < n; i++)
    {
        total_waiting_time += processes[i].waiting_time;
        total_turnaround_time += processes[i].turnaround_time;
    }

    float avg_waiting_time = (float)total_waiting_time / n;
    float avg_turnaround_time = (float)total_turnaround_time / n;

    printf("%s Scheduling:\n", scheduler_name);
    for (int i = 0; i < n; i++)
    {
        printf("Process %d - Waiting Time: %d, Turnaround Time: %d\n", processes[i].pid, processes[i].waiting_time, processes[i].turnaround_time);
    }
    printf("Average Waiting Time: %.2f\n", avg_waiting_time);
    printf("Average Turnaround Time: %.2f\n\n", avg_turnaround_time);
}

/* fcfs scheduling
 */
void fcfs(Process processes[], int n)
{
    int time = 0, completed = 0;

    while (completed < n)
    {
        for (int i = 0; i < n; i++)
        {
            if (processes[i].arrival_time <= time && processes[i].remaining_time > 0 && !processes[i].in_io)
            {
                if (processes[i].io_interrupt == time)
                {
                    processes[i].in_io = 1;
                    processes[i].io_waiting_time = IO_DURATION;
                }
                else
                {
                    processes[i].remaining_time--;
                    if (processes[i].remaining_time == 0)
                    {
                        completed++;
                        processes[i].turnaround_time = time + 1 - processes[i].arrival_time;
                        processes[i].waiting_time = processes[i].turnaround_time - processes[i].burst_time;
                    }
                }
                time++;
                break;
            }
        }

        handle_io(processes, n, time);

        if (completed == n)
        {
            break;
        }
    }

    evaluate_performance(processes, n, "FCFS");
}

/* sjf scheduling
 */
void sjf(Process processes[], int n)
{
    int time = 0, completed = 0, shortest;

    while (completed < n)
    {
        shortest = -1;
        for (int i = 0; i < n; i++)
        {
            if (processes[i].arrival_time <= time && processes[i].remaining_time > 0 && !processes[i].in_io)
            {
                if (shortest == -1 || processes[i].remaining_time < processes[shortest].remaining_time)
                {
                    shortest = i;
                }
            }
        }

        if (shortest != -1)
        {
            if (processes[shortest].io_interrupt == time)
            {
                processes[shortest].in_io = 1;
                processes[shortest].io_waiting_time = IO_DURATION;
            }
            else
            {
                processes[shortest].remaining_time--;
                if (processes[shortest].remaining_time == 0)
                {
                    completed++;
                    processes[shortest].turnaround_time = time + 1 - processes[shortest].arrival_time;
                    processes[shortest].waiting_time = processes[shortest].turnaround_time - processes[shortest].burst_time;
                }
            }
            time++;
        }
        else
        {
            time++;
        }

        handle_io(processes, n, time);
    }

    evaluate_performance(processes, n, "SJF");
}

/* priority scheduling
 */
void priority_scheduling(Process processes[], int n)
{
    int time = 0, completed = 0, highest_priority;

    while (completed < n)
    {
        highest_priority = -1;
        for (int i = 0; i < n; i++)
        {
            if (processes[i].arrival_time <= time && processes[i].remaining_time > 0 && !processes[i].in_io)
            {
                if (highest_priority == -1 || processes[i].priority < processes[highest_priority].priority)
                {
                    highest_priority = i;
                }
            }
        }

        if (highest_priority != -1)
        {
            if (processes[highest_priority].io_interrupt == time)
            {
                processes[highest_priority].in_io = 1;
                processes[highest_priority].io_waiting_time = IO_DURATION;
            }
            else
            {
                processes[highest_priority].remaining_time--;
                if (processes[highest_priority].remaining_time == 0)
                {
                    completed++;
                    processes[highest_priority].turnaround_time = time + 1 - processes[highest_priority].arrival_time;
                    processes[highest_priority].waiting_time = processes[highest_priority].turnaround_time - processes[highest_priority].burst_time;
                }
            }
            time++;
        }
        else
        {
            time++;
        }

        handle_io(processes, n, time);
    }

    evaluate_performance(processes, n, "Priority");
}

/* rr
 */
void round_robin(Process processes[], int n)
{
    int time = 0, completed = 0, current = 0, time_slice_remaining = TIME_SLICE;

    while (completed < n)
    {
        if (processes[current].arrival_time <= time && processes[current].remaining_time > 0 && !processes[current].in_io)
        {
            if (processes[current].io_interrupt == time)
            {
                processes[current].in_io = 1;
                processes[current].io_waiting_time = IO_DURATION;
                time_slice_remaining = TIME_SLICE;
            }
            else
            {
                processes[current].remaining_time--;
                time_slice_remaining--;

                if (time_slice_remaining == 0)
                {
                    current = (current + 1) % n;
                    time_slice_remaining = TIME_SLICE;
                }

                if (processes[current].remaining_time == 0)
                {
                    completed++;
                    processes[current].turnaround_time = time + 1 - processes[current].arrival_time;
                    processes[current].waiting_time = processes[current].turnaround_time - processes[current].burst_time;
                }
            }
            time++;
        }
        else
        {
            time++;
            current = (current + 1) % n;
        }

        handle_io(processes, n, time);
    }

    evaluate_performance(processes, n, "Round Robin");
}

/* preemptive_sjf scheduling
 */
void preemptive_sjf(Process processes[], int n)
{
    int time = 0, completed = 0, shortest = -1;

    while (completed < n)
    {
        shortest = -1;
        for (int i = 0; i < n; i++)
        {
            if (processes[i].arrival_time <= time && processes[i].remaining_time > 0 && !processes[i].in_io)
            {
                if (shortest == -1 || processes[i].remaining_time < processes[shortest].remaining_time)
                {
                    shortest = i;
                }
            }
        }

        if (shortest != -1)
        {
            processes[shortest].remaining_time--;
            if (processes[shortest].remaining_time == 0)
            {
                completed++;
                processes[shortest].turnaround_time = time + 1 - processes[shortest].arrival_time;
                processes[shortest].waiting_time = processes[shortest].turnaround_time - processes[shortest].burst_time;
            }
        }
        time++;

        handle_io(processes, n, time);
    }

    evaluate_performance(processes, n, "Preemptive SJF");
}

/* preemptive priority scheduling
 */

void preemptive_priority(Process processes[], int n)
{
    int time = 0, completed = 0, highest_priority = -1;

    while (completed < n)
    {
        highest_priority = -1;
        for (int i = 0; i < n; i++)
        {
            if (processes[i].arrival_time <= time && processes[i].remaining_time > 0 && !processes[i].in_io)
            {
                if (highest_priority == -1 || processes[i].priority < processes[highest_priority].priority)
                {
                    highest_priority = i;
                }
            }
        }

        if (highest_priority != -1)
        {
            processes[highest_priority].remaining_time--;
            if (processes[highest_priority].remaining_time == 0)
            {
                completed++;
                processes[highest_priority].turnaround_time = time + 1 - processes[highest_priority].arrival_time;
                processes[highest_priority].waiting_time = processes[highest_priority].turnaround_time - processes[highest_priority].burst_time;
            }
        }
        time++;

        handle_io(processes, n, time);
    }

    evaluate_performance(processes, n, "Preemptive Priority");
}