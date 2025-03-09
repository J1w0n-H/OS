#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

// Structure to represent a task
typedef struct {
    int id;           // Task ID
    int execution;    // Execution time (e)
    int period;       // Period (p)
    int deadline;     // Relative deadline (D)
} Task;

// Structure to represent a job instance
typedef struct {
    int task_id;      // ID of the task this job belongs to
    int release;      // Release time
    int execution;    // Execution time remaining
    int abs_deadline; // Absolute deadline
    bool completed;   // Whether the job has completed
} Job;

// Function to check schedulability using utilization bound test
bool check_schedulability_utilization(Task* tasks, int n) {
    float utilization = 0.0;
    
    for (int i = 0; i < n; i++) {
        utilization += (float)tasks[i].execution / (float)tasks[i].deadline;
    }
    
    printf("Total utilization: %.2f\n", utilization);
    // For EDF, the system is schedulable if utilization <= 1.0
    return utilization <= 1.0;
}

// Function to find the job with the earliest deadline
int find_earliest_deadline(Job* jobs, int n) {
    int earliest = -1;
    int min_deadline = __INT_MAX__;
    
    for (int i = 0; i < n; i++) {
        if (!jobs[i].completed && jobs[i].execution > 0 && jobs[i].abs_deadline < min_deadline) {
            min_deadline = jobs[i].abs_deadline;
            earliest = i;
        }
    }
    
    return earliest;
}

// Function to release new job instances at their periods
void release_jobs(Task* tasks, Job* jobs, int n_tasks, int* n_jobs, int time) {
    for (int i = 0; i < n_tasks; i++) {
        if (time % tasks[i].period == 0) {
            // Create a new job for this task
            jobs[*n_jobs].task_id = tasks[i].id;
            jobs[*n_jobs].release = time;
            jobs[*n_jobs].execution = tasks[i].execution;
            jobs[*n_jobs].abs_deadline = time + tasks[i].deadline;
            jobs[*n_jobs].completed = false;
            (*n_jobs)++;
        }
    }
}

// Function to construct EDF schedule and check for deadline misses
bool construct_edf_schedule(Task* tasks, int n, int simulation_time) {
    // Maximum number of jobs that could be released during simulation
    int max_jobs = 0;
    for (int i = 0; i < n; i++) {
        max_jobs += (simulation_time / tasks[i].period) + 1;
    }
    
    // Array to store all job instances
    Job* jobs = (Job*)malloc(max_jobs * sizeof(Job));
    int n_jobs = 0;
    
    // Array to track which task is running at each time unit
    int* schedule = (int*)malloc(simulation_time * sizeof(int));
    for (int i = 0; i < simulation_time; i++) {
        schedule[i] = -1; // -1 means idle
    }
    
    // Simulate EDF scheduling
    bool deadline_missed = false;
    
    for (int time = 0; time < simulation_time; time++) {
        // Release new jobs at their periods
        release_jobs(tasks, jobs, n, &n_jobs, time);
        
        // Find the job with the earliest deadline
        int job_idx = find_earliest_deadline(jobs, n_jobs);
        
        if (job_idx != -1) {
            // Execute the selected job
            jobs[job_idx].execution--;
            schedule[time] = jobs[job_idx].task_id;
            
            // Check if job completed
            if (jobs[job_idx].execution == 0) {
                jobs[job_idx].completed = true;
            }
        }
        
        // Check for deadline misses
        for (int i = 0; i < n_jobs; i++) {
            if (!jobs[i].completed && time >= jobs[i].abs_deadline) {
                printf("Deadline missed for task %d at time %d!\n", 
                       jobs[i].task_id, time);
                deadline_missed = true;
            }
        }
    }
    
    // Print the scheduling diagram
    printf("\nEDF Schedule for %d time units:\n", simulation_time);
    printf("Time: ");
    for (int t = 0; t < simulation_time; t++) {
        printf("%2d ", t);
    }
    printf("\n");
    
    printf("Task: ");
    for (int t = 0; t < simulation_time; t++) {
        if (schedule[t] == -1) {
            printf(" - ");
        } else {
            printf("τ%d ", schedule[t]);
        }
    }
    printf("\n\n");
    
    // Print task-specific timing diagrams
    for (int i = 0; i < n; i++) {
        printf("τ%d: ", tasks[i].id);
        for (int t = 0; t < simulation_time; t++) {
            if (schedule[t] == tasks[i].id) {
                printf("█ "); // Running
            } else if (t % tasks[i].period == 0) {
                printf("↓ "); // Release point
            } else if ((t + 1) % tasks[i].period == 0 || 
                      (t + 1 - (t / tasks[i].period) * tasks[i].period) == tasks[i].deadline) {
                printf("! "); // Deadline point
            } else {
                printf("  "); // Idle
            }
        }
        printf("\n");
    }
    
    // Free allocated memory
    free(jobs);
    free(schedule);
    
    return !deadline_missed;
}

int main() {
    // Define the task set: τi ≡ {ei, pi, Di}
    // τ1 ≡ {1, 5, 4}, τ2 ≡ {2, 8, 6}, and τ3 ≡ {1, 4, 3}
    Task tasks[3] = {
        {1, 1, 5, 4}, // Task 1: execution=1, period=5, deadline=4
        {2, 2, 8, 6}, // Task 2: execution=2, period=8, deadline=6
        {3, 1, 4, 3}  // Task 3: execution=1, period=4, deadline=3
    };
    
    int n_tasks = 3;
    int simulation_time = 20;
    
    printf("Task Set:\n");
    printf("Task ID | Execution | Period | Deadline\n");
    printf("--------|-----------|--------|----------\n");
    for (int i = 0; i < n_tasks; i++) {
        printf("τ%d      | %9d | %6d | %8d\n", 
               tasks[i].id, tasks[i].execution, tasks[i].period, tasks[i].deadline);
    }
    printf("\n");
    
    // Check schedulability using utilization bound
    printf("Checking schedulability using utilization bound test...\n");
    bool schedulable_by_utilization = check_schedulability_utilization(tasks, n_tasks);
    printf("Schedulable by utilization bound test: %s\n\n", 
           schedulable_by_utilization ? "YES" : "NO");
    
    // Construct EDF schedule
    printf("Constructing EDF schedule...\n");
    bool schedulable_by_simulation = construct_edf_schedule(tasks, n_tasks, simulation_time);
    printf("Schedulable by simulation: %s\n", 
           schedulable_by_simulation ? "YES" : "NO");
    
    return 0;
}
