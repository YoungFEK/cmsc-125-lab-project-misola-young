#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdbool.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <errno.h>
#include "command.h"

//Job details struct
typedef struct {
    int job_id;
    char cmd_str[256];
    pid_t pid;
} JobDetails;

static JobDetails active_job_list[99];
static int total_bg_job;

int execute_command (Command cmd_info){
    //If cmd is exit then kill all orphan processes then use exit command
    if (!strcmp(cmd_info.command, "exit")){
        exit(0);
    
    //If command is cd 
    } else if (!strcmp(cmd_info.command, "cd")){
        if (chdir(cmd_info.args[1]) < 0){
            printf("cd %s : %s", cmd_info.args[1], strerror(errno));
            return 0;
        }
        return 0;
    
    //If command is pwd
    } else if (!strcmp(cmd_info.command, "pwd")) {
        char curr_dir[99];

        if (getcwd(curr_dir, sizeof(curr_dir)) != NULL) {
            printf("%s\n", curr_dir);
            return 0;
        } else {
            perror("Error pwd");
        }
    }

    //Extrenal Commands
    pid_t pid = fork();

    if (pid < 0) {
        perror("fork failed");
        return -1;
    }

    if (pid == 0) {  // Child process
        //From file to the shell
        if (cmd_info.input_file) {
            int file_code = open(cmd_info.input_file, O_RDONLY);
            if (file_code < 0) {
                perror("open input file");
                exit(1);
            }
            dup2(file_code, STDIN_FILENO);
            close(file_code);
        }
        
        //From shell to the file
        if (cmd_info.output_file) {
            int flags = O_WRONLY | O_CREAT;
            if (cmd_info.append){
                flags |= O_APPEND;
            } else{
                flags |= O_TRUNC;
            }

            int file_code = open(cmd_info.output_file, flags, 0644);

            if (file_code < 0) {
                perror("open output file");
                exit(1);
            }

            dup2(file_code, STDOUT_FILENO);
            close(file_code);
        }
        
        // Execute command
        execvp(cmd_info.command, cmd_info.args);
        perror("exec failed");
        exit(127);

    } else {  // Parent process
        if (!cmd_info.background) {
            int status;
            waitpid(pid, &status, 0);
            if (WIFEXITED(status)) {
                int exit_code = WEXITSTATUS(status);
                
                if (exit_code != 0) {
                    printf("Command exited with code %d\n", exit_code);
                    perror("Error in exiting command");
                }
            }

        } else {
            JobDetails curr_job;

            if(total_bg_job == 0){
                curr_job.job_id = 1;
            } else {
                curr_job.job_id = active_job_list[total_bg_job - 1].job_id + 1;
            }
            
            snprintf(curr_job.cmd_str, sizeof(curr_job.cmd_str), "%s", cmd_info.command);
            curr_job.pid = pid;

            active_job_list[total_bg_job] = curr_job;
            total_bg_job++;

            printf("[%d] Started: %s (PID: %d)\n", curr_job.job_id, curr_job.cmd_str, curr_job.pid);
        }
    } 
}

void reap_zombies(){
    //Iterate through the jobs in the list and update the number of jobs when being reaped 
    //To be called after execvp and before mysh prompt
    int status = 0;
    pid_t termianted_pid = 0;

    for(int i = 0; i < total_bg_job ; i++){
        termianted_pid = waitpid(active_job_list[i].pid, &status, WNOHANG);

        if (termianted_pid != 0 ){
            if (termianted_pid == -1){
                perror("Background termination error");
            }

            //Print "done" with the job details that will be terminated
            printf("[%d] done %s", active_job_list[i].job_id, active_job_list[i].cmd_str);

            //Remove the terminated job from list
            for(int j = i; j < total_bg_job - 1; j++){
                active_job_list[j] = active_job_list[j + 1];
            }
            
            total_bg_job--;
            i--;
        }
    }
}