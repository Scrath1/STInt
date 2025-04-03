#ifndef STINT_H
#define STINT_H
#include <cstdint>

class Stint {
   public:
    enum ErrorCode {
        NO_MATCH = -4,
        BUFFER_EMPTY = -3, // The input buffer is empty
        BUFFER_FULL = -2, // The input buffer is full
        ERROR = -1,
        SUCCESS = 0
    };

    typedef void (*StintCmdFunction_t)(const char* cmd, uint32_t cmd_len);

    struct Command {
        const char* name;
        const StintCmdFunction_t function;
        const char* helptext;
    };

    /**
     *
     * @param commands [IN] Array of commands that stint can use
     * @param commands_size [IN] Size of commands array
     * @param input_buffer [IN] Buffer in which Stint will store user input for parsing commands
     * @param input_buffer_size [IN] size of input_buffer. The actual maximum size of the usable buffer
     *  will always be input_buffer_size - 1 to allow room for the null-terminator at the end
     */
    Stint(const Command commands[], uint32_t commands_size, char input_buffer[], uint32_t input_buffer_size);

    /**
     * Adds a character to the input_buffer
     * @param c [IN] Character to add to the input buffer
     * @return SUCCESS on success
     * @return BUFFER_FULL if the input buffer is full
     */
    ErrorCode ingest(char c);

    /**
     * Returns the array of commands
     */
    inline const Command* const getCommands() const {return commands;}

    /**
     * Returns the number of known commands
     */
    inline uint32_t getNumCommands() const {return commands_size;}

    /**
     * Empties the input buffer
     */
    void clearBuffer();

   private:
    /**
     * Returns true if a character is a valid command end delimiter
     * Valid characters are '\r', '\n', and '\0'
     * @param c
     * @return
     */
    static bool isLineEndDelim(char c);

    /**
     * Checks for a command in the input buffer and executes the respective function
     * @return SUCCESS on success
     * @return BUFFER_EMPTY if the input buffer is empty
     * @return NO_MATCH if no matching command was found
     */
    ErrorCode parse();

    // Array of commands that Stint knows
    const Command* commands;
    // Size of commands array
    const uint32_t commands_size;
    // Size of input buffer
    const uint32_t input_buffer_size;
    // Char array for storing and analyzing user input
    char* input_buffer;
    // next free char index in the input_buffer
    uint32_t next_buffer_idx;
};

#endif  // STINT_H
