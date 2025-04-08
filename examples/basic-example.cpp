#include <iostream>
#include "stint.h"

using std::cout, std::endl, std::cin;

bool running = true;

void cmd_help(const char* cmd, uint32_t cmd_len);
void cmd_echo(const char* cmd, uint32_t cmd_len);
void cmd_exit(const char* cmd, uint32_t cmd_len);

Stint::Command commands[] = {
    {.name = "help", .function = cmd_help, .helptext = "Prints all known commands and their helptext"},
    {.name = "echo", .function = cmd_echo, .helptext = "Echoes the given input"},
    {.name = "exit", .function = cmd_exit, .helptext = "Exits the program"}
};
char buf[128];
Stint stint{commands, std::size(commands), buf, sizeof(buf)};


void cmd_help(const char* cmd, uint32_t cmd_len) {
    for(const Stint::Command& c : commands) {
        cout << c.name << ": " << c.helptext << endl;
    }
}

void cmd_echo(const char* cmd, uint32_t cmd_len) {
    cout << cmd << endl;
}

void cmd_exit(const char* cmd, uint32_t cmd_len) {
    running = false;
}

int main() {
    std::string input;
    std::cout << "Basic STInt Example."<< endl
        <<"Type help for a list of commands and confirm with enter" << endl;
    while(running) {
        // Get user input
        std::getline(cin,input);
        // since getline ignores the linebreak char, we have to add that manually
        // in order for the command to be parsed
        input += "\n";
        // Feed each char from the input to the interpreter
        for(char c:input) {
            Stint::ErrorCode ret = stint.ingest(c);
            switch(ret) {
                default:
                    cout << "Unknown error" << endl;
                    break;
                case Stint::NO_MATCH:
                    cout << "Unknown command" << endl;
                    break;
                case Stint::SUCCESS:
                    // Do nothing
                    break;
            }
        }
    }
    return 0;
}
