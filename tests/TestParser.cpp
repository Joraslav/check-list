#include "Parser.hpp"

#include <gtest/gtest.h>

using namespace parser;

// Тесты для CommandToEnum
TEST(CommandToEnumTest, ValidCommands) {
    EXPECT_EQ(CommandToEnum("add"), TypeCommand::ADD);
    EXPECT_EQ(CommandToEnum("list"), TypeCommand::LIST);
    EXPECT_EQ(CommandToEnum("clear"), TypeCommand::CLEAR);
    EXPECT_EQ(CommandToEnum("done"), TypeCommand::DONE);
    EXPECT_EQ(CommandToEnum("remove"), TypeCommand::REMOVE);
    EXPECT_EQ(CommandToEnum("edit"), TypeCommand::EDIT);
    EXPECT_EQ(CommandToEnum("help"), TypeCommand::HELP);
    EXPECT_EQ(CommandToEnum("config"), TypeCommand::CONFIG);
}

TEST(CommandToEnumTest, InvalidCommand) {
    EXPECT_THROW(CommandToEnum("invalid"), std::invalid_argument);
    EXPECT_THROW(CommandToEnum(""), std::invalid_argument);
}

// Тесты для IsValidCommandWords
TEST(IsValidCommandWordsTest, AddCommand) {
    EXPECT_FALSE(IsValidCommandWords(TypeCommand::ADD, 2));  // add
    EXPECT_TRUE(IsValidCommandWords(TypeCommand::ADD, 3));   // add task
    EXPECT_TRUE(IsValidCommandWords(TypeCommand::ADD, 4));  // add task text
}

TEST(IsValidCommandWordsTest, ListCommand) {
    EXPECT_TRUE(IsValidCommandWords(TypeCommand::LIST, 2));  // list
    EXPECT_TRUE(IsValidCommandWords(TypeCommand::LIST, 3)); // list pending
    EXPECT_FALSE(IsValidCommandWords(TypeCommand::LIST, 4)); // list pending extra
}

TEST(IsValidCommandWordsTest, ClearCommand) {
    EXPECT_TRUE(IsValidCommandWords(TypeCommand::CLEAR, 2));  // clear
    EXPECT_FALSE(IsValidCommandWords(TypeCommand::CLEAR, 1)); // no command
    EXPECT_FALSE(IsValidCommandWords(TypeCommand::CLEAR, 3)); // clear extra
}

TEST(IsValidCommandWordsTest, DoneCommand) {
    EXPECT_TRUE(IsValidCommandWords(TypeCommand::DONE, 3));  // done 1
    EXPECT_FALSE(IsValidCommandWords(TypeCommand::DONE, 2));  // done
    EXPECT_FALSE(IsValidCommandWords(TypeCommand::DONE, 4));   // done 1 extra
}

TEST(IsValidCommandWordsTest, RemoveCommand) {
    EXPECT_TRUE(IsValidCommandWords(TypeCommand::REMOVE, 3));  // remove 1
    EXPECT_FALSE(IsValidCommandWords(TypeCommand::REMOVE, 2));  // remove
    EXPECT_FALSE(IsValidCommandWords(TypeCommand::REMOVE, 4));  // remove 1 extra
}

TEST(IsValidCommandWordsTest, EditCommand) {
    EXPECT_FALSE(IsValidCommandWords(TypeCommand::EDIT, 3));  // edit 1
    EXPECT_TRUE(IsValidCommandWords(TypeCommand::EDIT, 4));   // edit 1 text
    EXPECT_TRUE(IsValidCommandWords(TypeCommand::EDIT, 5));   // edit 1 text more
}

TEST(IsValidCommandWordsTest, HelpCommand) {
    EXPECT_TRUE(IsValidCommandWords(TypeCommand::HELP, 2));  // help
    EXPECT_FALSE(IsValidCommandWords(TypeCommand::HELP, 1)); // no command
    EXPECT_FALSE(IsValidCommandWords(TypeCommand::HELP, 3)); // help extra
}

TEST(IsValidCommandWordsTest, ConfigCommand) {
    EXPECT_FALSE(IsValidCommandWords(TypeCommand::CONFIG, 3));  // config path
    EXPECT_TRUE(IsValidCommandWords(TypeCommand::CONFIG, 4));   // config path value
    EXPECT_TRUE(IsValidCommandWords(TypeCommand::CONFIG, 5));   // config path value more
}

