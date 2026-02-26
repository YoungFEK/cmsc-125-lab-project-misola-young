//main.c
#include <stdio.h>
#include "parser.h"
#include "command.h"
#include "interpreter.h"
#include <stdlib.h> 
#include <string.h>

int main(int argc, char *argv[]) {

    Command statement;
    char user_input[256];
    int user_input_index =0;

    if (argc < 2) { 
        while(1){
            reap_zombies();
            printf("mysh> ");

            // While the current character is NOT null
            fgets(user_input, sizeof(user_input), stdin);
            statement = parse(user_input);

            if(statement.command == NULL){
                continue;
            }
            execute_command(statement);

        }
    }else{
        FILE *file = fopen(argv[1], "r");
        if (!file) {
            perror("Error opening file");
            return 1;
        }

        char line[256];
        while (fgets(user_input, sizeof(line), file)) {
            reap_zombies();
            printf("mysh> %s", user_input);
            statement = parse(user_input);

            if(statement.command == NULL){
                continue;
            }

            execute_command(statement);

        }

        fclose(file);
    }

    
    // free_cmd_struct(statement);
    
    return 0;
}