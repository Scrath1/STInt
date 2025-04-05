#include <gtest/gtest.h>
#include "stint.h"

#define BUF_SIZE (16)

bool foo_called = false;
void cmd_foo(const char* cmd, uint32_t cmd_len) {
    foo_called = true;
}

bool foobar_called = false;
void cmd_foobar(const char* cmd, uint32_t cmd_len) {
    foobar_called = true;
}

bool barfoo_called = false;
void cmd_barfoo(const char* cmd, uint32_t cmd_len) {
    barfoo_called = true;
}

uint32_t given_cmd_len = 0;
char given_cmd[BUF_SIZE] = "";
void cmd_stringCheck(const char* cmd, uint32_t cmd_len) {
    given_cmd_len = cmd_len;
    strncpy(given_cmd, cmd, cmd_len);
}

class Stint_Test: public testing::Test {
protected:
    Stint::Command commands[4] = {
        {.name = "foo", .function = cmd_foo, .helptext = "Prints \"bar\""},
        {.name = "foobar", .function = cmd_foobar, .helptext = ""},
        {.name = "check", .function = cmd_stringCheck, .helptext = "Stores given length of string"},
        {.name = "barfoo", .function = cmd_barfoo, .helptext = ""}
    };
    char buf[BUF_SIZE] = "";

    Stint stint{commands, sizeof(commands) / sizeof(commands[0]), buf, sizeof(buf)};

    void SetUp() override {

    }

    void TearDown() override {
        foo_called = false;
        foobar_called = false;
        memset(given_cmd, '\0', BUF_SIZE);
        given_cmd_len = 0;
    }
};

TEST_F(Stint_Test, BufferLimitTest) {
    for(uint32_t i = 0; i < sizeof(buf) - 1; i++) {
        EXPECT_EQ(Stint::SUCCESS, stint.ingest('a'));
    }
    EXPECT_EQ(Stint::BUFFER_FULL, stint.ingest('b'));
}

TEST_F(Stint_Test, BasicParsingTest) {
    // Expect function foo to be called since \0 counts as string end delimiter
    ASSERT_FALSE(foo_called);
    constexpr char input1[] = "foo";
    for(char c:input1) {
        EXPECT_EQ(Stint::SUCCESS,stint.ingest(c));
    }
    EXPECT_TRUE(foo_called);
    foo_called = false;
    // Expect function foo to be called
    char input2[] = "foo\n";
    for(char c:input2) {
        EXPECT_EQ(Stint::SUCCESS,stint.ingest(c));
    }
    EXPECT_TRUE(foo_called);
    // Expect function foobar to be called and not foo
    foo_called = false;
    char input3[] = "foobar\n";
    for(char c:input3) {
        EXPECT_EQ(Stint::SUCCESS,stint.ingest(c));
    }
    EXPECT_TRUE(foobar_called);
    EXPECT_FALSE(foo_called);
    // Expect function barfoo to be called and not foo
    foo_called = false;
    barfoo_called = false;
    ASSERT_FALSE(barfoo_called);
    ASSERT_FALSE(foo_called);
    char input4[] = "barfoo";
    for(char c:input4) {
        EXPECT_EQ(Stint::SUCCESS, stint.ingest(c));
    }
    EXPECT_FALSE(foo_called);
    EXPECT_TRUE(barfoo_called);
}

TEST_F(Stint_Test, CommandParameterTest) {
    // Purpose of this test is to validate the correct passing of the remaining
    // command content to the command function
    constexpr char input1[] = "check this\n";
    constexpr char expectedParam[] = "this";
    constexpr uint32_t expectParam_size = sizeof(expectedParam);
    for(auto c : input1) {
        EXPECT_EQ(Stint::SUCCESS,stint.ingest(c));
    }
    EXPECT_EQ(expectParam_size, given_cmd_len);
    EXPECT_STREQ(expectedParam, given_cmd);
}

TEST_F(Stint_Test, FailedCommandBufferClearTest) {
    // Checks that the buffer was cleared following a failed
    // command so that the next valid command will work
    constexpr char input1[] = "invalid";
    for(auto c : input1) {
        if(c != '\0') EXPECT_EQ(Stint::SUCCESS,stint.ingest(c));

    }
    EXPECT_EQ(Stint::NO_MATCH, stint.ingest('\0'));

    ASSERT_FALSE(foo_called);
    constexpr char input2[] = "foo\n";
    for(auto c : input2) {
        EXPECT_EQ(Stint::SUCCESS,stint.ingest(c));
    }
    EXPECT_TRUE(foo_called);
}

TEST_F(Stint_Test, LineEndingsTest) {
    constexpr char input1[] = "foo\r";
    for(auto c : input1) {
        EXPECT_EQ(Stint::SUCCESS,stint.ingest(c));
    }
    EXPECT_TRUE(foo_called);
    foo_called = false;
    constexpr char input2[] = "foo\n";
    for(auto c : input2) {
        EXPECT_EQ(Stint::SUCCESS,stint.ingest(c));
    }
    EXPECT_TRUE(foo_called);
    foo_called = false;
    constexpr char input3[] = "foo\r\n";
    for(auto c : input3) {
        EXPECT_EQ(Stint::SUCCESS,stint.ingest(c));
    }
    EXPECT_TRUE(foo_called);
}

TEST_F(Stint_Test, BackspaceTest) {
    constexpr char input1[] = "fooo";
    constexpr uint32_t input1_len = strlen(input1); // number of characters excluding null-terminator
    for(uint32_t i = 0; i < input1_len; i++) {
        EXPECT_EQ(Stint::SUCCESS,stint.ingest((input1[i])));
    }
    EXPECT_EQ(input1_len, stint.fillLevel());
    stint.deleteLastChar();
    EXPECT_EQ(input1_len - 1, stint.fillLevel());
    EXPECT_EQ(Stint::SUCCESS, stint.ingest('\n'));
    EXPECT_TRUE(foo_called);

    // Also check correct handling if no data is in buffer
    stint.clearBuffer();
    ASSERT_EQ(0, stint.fillLevel());
    stint.deleteLastChar();
    EXPECT_EQ(0, stint.fillLevel());
}

TEST_F(Stint_Test, AutoBackspaceTest){
    stint.setAutoBackspace(true);
    constexpr char input1[] = "fooo\b\n";
    for(char c:input1) {
        EXPECT_EQ(Stint::SUCCESS, stint.ingest(c));
    }
    EXPECT_TRUE(foo_called);
    foo_called = false;
    stint.setAutoBackspace(false);
    constexpr char input2[] = "fooo\b";
    for(uint32_t i = 0; i < strlen(input2); i++) {
        EXPECT_EQ(Stint::SUCCESS, stint.ingest(input2[i]));
    }
    EXPECT_EQ(Stint::NO_MATCH, stint.ingest('\n'));
}