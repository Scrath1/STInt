# STInt - A Simple Terminal Interface library
STInt is a library for parsing line-based user input to execute custom commands.

The [Basic Example](examples/basic-example.cpp) showcases how to write
a simple console application using this library

## Installation
To use this library, simple add the contents of the `src` and `include` folders to
your project add `#include "stint.h` to your source file.

## Usage
To begin with using STInt you will need to define the following:
1.  The functions you want to execute as commands.
    These must have the signature `void function_name(const char* cmd, uint32_t cmd_len)`.
    The parameters `cmd` and `cmd_len` are used to pass along any additional user
    input beside the command name.
2.  An array of Stint::Command objects which act as your table of commands.
    While you have to define the command name and function, the helptext is optional and not used
    by the library itself.
3.  A char array to use as an input buffer: `char buffer[<Maximum line length for user input>]`
4.  The actual Stint object `Stint stint{commands, sizeof(commands),buffer, sizeof(buffer)}`

A minimal example might look like this:
```C++
void cmd_help(const char* cmd, uint32_t cmd_len);

Stint::Command commands[] = {
    {.name = "help", .function = cmd_help, .helptext = "Prints all known commands and their helptext"},
};
char buffer[128];
Stint stint{commands, std::size(commands), buf, sizeof(buf)};

void cmd_help(const char* cmd, uint32_t cmd_len){
    // do something like printing out your list of commands
}
```