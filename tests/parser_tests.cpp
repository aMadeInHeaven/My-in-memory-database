#include <gtest/gtest.h>
#include "Engine/Parser.h"

class ParserTest : public ::testing::Test {
protected:
    Parser parser_;
};

TEST_F(ParserTest, ParseSimpleCommand) {
    auto tokens = Parser::ParseLine("SET key value");
    ASSERT_EQ(tokens.size(), 3);
    EXPECT_EQ(tokens[0], "SET");
    EXPECT_EQ(tokens[1], "key");
    EXPECT_EQ(tokens[2], "value");
}

TEST_F(ParserTest, ParseCommandWithMultipleArgs) {
    auto tokens = Parser::ParseLine("LPUSH mylist a b c d");
    ASSERT_EQ(tokens.size(), 6);
    EXPECT_EQ(tokens[0], "LPUSH");
    EXPECT_EQ(tokens[1], "mylist");
    EXPECT_EQ(tokens[2], "a");
    EXPECT_EQ(tokens[3], "b");
    EXPECT_EQ(tokens[4], "c");
    EXPECT_EQ(tokens[5], "d");
}

TEST_F(ParserTest, ParseEmptyLine) {
    auto tokens = Parser::ParseLine("");
    EXPECT_TRUE(tokens.empty());
}

TEST_F(ParserTest, ParseLineWithSpaces) {
    auto tokens = Parser::ParseLine("   GET    key   ");
    ASSERT_EQ(tokens.size(), 2);
    EXPECT_EQ(tokens[0], "GET");
    EXPECT_EQ(tokens[1], "key");
}

TEST_F(ParserTest, ParseGeoCommand) {
    auto tokens = Parser::ParseLine("GEOADD cities 13.361389 38.115556 Palermo");
    ASSERT_EQ(tokens.size(), 5);
    EXPECT_EQ(tokens[0], "GEOADD");
    EXPECT_EQ(tokens[1], "cities");
    EXPECT_EQ(tokens[2], "13.361389");
    EXPECT_EQ(tokens[3], "38.115556");
    EXPECT_EQ(tokens[4], "Palermo");
}


TEST_F(ParserTest, ParseTabsAndMultipleSpaces) {
    auto tokens = Parser::ParseLine("SET\tkey   value");
    ASSERT_EQ(tokens.size(), 3);
    EXPECT_EQ(tokens[0], "SET");
    EXPECT_EQ(tokens[1], "key");
    EXPECT_EQ(tokens[2], "value");
}
