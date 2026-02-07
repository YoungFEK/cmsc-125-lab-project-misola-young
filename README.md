# CMSC 125 | UNIX Shell (Lab 1)

## Contributors
Mark Leonel T. Misola (Xenon5443)\
Francis Eugene Kho Young (YoungFEK)

> [!NOTE]
> This project is still in development!

## POSIX API
POSIX API is utilized througught the project. This inludes the following functions:

| Function | Description |
| --- | --- |
| fork | Makes a child process. Returns a PID of a child to the parent, and returns 0 to the child |
| execvp | Used for executing program(binary executable / linux utility program) and replacing the caller, therefore, a child should be spawned before calling this function.|
| wait | Forces parent to wait for the child process to finish before resuming execution|
| waitpid | Similar to wait() but you get to choose which child to wait for |
| dup2 | Creates an alternative identifies for i/o resource or open files |
| open | Opens a specified file by path |
| close | Close a file |
| chdir | Change directory |
| getcwd | Get current working directory to be used to implement pwd |

## Solution Architecture
### 1. myshell.c
The main file that calls the other parts of the code.

### 2. lexer.c
Returns a list of token structure variable from an input string.\
Utilizes strtok() to separate input string by white space.\
A token structure will be utilized to differentiate different token types.

### 3. parser.c
Return a command structure variable from a list of token structure variables to be executed.\
<details>
<summary>Suggested command structure</summary>
  
```
typedef struct {
    char *command;        // Command name
    char *args[256];      // Arguments (NULL-terminated)
    char *input_file;     // For < redirection (NULL if none)
    char *output_file;    // For > or >> redirection (NULL if none)
    bool append;          // true for >>, false for >
    bool background;      // true if & present
} Command;
```
</details>

      
### 4. interpreter.c
Uses execvp(), basic execution pattern and standard C functions to execute code from command structure variable.\
<details>
<summary>Suggested command execution structure</summary>
  
```
pid_t pid = fork();

if (pid < 0) {
    perror("fork failed");
    return -1;
}

if (pid == 0) {  // Child process
    // Apply redirections if needed
    if (cmd.input_file) {
        int fd = open(cmd.input_file, O_RDONLY);
        if (fd < 0) {
            perror("open input file");
            exit(1);
        }
        dup2(fd, STDIN_FILENO);
        close(fd);
    }
    
    if (cmd.output_file) {
        int flags = O_WRONLY | O_CREAT;
        flags |= cmd.append ? O_APPEND : O_TRUNC;
        int fd = open(cmd.output_file, flags, 0644);
        if (fd < 0) {
            perror("open output file");
            exit(1);
        }
        dup2(fd, STDOUT_FILENO);
        close(fd);
    }
    
    // Execute command
    execvp(cmd.command, cmd.args);
    perror("exec failed");
    exit(127);
} else {  // Parent process
    if (!cmd.background) {
        int status;
        waitpid(pid, &status, 0);
        if (WIFEXITED(status)) {
            int exit_code = WEXITSTATUS(status);
            if (exit_code != 0) {
                printf("Command exited with code %d\n", exit_code);
            }
        }
    } else {
        printf("[%d] Started: %s (PID: %d)\n", job_id, cmd_str, pid);
        // Add to background job list
    }
}
```
</details>

## Timeline
| Week Number | Objective |
|---|---|
| Week 1 | Learn concepts essential to implement the project such as Unix system calls like fork(), wait() etc.. Create rough design of project structure|
| Week 2 | Finish working code for lexer, parser, and interpreter prototypes |
| Week 3 | Ideally finish code for interpreter and test |
| Week 4 | Refactors and final defense |