// Тесты для WordToNumber
TEST(WordToNumberTest, ValidNumbers) {
    EXPECT_EQ(WordToNumber("0"), 0);
    EXPECT_EQ(WordToNumber("1"), 1);
    EXPECT_EQ(WordToNumber("123"), 123);
    EXPECT_EQ(WordToNumber("999999"), 999999);
}

TEST(WordToNumberTest, InvalidNumbers) {
    EXPECT_THROW(WordToNumber(""), std::invalid_argument);
    EXPECT_THROW(WordToNumber("abc"), std::invalid_argument);
    EXPECT_THROW(WordToNumber("12a"), std::invalid_argument);
    EXPECT_THROW(WordToNumber("1 2"), std::invalid_argument);
}

// Тесты для ToLower
TEST(ToLowerTest, ValidStrings) {
    EXPECT_EQ(ToLower("HELLO"), "hello");
    EXPECT_EQ(ToLower("Hello"), "hello");
    EXPECT_EQ(ToLower("hello"), "hello");
    EXPECT_EQ(ToLower(""), "");
    EXPECT_EQ(ToLower("123"), "123");
    EXPECT_EQ(ToLower("Hello World!"), "hello world!");
}

// Тесты для Parser::Parse
TEST(ParserTest, HelpCommand) {
    int argc = 2;
    char* argv[] = { (char*)"todo", (char*)"help" };
    
    Parser parser;
    parser.Parse(argc, argv);
    
    EXPECT_EQ(parser.GetTypeCommand(), TypeCommand::HELP);
}

TEST(ParserTest, AddCommand) {
    int argc = 4;
    char* argv[] = { (char*)"todo", (char*)"add", (char*)"task", (char*)"description" };
    
    Parser parser;
    parser.Parse(argc, argv);
    
    EXPECT_EQ(parser.GetTypeCommand(), TypeCommand::ADD);
    EXPECT_EQ(parser.GetTaskText(), "task description");
}

TEST(ParserTest, ListCommand) {
    int argc = 2;
    char* argv[] = { (char*)"todo", (char*)"list" };
    
    Parser parser;
    parser.Parse(argc, argv);
    
    EXPECT_EQ(parser.GetTypeCommand(), TypeCommand::LIST);
}

TEST(ParserTest, ListPendingCommand) {
    int argc = 3;
    char* argv[] = { (char*)"todo", (char*)"list", (char*)"pending" };
    
    Parser parser;
    parser.Parse(argc, argv);
    
    EXPECT_EQ(parser.GetTypeCommand(), TypeCommand::LIST);
    EXPECT_TRUE(std::holds_alternative<ListOption>(parser.GetCommandOption()));
    EXPECT_EQ(std::get<ListOption>(parser.GetCommandOption()), ListOption::PENDING);
}

TEST(ParserTest, ListCompletedCommand) {
    int argc = 3;
    char* argv[] = { (char*)"todo", (char*)"list", (char*)"completed" };
    
    Parser parser;
    parser.Parse(argc, argv);
    
    EXPECT_EQ(parser.GetTypeCommand(), TypeCommand::LIST);
    EXPECT_TRUE(std::holds_alternative<ListOption>(parser.GetCommandOption()));
    EXPECT_EQ(std::get<ListOption>(parser.GetCommandOption()), ListOption::COMPLETED);
}

TEST(ParserTest, ClearCommand) {
    int argc = 2;
    char* argv[] = { (char*)"todo", (char*)"clear" };
    
    Parser parser;
    parser.Parse(argc, argv);
    
    EXPECT_EQ(parser.GetTypeCommand(), TypeCommand::CLEAR);
}

TEST(ParserTest, DoneCommand) {
    int argc = 3;
    char* argv[] = { (char*)"todo", (char*)"done", (char*)"5" };
    
    Parser parser;
    parser.Parse(argc, argv);
    
    EXPECT_EQ(parser.GetTypeCommand(), TypeCommand::DONE);
    EXPECT_TRUE(parser.GetTaskIndex().has_value());
    EXPECT_EQ(parser.GetTaskIndex().value(), 5);
}

