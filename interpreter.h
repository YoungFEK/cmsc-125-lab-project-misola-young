#ifndef INTERPRETER_H
#define INTERPRETER_H
#include "command.h"

int execute_command (Command cmd_info);
void reap_zombies();
void free_cmd_struct(Command cmd_info);

#endif