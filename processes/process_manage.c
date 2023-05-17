#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <unistd.h>
#include <sys/wait.h>
#include <signal.h>

#define MAX_PROCESS 10

typedef struct {
    pid_t pid;
    char cmd[256];
} ProcessControlBlock;

void create_process(ProcessControlBlock pcb[], int *pcb_count) {
    if (*pcb_count >= MAX_PROCESS) {
        printf("Process creation failed. Process control block is full.\n");
        return;
    }

    char cmd[256];
    printf("Enter process command: ");
    scanf("%s", cmd);

    int pid = fork();
    if (pid < 0) {
        printf("Failed to fork a new process.\n");
        return;
    } else if (pid == 0) {
        // Child process
        execlp(cmd, cmd, NULL);
        perror("Failed to start process");
        exit(EXIT_FAILURE);
    } else {
        // Parent process
        pcb[*pcb_count].pid = pid;
        strcpy(pcb[*pcb_count].cmd, cmd);
        (*pcb_count)++;
        printf("\nProcess started with PID %d\n", pid);
    }
}

void kill_process(ProcessControlBlock pcb[], int *pcb_count) {
    int pid;
    printf("Enter PID of process to kill: ");
    scanf("%d", &pid);

    for (int i = 0; i < *pcb_count; i++) {
        if (pcb[i].pid == pid) {
            if (kill(pid, SIGKILL) < 0) {
                printf("Failed to kill process with PID %d\n", pid);
                return;
            }
            printf("Process with PID %d killed\n", pid);
            (*pcb_count)--;
            for (int j = i; j < *pcb_count; j++) {
                pcb[j] = pcb[j + 1];
            }
            return;
        }
    }
    printf("No process found with PID %d\n", pid);
}

void display_pcb(ProcessControlBlock pcb[], int pcb_count) {
    printf("PID\tCommand\n");
    for (int i = 0; i < pcb_count; i++) {
        printf("%d\t%s\n", pcb[i].pid, pcb[i].cmd);
    }
}

int main(int argc, char* argv[]) {
    ProcessControlBlock pcb[MAX_PROCESS];
    int pcb_count = 0;

    while (true) {
        printf("Please input new command: ");
        char cmd[256];
        scanf("%s", cmd);

        if (strcmp(cmd, "create") == 0) {
            create_process(pcb, &pcb_count);
        } else if (strcmp(cmd, "kill") == 0) {
            kill_process(pcb, &pcb_count);
        } else if (strcmp(cmd, "pcb") == 0) {
            display_pcb(pcb, pcb_count);
        } else if (strcmp(cmd, "exit") == 0) {
            break;
        } else {
            printf("Invalid command. Valid commands are 'create', 'kill', 'pcb', and 'exit'\n");
        }
    }

    return 0;
}