TEST(ParserTest, RemoveCommand) {
    int argc = 3;
    char* argv[] = { (char*)"todo", (char*)"remove", (char*)"3" };
    
    Parser parser;
    parser.Parse(argc, argv);
    
    EXPECT_EQ(parser.GetTypeCommand(), TypeCommand::REMOVE);
    EXPECT_TRUE(parser.GetTaskIndex().has_value());
    EXPECT_EQ(parser.GetTaskIndex().value(), 3);
}

TEST(ParserTest, EditCommand) {
    int argc = 5;
    char* argv[] = { (char*)"todo", (char*)"edit", (char*)"2", (char*)"new", (char*)"description" };
    
    Parser parser;
    parser.Parse(argc, argv);
    
    EXPECT_EQ(parser.GetTypeCommand(), TypeCommand::EDIT);
    EXPECT_TRUE(parser.GetTaskIndex().has_value());
    EXPECT_EQ(parser.GetTaskIndex().value(), 2);
    EXPECT_EQ(parser.GetTaskText(), "new description");
}

TEST(ParserTest, ConfigPathCommand) {
    int argc = 4;
    char* argv[] = { (char*)"todo", (char*)"config", (char*)"path", (char*)"/new/path" };
    
    Parser parser;
    parser.Parse(argc, argv);
    
    EXPECT_EQ(parser.GetTypeCommand(), TypeCommand::CONFIG);
    EXPECT_TRUE(std::holds_alternative<ConfigOption>(parser.GetCommandOption()));
    EXPECT_EQ(std::get<ConfigOption>(parser.GetCommandOption()), ConfigOption::PATH);
    EXPECT_EQ(parser.GetTaskText(), "/new/path");
}

TEST(ParserTest, ConfigNameCommand) {
    int argc = 4;
    char* argv[] = { (char*)"todo", (char*)"config", (char*)"name", (char*)"newname.txt" };
    
    Parser parser;
    parser.Parse(argc, argv);
    
    EXPECT_EQ(parser.GetTypeCommand(), TypeCommand::CONFIG);
    EXPECT_TRUE(std::holds_alternative<ConfigOption>(parser.GetCommandOption()));
    EXPECT_EQ(std::get<ConfigOption>(parser.GetCommandOption()), ConfigOption::NAME);
    EXPECT_EQ(parser.GetTaskText(), "newname.txt");
}

// Тесты для проверки обработки ошибок
TEST(ParserErrorTest, UnknownCommand) {
    int argc = 2;
    char* argv[] = { (char*)"todo", (char*)"unknown" };
    
    Parser parser;
    EXPECT_THROW(parser.Parse(argc, argv), std::invalid_argument);
}

TEST(ParserErrorTest, InvalidArgumentCount) {
    int argc = 1;
    char* argv[] = { (char*)"todo" };
    
    Parser parser;
    parser.Parse(argc, argv);
    
    EXPECT_EQ(parser.GetTypeCommand(), TypeCommand::HELP);
}

TEST(ParserErrorTest, InvalidListOption) {
    int argc = 3;
    char* argv[] = { (char*)"todo", (char*)"list", (char*)"invalid" };
    
    Parser parser;
    EXPECT_THROW(parser.Parse(argc, argv), std::invalid_argument);
}

TEST(ParserErrorTest, InvalidConfigOption) {
    int argc = 4;
    char* argv[] = { (char*)"todo", (char*)"config", (char*)"invalid", (char*)"value" };
    
    Parser parser;
    EXPECT_THROW(parser.Parse(argc, argv), std::invalid_argument);
}

TEST(ParserErrorTest, InvalidNumberInDone) {
    int argc = 3;
    char* argv[] = { (char*)"todo", (char*)"done", (char*)"abc" };
    
    Parser parser;
    EXPECT_THROW(parser.Parse(argc, argv), std::invalid_argument);
}

TEST(ParserErrorTest, InvalidNumberInRemove) {
    int argc = 3;
    char* argv[] = { (char*)"todo", (char*)"remove", (char*)"xyz" };
    
    Parser parser;
    EXPECT_THROW(parser.Parse(argc, argv), std::invalid_argument);
}

TEST(ParserErrorTest, InvalidNumberInEdit) {
    int argc = 4;
    char* argv[] = { (char*)"todo", (char*)"edit", (char*)"invalid", (char*)"text" };
    
    Parser parser;
    EXPECT_THROW(parser.Parse(argc, argv), std::invalid_argument);
}
