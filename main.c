#include <stdio.h>
#include <unistd.h>

typedef struct {
    char *command;        // Command name
    char *args[256];      // Arguments (NULL-terminated)
    char *input_file;     // For < redirection (NULL if none)
    char *output_file;    // For > or >> redirection (NULL if none)
    bool append;          // true for >>, false for >
    bool background;      // true if & present
} Command;


int main() {
    //create a struct object/instance out of user input
    struct Command cmd;

    
}