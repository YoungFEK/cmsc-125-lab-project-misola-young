// parser.c
// reminder free memory using free(), with a copy, u have indepence, safer code, 
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include "command.h"
#include <stdbool.h>
#include "parser.h"
#include <stdlib.h> 

// edit this to add the command typed(choose the safest method once the process is complete)
void print_command_not_found(){
    printf("command not found\n");
}

void print_unexpected_token(){
    printf("unexpected token\n");
}

void print_too_many_args(){
    printf("too many arguments\n");
}

void parse_exit_cmd(char *tokens[], Command *statement){

     if(tokens[2] != NULL){
        statement->command = NULL;
        print_too_many_args();
        return;
    }

    statement->command = strdup(tokens[0]);

    if( (tokens[1] != NULL) ){
        if( (strcmp(tokens[1], "1") == 0) || (strcmp(tokens[1], "0") == 0)){
            statement->args[0] = strdup(tokens[1]);
            return;
        }
        //string coming after exit is neither 0 or 1
        print_unexpected_token();
        statement->command = NULL;
    }

}

void parse_cd_cmd(char *tokens[], Command *statement){

    if(tokens[2] != NULL){
        statement->command = NULL;
        print_too_many_args();
        return;
    }

    statement->command = strdup(tokens[0]);

    if( (tokens[1] != NULL) ){
        //to be decided: whether or not this should be removed, and checking will be left chdir()
        if(access(tokens[1], X_OK) != 0){
            statement->command = NULL;
            perror("cd");
        }else{
            statement->args[0] = strdup(tokens[0]);
            statement->args[1] = strdup(tokens[1]);
        }
    }
}


void parse_pwd_cmd(char *tokens[], Command *statement){
    if( (tokens[1] != NULL) ){
        statement->command = NULL;
        print_unexpected_token();
        return;
    }

    statement->command = strdup(tokens[0]);
    statement->args[0] = strdup(tokens[0]);
    statement->args[1] = NULL;
}


void parse_input_output_cmd(char *tokens[], Command *statement){
    int token_index = 0;

    statement->command = strdup(tokens[0]);

    // initialize args with command name and other arguments until an operator or end of line is encountered
    while ((tokens[token_index] != NULL) &&
        (strcmp(tokens[token_index], "<") != 0) && 
        (strcmp(tokens[token_index], ">") != 0) && 
        (strcmp(tokens[token_index], ">>") != 0)  && 
        (strcmp(tokens[token_index], "&") != 0)){

        statement->args[token_index] = strdup(tokens[token_index]);
        token_index++;
    }

    while (tokens[token_index] != NULL){
        //check if token matches <
        if(strcmp(tokens[token_index], "<") == 0){

            //check if input file already exists
            if(statement->input_file != NULL){
                print_unexpected_token();
                statement->command = NULL;
                return;
            }
            
            //move to the next token to check if file exists
            token_index++; 
            if(tokens[token_index] == NULL){
                printf("missing file\n");
                statement->command = NULL;
                return;
            }
            
            //assign to input or output file(separate function)
            statement->input_file = strdup(tokens[token_index]); 
        }
        //check if token matches >
        else if(strcmp(tokens[token_index], ">") == 0){

            //check if output file already exists
            if(statement->output_file != NULL){
                print_unexpected_token();
                statement->command = NULL;
                return;
            }

            //move to the next token to check if file exists
            token_index++; 
            if(tokens[token_index] == NULL){
                printf("missing file\n");
                statement->command = NULL;
                return;
            }

            //assign to input or output file(separate function)
            statement->output_file = strdup(tokens[token_index]); 

        }
        //check if token matches >>
        else if(strcmp(tokens[token_index], ">>") == 0){
            //check if output file already exists
            if(statement->output_file != NULL){
                print_unexpected_token();
                statement->command = NULL;
                return;
            }

            //move to the next token to check if file exists
            token_index++; 
            if(tokens[token_index] == NULL){
                printf("missing file\n");
                statement->command = NULL;
                return;
            }

            //assign to input or output file(separate function)
            statement->output_file = strdup(tokens[token_index]); 
            statement->append = true;

        }
        //check if token matches &
        else if(strcmp(tokens[token_index], "&") == 0){
            if(tokens[token_index+1] != NULL){
                statement->command = NULL;
                print_too_many_args();
                return;
            }else{
                statement->background = true;
                return;
            }

        }
        //any token not matching the specified operators above is invalid
        else{
            statement->command = NULL;
            print_unexpected_token();
            return;
        }


        
        token_index++;
    }
}



void parse_sleep_cmd(char *tokens[], Command *statement){

     if( (tokens[2] != NULL)){
        if(strcmp(tokens[2], "&") == 0){
            statement->background = true;
        }else{
            statement->command = NULL;
            print_unexpected_token();
            return;
        }
    }

    statement->command = strdup(tokens[0]);


    if( (tokens[1] != NULL) ){
        statement->args[0] = strdup(tokens[0]);
        statement->args[1] = strdup(tokens[1]);
        return;

    }else{
        statement->command = NULL;
        print_unexpected_token();
        return;
    }   
}

