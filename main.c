//main.c
#include <stdio.h>
#include "parser.h"
#include "command.h"
#include <stdlib.h> 

int main() {

    Command statement;
    statement = parse();

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
    

    if(statement.command != NULL){
        free(statement.command);
        int args_index = 0;
        while(statement.args[args_index] != NULL){
            free(statement.args[args_index]);
            args_index++;
        }

        if(statement.input_file != NULL){
            free(statement.input_file);
        }

        if(statement.output_file != NULL){
            free(statement.output_file );
        }
    }
    
    
    return 0;
}
