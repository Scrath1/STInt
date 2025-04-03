#include "stint.h"

#include <cstring>

Stint::Stint(const Command commands[], uint32_t commands_size, char input_buffer[], uint32_t input_buffer_size)
    : commands(commands),
      commands_size(commands_size),
      input_buffer_size(input_buffer_size),
      input_buffer(input_buffer) {
    next_buffer_idx = 0;
}
Stint::ErrorCode Stint::ingest(char c) {
    // multiple command delims without content between them are ignored and
    // not added to the buffer
    if(isLineEndDelim(c) && next_buffer_idx > 0) {
        // character signifies end of command. Parse and execute that command
        input_buffer[next_buffer_idx++] = '\0';
        return parse();
    } else if(!isLineEndDelim(c)){
        // add character to buffer and move on
        if(next_buffer_idx >= input_buffer_size - 1) return BUFFER_FULL;
        input_buffer[next_buffer_idx++] = c;
    }

    return SUCCESS;
}

void Stint::clearBuffer() { next_buffer_idx = 0; }

bool Stint::isLineEndDelim(char c) {
    return (c == '\r' || c == '\n' || c == '\0');
}

Stint::ErrorCode Stint::parse() {
    if(next_buffer_idx == 0) return BUFFER_EMPTY;
    for(uint32_t cidx = 0; cidx < commands_size; cidx++) {
        const Command& cmd = commands[cidx];
        // skip commands which do not have valid functions. Those shouldn't exist anyway
        if(cmd.function == nullptr) continue;
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
                // case 1: End of string/line. Probably a one word command
                // case 2: c is the space after the command name and there is a parameter behind it
                // case 3: Wrong command => reset match_count
                if(match_count == name_len && (isLineEndDelim(c) || c == ' ')) {
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