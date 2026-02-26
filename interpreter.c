#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdbool.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <errno.h>
#include <signal.h>
#include "command.h"

void reap_zombies();

#define MAX_JOBS 99

//Job details struct
typedef struct {
    int job_id;
    char cmd_str[256];
    pid_t pid;
} JobDetails;

static JobDetails active_job_list[MAX_JOBS];
static int total_bg_job;
static int lowest_job_id = 1;

int execute_command (Command cmd_info){
    if (!strcmp(cmd_info.command, "exit")){
        int active_job_pid[total_bg_job];
        int total_active_bg = total_bg_job;
        int status = 1;
        
        //Kill remaining orphans for proper exit
        for(int curr_index = MAX_JOBS - 1; curr_index >= 0; curr_index--){
            if(active_job_list[curr_index].pid > 0){
                status = kill(active_job_list[curr_index].pid, SIGTERM);

                if (status < 0){
                    perror("Kill through termination signal failed");
                }

                active_job_pid[total_bg_job - total_active_bg] = active_job_list[curr_index].pid;
                total_active_bg--;
                if (total_active_bg <= 0){
                    break;
                }
            }
        }
        total_active_bg = total_bg_job;
        reap_zombies();

        //Catch orphans that ignored terminate signal
        if(total_bg_job > 0){
            for(int curr_index = 0; curr_index > total_active_bg; curr_index++){
                if(waitpid(active_job_list[curr_index].pid, &status, WNOHANG) == 0){
                    kill(active_job_list[curr_index].pid, SIGKILL);

                    if (status < 0){
                        perror("Kill through force kill signal failed");
                    }
                }
            }
            reap_zombies();
        }

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

            curr_job.job_id = lowest_job_id;
            snprintf(curr_job.cmd_str, sizeof(curr_job.cmd_str), "%s", cmd_info.command); //Use free() and replace with strdup, cuz need consistency
            curr_job.pid = pid;

            active_job_list[curr_job.job_id - 1] = curr_job;

            printf("[%d] Started: %s (PID: %d)\n", curr_job.job_id, curr_job.cmd_str, curr_job.pid);

            do{ //Find next free space, seems efficient for now - maybe use for loop again
                lowest_job_id++;
                if (lowest_job_id > MAX_JOBS){
                    perror("Exceeded maximum number of background jobs");
                }
            } while (active_job_list[lowest_job_id].pid != 0);
            total_bg_job++;
        }
    } 
}

void reap_zombies(){
    //Iterate through the jobs in the list and update the number of jobs when being reaped 
    int status = 0;

    for(pid_t termianted_pid = waitpid(-1, &status, WNOHANG); termianted_pid > 0; termianted_pid = waitpid(-1, &status, WNOHANG)){

        if (termianted_pid < 0 ){
            if (termianted_pid == -1 && errno != ECHILD){
                perror("Background termination error");
                break;
            }

        } else { //Remove terminated job by finding its index 
            int term_job_index = 0;
            for (; termianted_pid != active_job_list[term_job_index].pid ; term_job_index++){
                if (term_job_index >= MAX_JOBS){
                    perror("Missing job process inside list");
                }
            }

            //Print "done" with the job details to notify users - Add exit status
            printf("[%d] done %s \n", active_job_list[term_job_index].job_id, active_job_list[term_job_index].cmd_str);

            //Remove the terminated job from list
            active_job_list[term_job_index].pid = 0;
            //FREE the strings using young func

            term_job_index++;
            if(term_job_index < lowest_job_id){
                lowest_job_id = term_job_index;
            }
            total_bg_job--;
        }
    }
}