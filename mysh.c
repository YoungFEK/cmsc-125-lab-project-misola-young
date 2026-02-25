//main.c
#include <stdio.h>
#include "parser.h"
#include "command.h"
#include "interpreter.h"
#include <stdlib.h> 
#include <string.h>

void clearUntilNull(char *str) {
    if (str == NULL) return; // Safety check

    while (*str != '\0') {
        *str = '\0';
        str++;
    }
}

int main(int argc, char *argv[]) {

    Command statement;
    char user_input[256];
    int user_input_index =0;
    

    printf("output:\n");

    if (argc < 2) { 
        while(1){
            
            printf("mysh> ");

            // While the current character is NOT null

            
            
            fgets(user_input, sizeof(user_input), stdin);

            statement = parse(user_input);
            
            // execute_command(statement);
            int args_index0 = 0;
            if(statement.command != NULL){
                printf("command: %s\n", statement.command);
                printf("args:\n");
                while(statement.args[args_index0] != NULL){
                    printf("%s\n", statement.args[args_index0]);
                    args_index0++;
                }
                if(statement.input_file != NULL){
                    printf("append: input_file true\n");
                }else{
                    printf("append: input_file false\n");
                }
                if(statement.output_file != NULL){
                    printf("append: output_file true\n");
                }else{
                    printf("append: output_file false\n");
                }
                if(statement.append == true){
                    printf("append: true\n");
                }else{
                    printf("append: false\n");
                }
                if(statement.background == true){
                    printf("background: true\n");
                }else{
                    printf("background: false\n");
                }
                
            }
    


        }
    }else{
        FILE *file = fopen(argv[1], "r");
        if (!file) {
            perror("Error opening file");
            return 1;
        }

        char line[256];
        while (fgets(user_input, sizeof(line), file)) {
            printf("mysh> %s", user_input);

            statement = parse(user_input);
            
            // execute_command(statement);
            int args_index0 = 0;
            if(statement.command != NULL){
                printf("command: %s\n", statement.command);
                printf("args:\n");
                while(statement.args[args_index0] != NULL){
                    printf("%s\n", statement.args[args_index0]);
                    args_index0++;
                }
                if(statement.input_file != NULL){
                    printf("append: input_file true\n");
                }else{
                    printf("append: input_file false\n");
                }
                if(statement.output_file != NULL){
                    printf("append: output_file true\n");
                }else{
                    printf("append: output_file false\n");
                }
                if(statement.append == true){
                    printf("append: true\n");
                }else{
                    printf("append: false\n");
                }
                if(statement.background == true){
                    printf("background: true\n");
                }else{
                    printf("background: false\n");
                }
                
            }

            printf("\n\n");
            

        }

        fclose(file);
    }

    


    

    // free_cmd_struct(statement);
    


    
    return 0;
}
