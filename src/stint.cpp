#include "stint.h"

#include <cstring>

// Default value to indicate a full command was entered and the input buffer contents should
// now be parsed
#define DEFAULT_CMD_DELIM '\n'
// The next character after a command code (name of the command) must have this value
#define CMD_CODE_DELIM ' '

Stint::Stint(const Command commands[], uint32_t commands_size, char input_buffer[], uint32_t input_buffer_size)
    : commands(commands),
      commands_size(commands_size),
      input_buffer_size(input_buffer_size),
      input_buffer(input_buffer) {
    next_buffer_idx = 0;
    command_delim = DEFAULT_CMD_DELIM;
}
Stint::ErrorCode Stint::ingest(char c) {
    if(c == command_delim) {
        // character signifies end of command. Parse and execute that command
        input_buffer[next_buffer_idx++] = '\0';
        return parse();
    } else {
        // add character to buffer and move on
        if(next_buffer_idx >= input_buffer_size - 1) return BUFFER_FULL;
        input_buffer[next_buffer_idx++] = c;
    }

    return SUCCESS;
}
void Stint::setCommandDelim(char c) {
    command_delim = c;
}
void Stint::clearBuffer() {
    next_buffer_idx = 0;
}

Stint::ErrorCode Stint::parse() {
    if(next_buffer_idx == 0) return BUFFER_EMPTY;
    for(uint32_t cidx = 0; cidx < commands_size; cidx++) {
        const Command& cmd = commands[cidx];
        const char* name = cmd.name;
        const uint32_t name_len = strlen(name); // excluding null-terminator
        uint32_t match_count = 0;
        for(uint32_t i = 0; i < next_buffer_idx; i++) {
            const char c = input_buffer[i];
            // If characters sequentially match the contents of a command
            // name, increase the match_count
            if(c == name[match_count] && c != '\0') match_count++;
            else {
                // Character doesn't match
                // case 1: End of string
                // case 2: C is the space after the command name
                // case 3: Wrong command => reset match_count
                if(match_count == name_len && (c == '\0' || c == CMD_CODE_DELIM)) {
                    // execute matching command
                    const char* str = &input_buffer[i + 1];
                    uint32_t str_size = strlen(str) + 1;
                    // execute command function
                    cmd.function(str, str_size);
                    clearBuffer();
                    return SUCCESS;
                }
                else {
                    // reset match counter
                    match_count = 0;
                }
            }
        }
    }
    clearBuffer();
    return NO_MATCH;
}