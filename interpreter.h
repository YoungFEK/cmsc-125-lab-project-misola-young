#ifndef INTERPRETER_H
#define INTERPRETER_H
#include "command.h"

int execute_command (Command cmd_info);
void reap_zombies();

#endif