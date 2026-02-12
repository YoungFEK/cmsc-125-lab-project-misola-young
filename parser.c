// parser.c
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include "command.h"
#include <stdbool.h>

// #include "parser.h"

Command parse(char *tokens[]) {

    Command statement;
    statement.input_file = NULL;
    statement.output_file = NULL;
    statement.append = false;
    statement.background = false;

    if (tokens[0] == NULL) {
        return statement;
    }

    else if (strcmp(tokens[0], "exit") == 0) {
        // printf("exit command\n");
        statement.command = tokens[0];
        statement.args[0] = tokens[0];
        statement.args[1] = NULL;
        return statement;
    }

    else if (strcmp(tokens[0], "cd") == 0) {
        // if (tokens[1] != NULL) {
        //     chdir(tokens[1]);
        // } else {
        //     printf("cd: missing argument\n");
        // }
        printf("cd command\n");
        return statement;
    }

    else if (strcmp(tokens[0], "pwd") == 0) {
        // char cwd[1024];
        // getcwd(cwd, sizeof(cwd));
        // printf("%s\n", cwd);
        //  printf("Input redirection\n");
        // printf("pwd command\n");

        if(tokens[0] != "\0") {
            statement.command = tokens[0];
            statement.args[0] = tokens[0];
            statement.args[1] = NULL;
            // statment.input_file = NULL;
            // statement.output_file = NULL;
            // statement.append = false;
            // statement.background = false;
        }


        return statement;
    }

    else {
        printf("External command: %s\n", tokens[0]);
    }
}
