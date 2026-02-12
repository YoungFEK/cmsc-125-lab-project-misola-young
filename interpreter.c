#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdbool.h>
#include <stdlib.h>
#include <sys/wait.h>

int execute_command (Command cmd_info){
    //DEBUG
    printf("Executing: %s\n", cmd_info.command);
    char curr_dir[99];
    getcwd(curr_dir, sizeof(curr_dir));
    printf("Current Directory: %s\n", curr_dir);

    if (!strcmp(cmd_info.command, "exit")){
        exit(0);
        //Kill orphan processes from background processes
        
    } else if (!strcmp(cmd_info.command, "cd")){
        //DEBUG
        printf("changed dir\n");

        if (chdir(cmd_info.args[1]) < 0){
            //DEBUG
            printf("CHANGEE: %s\n", cmd_info.args[1]);

            //Possible to add more error cases depending on value of errno variable
            perror("chdir() failed");
            return 0;
        }
        return 0;

    } else if (!strcmp(cmd_info.command, "pwd")) {
        char curr_dir[99];
        getcwd(curr_dir, sizeof(curr_dir));

        if (curr_dir != NULL) {
            printf("%s\n", curr_dir);
            return 0;
        } else {
            perror("Getting current directory error");
        }
    }

    pid_t pid = fork();

    if (pid < 0) {
        perror("fork failed");
        return -1;
    }

    if (pid == 0) {  // Child process
        // Apply redirections if needed
        if (cmd_info.input_file) {
            int file_code = open(cmd_info.input_file, O_RDONLY);
            if (file_code < 0) {
                perror("open input file");
                exit(1);
            }
            dup2(file_code, STDIN_FILENO);
            close(file_code);
        }
        
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
                
                //DEBUG
                if (exit_code != 0) {
                    printf("Command exited with code %d\n", exit_code);
                } else{
                    //Error in exiting
                }
            }
        } else {
            // printf("[%d] Started: %s (PID: %d)\n", job_id, cmd_str, pid);
            // Add to background job list
            //Check zombie processes before the main prompt. Probably palced at the bottom of the parent process
        }
    } 
}