char *remove_quotes(const char *str) {
    int len = strlen(str);

    if (len == 0)
        return strdup(str);

    int start = 0;
    int end = len;

    // Remove leading quote
    if (str[0] == '"') {
        start = 1;
    }

    // Remove trailing quote
    if (len > 1 && str[len - 1] == '"') {
        end = len - 1;
    }

    int new_len = end - start;

    char *result = malloc(new_len + 1);
    if (!result)
        return NULL;

    strncpy(result, str + start, new_len);
    result[new_len] = '\0';

    return result;
}


void parse_echo_cmd(char *tokens[], Command *statement){
    int token_index = 0;

    statement->command = strdup(tokens[0]);

    // initialize args with command name and other arguments until an operator or end of line is encountered
    while ((tokens[token_index] != NULL) &&
        (strcmp(tokens[token_index], "<") != 0) && 
        (strcmp(tokens[token_index], ">") != 0) && 
        (strcmp(tokens[token_index], ">>") != 0)  && 
        (strcmp(tokens[token_index], "&") != 0)){

    


        

        // statement->args[token_index] = strdup(tokens[token_index]);
        statement->args[token_index] = remove_quotes(tokens[token_index]);
        token_index++;
    }

    while (tokens[token_index] != NULL){
        //check if token matches <
        if(strcmp(tokens[token_index], "<") == 0){

            //check if input file already exists
            if(statement->input_file != NULL){
                print_unexpected_token();
                statement->command = NULL;
                return;
            }
            
            //move to the next token to check if file exists
            token_index++; 
            if(tokens[token_index] == NULL){
                printf("missing file\n");
                statement->command = NULL;
                return;
            }
            
            //assign to input or output file(separate function)
            statement->input_file = strdup(tokens[token_index]); 
        }
        //check if token matches >
        else if(strcmp(tokens[token_index], ">") == 0){

            //check if output file already exists
            if(statement->output_file != NULL){
                print_unexpected_token();
                statement->command = NULL;
                return;
            }

            //move to the next token to check if file exists
            token_index++; 
            if(tokens[token_index] == NULL){
                printf("missing file\n");
                statement->command = NULL;
                return;
            }

            //assign to input or output file(separate function)
            statement->output_file = strdup(tokens[token_index]); 

        }
        //check if token matches >>
        else if(strcmp(tokens[token_index], ">>") == 0){
            //check if output file already exists
            if(statement->output_file != NULL){
                print_unexpected_token();
                statement->command = NULL;
                return;
            }

            //move to the next token to check if file exists
            token_index++; 
            if(tokens[token_index] == NULL){
                printf("missing file\n");
                statement->command = NULL;
                return;
            }

            //assign to input or output file(separate function)
            statement->output_file = strdup(tokens[token_index]); 
            statement->append = true;

        }
        //check if token matches &
        else if(strcmp(tokens[token_index], "&") == 0){
            if(tokens[token_index+1] != NULL){
                statement->command = NULL;
                print_too_many_args();
                return;
            }else{
                statement->background = true;
                return;
            }

        }
        //any token not matching the specified operators above is invalid
        else{
            statement->command = NULL;
            print_unexpected_token();
            return;
        }


        
        token_index++;
    }
}


Command parse(char *user_input) {

    // to prevent unpredictable behavior when checking for excess tokens
    char *tokens[100] = {0}; 
    int user_input_index1 = 0;

    Command statement = {0};  


    user_input[strcspn(user_input, "\n")] = '\0';

    char *token = strtok(user_input, " ");

    while (token != NULL) {
        tokens[user_input_index1++] = strdup(token); // encounter problems with printing if strdup is not added here
        token = strtok(NULL, " ");
    }

    tokens[user_input_index1] = NULL;
    // tokens[user_input_index1] = strdup(token);

    //for checking contents of tokens which will help in debugging
    // for(int i=0; i<100 ; i++){
    //     printf("%s, ", tokens[i]);
    // }
    // printf("\n");
    

    // user_input is empty, tokens only has null element
    if (tokens[0] == NULL) { 
        print_command_not_found();
    }
    else if (strcmp(tokens[0], "exit") == 0) {
        parse_exit_cmd(tokens, &statement);
    }

    else if (strcmp(tokens[0], "cd") == 0) {
        parse_cd_cmd(tokens, &statement);
    }

    else if (strcmp(tokens[0], "pwd") == 0) {
        parse_pwd_cmd(tokens, &statement);
    }

    else if (strcmp(tokens[0], "ls") == 0) {
        parse_input_output_cmd(tokens, &statement);
    }

    else if (strcmp(tokens[0], "wc") == 0) {
        parse_input_output_cmd(tokens, &statement);
    }

    else if (strcmp(tokens[0], "sort") == 0) {
        parse_input_output_cmd(tokens, &statement); 
    }

    else if (strcmp(tokens[0], "echo") == 0) {
        parse_input_output_cmd(tokens, &statement); 
    }

    else if (strcmp(tokens[0], "cat") == 0) {
        parse_input_output_cmd(tokens, &statement);
    }

    else if (strcmp(tokens[0], "sleep") == 0) {
        parse_sleep_cmd(tokens, &statement); 
    }

    else {
        print_command_not_found();
    }

    for(int j = 0; tokens[j] != NULL; j++) {
        free(tokens[j]);
    }

    return statement;

}




// for (int i = 0; i < user_input_index1; i++) {
    //     printf("Token %d: %s\n", i + 1, tokens[i]);
    // }
