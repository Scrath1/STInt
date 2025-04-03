#include <iostream>
#include "stint.h"

using std::cout, std::endl;

void cmd_foobar(const char* cmd, uint32_t cmd_len) {
    cout << "bar" << endl;
}

Stint::Command commands[] = {
    {.name = "foo", .function = cmd_foobar, .helptext = "Prints \"bar\""}
};
char buf[16];

Stint stint{commands, std::size(commands), buf, sizeof(buf)};

int main() {
    const char input[] = "foo something\n";
    for(char c : input) {
        if(Stint::SUCCESS != stint.ingest(c)) {
            std::cerr << "Parsing error" << endl;
        }
    }
    return 0;
}
