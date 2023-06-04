//
// Created by Jan Kędra on 16/04/2023.
//

#include <variant>
#include <string>
#include "../include/Token.h"
#include "../include/Lexer.h"
#include "Parser.h"
#include "Visitors/ParserVisitor.h"
#include "gtest/gtest.h"
#include "SemanticAnalyzerVisitor.h"
#include "InterpreterVisitor.h"

typedef TokenType tt;
using tv = std::variant<int, float, std::string>;

TEST(GeneralSuite, TestGTest) {
    EXPECT_EQ(2 + 2, 4);
}

TEST(LexerSuite, simpleInt) {
    std::vector<std::pair<std::string, std::vector<tt>>> test;

    std::string program("int");
    std::vector<tt> types = {
            tt::IntegerToken,
            tt::EoF};

    std::vector<tt> tokens;
    std::stringstream ss;
    ss << program;
    Lexer lexer(ss);
    Token t(tt::Unknown, Position());
    do {
        t = lexer.nextToken();
        tokens.push_back(t.getType());
    } while (t.getType() != tt::EoF);

    EXPECT_EQ(tokens, types);
}

TEST(LexerSuite, simpleFloat) {
    std::vector<std::pair<std::string, std::vector<tt>>> test;

    std::string program("float");
    std::vector<tt> types = {
            tt::FloatToken,
            tt::EoF};

    std::vector<tt> tokens;
    std::stringstream ss;
    ss << program;
    Lexer lexer(ss);
    Token t(tt::Unknown, Position());
    do {
        t = lexer.nextToken();
        tokens.push_back(t.getType());
    } while (t.getType() != tt::EoF);

    EXPECT_EQ(tokens, types);
}

TEST(LexerSuite, simpleString) {
    std::vector<std::pair<std::string, std::vector<tt>>> test;

    std::string program("string a = \"Hello world\";");
    std::vector<tt> types = {
            tt::StringToken,
            tt::Identifier,
            tt::Assign,
            tt::StringLiteral,
            tt::Semicolon,
            tt::EoF};

    std::vector<tt> tokens;
    std::stringstream ss;
    ss << program;
    Lexer lexer(ss);
    Token t(tt::Unknown, Position());
    do {
        t = lexer.nextToken();
        tokens.push_back(t.getType());
    } while (t.getType() != tt::EoF);

    EXPECT_EQ(tokens, types);
}

TEST(LexerSuite, simpleComment) {
    std::vector<std::pair<std::string, std::vector<tt>>> test;

    std::string program("// I'm a helpful comment!");
    std::vector<tt> types = {
            tt::Comment,
            tt::EoF};

    std::vector<tt> tokens;
    std::stringstream ss;
    ss << program;
    Lexer lexer(ss);
    Token t(tt::Unknown, Position());
    do {
        t = lexer.nextToken();
        tokens.push_back(t.getType());
    } while (t.getType() != tt::EoF);

    EXPECT_EQ(tokens, types);
}

TEST(LexerSuite, simpleCommentContent) {
    std::string program("// I'm a helpful comment!");

    std::vector<tv> tokens;
    std::stringstream ss;
    ss << program;
    Lexer lexer(ss);
    Token t(tt::Unknown, Position());
    do {
        t = lexer.nextToken();
        tokens.push_back(t.getValue());
    } while (t.getType() != tt::EoF);

    EXPECT_EQ(static_cast<std::string>(std::get<std::string>(tokens.at(0))), "I'm a helpful comment!");
}

TEST(LexerSuite, simpleSunnyInt) {
    std::vector<std::pair<std::string, std::vector<tt>>> test;

    std::string program("int m() { printf(0); }");
    std::vector<tt> types = {
            tt::IntegerToken, tt::Identifier, tt::OpenParenthesis, tt::ClosingParenthesis,
            tt::OpenBracket,
            tt::Identifier,tt::OpenParenthesis, tt::IntegerLiteral, tt::ClosingParenthesis, tt::Semicolon,
            tt::ClosingBracket,
            tt::EoF};

    std::vector<tt> tokens;
    std::stringstream ss;
    ss << program;
    Lexer lexer(ss);
    Token t(tt::Unknown, Position());
    do {
        t = lexer.nextToken();
        tokens.push_back(t.getType());
    } while (t.getType() != tt::EoF);

    EXPECT_EQ(tokens, types);
}

TEST(LexerSuite, simpleSunnyIntOverflow) {
    std::vector<std::pair<std::string, std::vector<tt>>> test;

    std::string program("20000147483647");

    std::vector<tt> tokens;
    std::stringstream ss;
    ss << program;
    Lexer lexer(ss);
    EXPECT_THROW(lexer.nextToken(), std::overflow_error);
}

TEST(LexerSuite, simpleSunnyFloat) {
    std::vector<std::pair<std::string, std::vector<tt>>> test;

    std::string program("int m() { printf(1.1); }");
    std::vector<tt> types = {
            tt::IntegerToken, tt::Identifier, tt::OpenParenthesis, tt::ClosingParenthesis,
            tt::OpenBracket,
            tt::Identifier,tt::OpenParenthesis, tt::FloatLiteral, tt::ClosingParenthesis, tt::Semicolon,
            tt::ClosingBracket,
            tt::EoF};

    std::vector<tt> tokens;
    std::stringstream ss;
    ss << program;
    Lexer lexer(ss);
    Token t(tt::Unknown, Position());
    do {
        t = lexer.nextToken();
        tokens.push_back(t.getType());
    } while (t.getType() != tt::EoF);

    EXPECT_EQ(tokens, types);
}

TEST(LexerSuite, simpleSunnyDictionaryWhere) {
    std::vector<std::pair<std::string, std::vector<tt>>> test;

    std::string program("dictionary.where(n => n % 2 == 0);");
    std::vector<tt> types = {
            tt::Identifier, tt::Dot, tt::Where, tt::OpenParenthesis, tt::Identifier, tt::Lambda, tt::Identifier,
            tt::Mod, tt::IntegerLiteral, tt::Equal, tt::IntegerLiteral, tt::ClosingParenthesis, tt::Semicolon,
            tt::EoF};

    std::vector<tt> tokens;
    std::stringstream ss;
    ss << program;
    Lexer lexer(ss);
    Token t(tt::Unknown, Position());
    do {
        t = lexer.nextToken();
        tokens.push_back(t.getType());
    } while (t.getType() != tt::EoF);

    EXPECT_EQ(tokens, types);
}

TEST(LexerSuite, simpleSunnyRelGreater) {
    std::vector<std::pair<std::string, std::vector<tt>>> test;

    std::string program(">a");
    std::vector<tt> types = {
            tt::Greater, tt::Identifier,
            tt::EoF};

    std::vector<tt> tokens;
    std::stringstream ss;
    ss << program;
    Lexer lexer(ss);
    Token t(tt::Unknown, Position());
    do {
        t = lexer.nextToken();
        tokens.push_back(t.getType());
    } while (t.getType() != tt::EoF);

    EXPECT_EQ(tokens, types);
}

TEST(LexerSuite, simpleSunnyRelGreaterEqual) {
    std::vector<std::pair<std::string, std::vector<tt>>> test;

    std::string program(">=a");
    std::vector<tt> types = {
            tt::GreaterEqual, tt::Identifier,
            tt::EoF};

    std::vector<tt> tokens;
    std::stringstream ss;
    ss << program;
    Lexer lexer(ss);
    Token t(tt::Unknown, Position());
    do {
        t = lexer.nextToken();
        tokens.push_back(t.getType());
    } while (t.getType() != tt::EoF);

    EXPECT_EQ(tokens, types);
}

TEST(LexerSuite, simpleSunnyRelEqual) {
    std::vector<std::pair<std::string, std::vector<tt>>> test;

    std::string program("==a");
    std::vector<tt> types = {
            tt::Equal, tt::Identifier,
            tt::EoF};

    std::vector<tt> tokens;
    std::stringstream ss;
    ss << program;
    Lexer lexer(ss);
    Token t(tt::Unknown, Position());
    do {
        t = lexer.nextToken();
        tokens.push_back(t.getType());
    } while (t.getType() != tt::EoF);

    EXPECT_EQ(tokens, types);
}

TEST(LexerSuite, simpleSunnyRelLess) {
    std::vector<std::pair<std::string, std::vector<tt>>> test;

    std::string program("<a");
    std::vector<tt> types = {
            tt::Less, tt::Identifier,
            tt::EoF};

    std::vector<tt> tokens;
    std::stringstream ss;
    ss << program;
    Lexer lexer(ss);
    Token t(tt::Unknown, Position());
    do {
        t = lexer.nextToken();
        tokens.push_back(t.getType());
    } while (t.getType() != tt::EoF);

    EXPECT_EQ(tokens, types);
}

TEST(LexerSuite, simpleSunnyRelLessEqual) {
    std::vector<std::pair<std::string, std::vector<tt>>> test;

    std::string program("<=a");
    std::vector<tt> types = {
            tt::LessEqual, tt::Identifier,
            tt::EoF};

    std::vector<tt> tokens;
    std::stringstream ss;
    ss << program;
    Lexer lexer(ss);
    Token t(tt::Unknown, Position());
    do {
        t = lexer.nextToken();
        tokens.push_back(t.getType());
    } while (t.getType() != tt::EoF);

    EXPECT_EQ(tokens, types);
}

TEST(LexerSuite, simpleSunnyString) {
    std::vector<std::pair<std::string, std::vector<tt>>> test;

    std::string program("printf(\"Hello world!\");");
    std::vector<tt> types = {
            tt::Identifier, tt::OpenParenthesis, tt::StringLiteral, tt::ClosingParenthesis, tt::Semicolon,
            tt::EoF};

    std::vector<tt> tokens;
    std::stringstream ss;
    ss << program;
    Lexer lexer(ss);
    Token t(tt::Unknown, Position());
    do {
        t = lexer.nextToken();
        tokens.push_back(t.getType());
    } while (t.getType() != tt::EoF);

    EXPECT_EQ(tokens, types);
}

TEST(LexerSuite, simpleSunnyStringNoEnd) {
    std::vector<std::pair<std::string, std::vector<tt>>> test;

    std::string program("printf(\"Hello world!);");
    std::vector<tt> types = {
            tt::Identifier, tt::OpenParenthesis, tt::Unknown,
            tt::EoF};

    std::vector<tt> tokens;
    std::stringstream ss;
    ss << program;
    Lexer lexer(ss);
    Token t(tt::Unknown, Position());
    do {
        t = lexer.nextToken();
        tokens.push_back(t.getType());
    } while (t.getType() != tt::EoF);

    EXPECT_EQ(tokens, types);
}

TEST(LexerSuite, simpleSunnyStringNoEndBackslash) {
    std::vector<std::pair<std::string, std::vector<tt>>> test;

    std::string program("printf(\"Hello world!);\\");
    std::vector<tt> types = {
            tt::Identifier, tt::OpenParenthesis, tt::Unknown,
            tt::EoF};

    std::vector<tt> tokens;
    std::stringstream ss;
    ss << program;
    Lexer lexer(ss);
    Token t(tt::Unknown, Position());
    do {
        t = lexer.nextToken();
        tokens.push_back(t.getType());
    } while (t.getType() != tt::EoF);

    EXPECT_EQ(tokens, types);
}

TEST(LexerSuite, simpleSunnyStringEscapeNewline1) {
    std::vector<std::pair<std::string, std::vector<tt>>> test;

    std::string program("printf(\"Hello world!\\n\");");
    std::vector<tt> types = {
            tt::Identifier, tt::OpenParenthesis, tt::StringLiteral, tt::ClosingParenthesis, tt::Semicolon,
            tt::EoF};

    std::vector<tt> tokens;
    std::stringstream ss;
    ss << program;
    Lexer lexer(ss);
    Token t(tt::Unknown, Position());
    do {
        t = lexer.nextToken();
        tokens.push_back(t.getType());
    } while (t.getType() != tt::EoF);

    EXPECT_EQ(tokens, types);
}

TEST(LexerSuite, simpleSunnyStringEscapeNewline2) {
    std::vector<std::pair<std::string, std::vector<tt>>> test;

    std::string program("printf(\"Hello world!\\r\");");
    std::vector<tt> types = {
            tt::Identifier, tt::OpenParenthesis, tt::StringLiteral, tt::ClosingParenthesis, tt::Semicolon,
            tt::EoF};

    std::vector<tt> tokens;
    std::stringstream ss;
    ss << program;
    Lexer lexer(ss);
    Token t(tt::Unknown, Position());
    do {
        t = lexer.nextToken();
        tokens.push_back(t.getType());
    } while (t.getType() != tt::EoF);

    EXPECT_EQ(tokens, types);
}

TEST(LexerSuite, simpleSunnyDictionaryDeclaration) {
    std::vector<std::pair<std::string, std::vector<tt>>> test;

    std::string program("dict(int, float) dictionary;");
    std::vector<tt> types = {
            tt::DictToken, tt::OpenParenthesis, tt::IntegerToken, tt::Comma, tt::FloatToken, tt::ClosingParenthesis,
            tt::Identifier, tt::Semicolon,
            tt::EoF};

    std::vector<tt> tokens;
    std::stringstream ss;
    ss << program;
    Lexer lexer(ss);
    Token t(tt::Unknown, Position());
    do {
        t = lexer.nextToken();
        tokens.push_back(t.getType());
    } while (t.getType() != tt::EoF);

    EXPECT_EQ(tokens, types);
}

TEST(LexerSuite, simpleSunnyDictionarySelect) {
    std::vector<std::pair<std::string, std::vector<tt>>> test;

    std::string program("dictionary.select(n => n * n);");
    std::vector<tt> types = {
            tt::Identifier, tt::Dot, tt::Select, tt::OpenParenthesis, tt::Identifier, tt::Lambda, tt::Identifier,
            tt::Multiply, tt::Identifier, tt::ClosingParenthesis, tt::Semicolon,
            tt::EoF};

    std::vector<tt> tokens;
    std::stringstream ss;
    ss << program;
    Lexer lexer(ss);
    Token t(tt::Unknown, Position());
    do {
        t = lexer.nextToken();
        tokens.push_back(t.getType());
    } while (t.getType() != tt::EoF);

    EXPECT_EQ(tokens, types);
}

TEST(LexerSuite, simpleSunnyDictionaryConstructorFunction) {
    std::vector<std::pair<std::string, std::vector<tt>>> test;

    std::string program("dict(int, float) dictionary(comparator);");
    std::vector<tt> types = {
            tt::DictToken, tt::OpenParenthesis, tt::IntegerToken, tt::Comma, tt::FloatToken, tt::ClosingParenthesis,
            tt::Identifier, tt::OpenParenthesis, tt::Identifier, tt::ClosingParenthesis, tt::Semicolon,
            tt::EoF};

    std::vector<tt> tokens;
    std::stringstream ss;
    ss << program;
    Lexer lexer(ss);
    Token t(tt::Unknown, Position());
    do {
        t = lexer.nextToken();
        tokens.push_back(t.getType());
    } while (t.getType() != tt::EoF);

    EXPECT_EQ(tokens, types);
}

TEST(ParserSuite, simpleSunny) {
    std::string code("int main() {if(val<=val2) { make(); } else { make_else(); }}");

    std::vector<std::string> expected = {"function:main",
                                         "type:INT",
                                         "Body:",
                                         "stmtBlock",
                                         "if:",
                                         "condition:",
                                         "expression",
                                         "leftSide:",
                                         "arithmeticExpr:",
                                         "left term:",
                                         "term:",
                                         "left factor:",
                                         "variableReference:val",
                                         "operator:",
                                         "right factor:",
                                         "operator:",
                                         "right term:",
                                         "operator:",
                                         "rightSide:",
                                         "arithmeticExpr:",
                                         "left term:",
                                         "term:",
                                         "left factor:",
                                         "variableReference:val2",
                                         "operator:",
                                         "right factor:",
                                         "operator:",
                                         "right term:",
                                         "body:",
                                         "stmtBlock",
                                         "FunctionCall:make",
                                         "else:",
                                         "stmtBlock",
                                         "FunctionCall:make_else"};
    std::stringstream ss;
    ss << code;
    Parser parser(ss);
    std::unique_ptr<Nodes::Program> program = std::move(parser.parseProgram());
    parserVisitor tester;
    program->accept(tester);
    auto parsed = tester.getParsed();

    EXPECT_EQ(parsed, expected);
}

TEST(ParserSuite, complexSunny) {
    std::string code(
            "int main(int a, int b)"
            "{"
                "print(70);"
                "if(foo<bar)"
                "{"
                    "call();"
                "}"
                "else"
                "{"
                    "call2();"
                    "call2();"
                    "call2();"
                    "call2();"
                "}"
            "}"

            "int baz=4+4+4;"
            "int qux;"

            "int fun()"
            "{"
                "int main;"
                "int main2;"
                "main();"
                "main();"
                "return 3;"
            "}"
    );

    std::vector<std::string> expected = {"variable:baz",
                                         "type:INT",
                                         "default:",
                                         "arithmeticExpr:",
                                         "left term:",
                                         "term:",
                                         "left factor:",
                                         "number:",
                                         "int:",
                                         "4",
                                         "operator:",
                                         "right factor:",
                                         "operator:",
                                         "right term:",
                                         "arithmeticExpr:",
                                         "left term:",
                                         "term:",
                                         "left factor:",
                                         "number:",
                                         "int:",
                                         "4",
                                         "operator:",
                                         "right factor:",
                                         "operator:",
                                         "right term:",
                                         "arithmeticExpr:",
                                         "left term:",
                                         "term:",
                                         "left factor:",
                                         "number:",
                                         "int:",
                                         "4",
                                         "operator:",
                                         "right factor:",
                                         "operator:",
                                         "right term:",
                                         "variable:qux",
                                         "type:INT",
                                         "function:fun",
                                         "type:INT",
                                         "Body:",
                                         "stmtBlock",
                                         "localVariable:main",
                                         "localVariable:main2",
                                         "FunctionCall:main",
                                         "FunctionCall:main",
                                         "return:",
                                         "arithmeticExpr:",
                                         "left term:",
                                         "term:",
                                         "left factor:",
                                         "number:",
                                         "int:",
                                         "3",
                                         "operator:",
                                         "right factor:",
                                         "operator:",
                                         "right term:",
                                         "function:main",
                                         "type:INT",
                                         "Body:",
                                         "stmtBlock",
                                         "FunctionCall:print",
                                         "arguments:",
                                         "arithmeticExpr:",
                                         "left term:",
                                         "term:",
                                         "left factor:",
                                         "number:",
                                         "int:",
                                         "70",
                                         "operator:",
                                         "right factor:",
                                         "operator:",
                                         "right term:",
                                         "if:",
                                         "condition:",
                                         "expression",
                                         "leftSide:",
                                         "arithmeticExpr:",
                                         "left term:",
                                         "term:",
                                         "left factor:",
                                         "variableReference:foo",
                                         "operator:",
                                         "right factor:",
                                         "operator:",
                                         "right term:",
                                         "operator:",
                                         "rightSide:",
                                         "arithmeticExpr:",
                                         "left term:",
                                         "term:",
                                         "left factor:",
                                         "variableReference:bar",
                                         "operator:",
                                         "right factor:",
                                         "operator:",
                                         "right term:",
                                         "body:",
                                         "stmtBlock",
                                         "FunctionCall:call",
                                         "else:",
                                         "stmtBlock",
                                         "FunctionCall:call2",
                                         "FunctionCall:call2",
                                         "FunctionCall:call2",
                                         "FunctionCall:call2"
    };
    std::stringstream ss;
    ss << code;
    Parser parser(ss);
    std::unique_ptr<Nodes::Program> program = std::move(parser.parseProgram());
    parserVisitor tester;
    program->accept(tester);
    auto parsed = tester.getParsed();

    EXPECT_EQ(parsed, expected);
}

TEST(ParserSuite, simpleInt) {
    std::string code("int main()  { int a = 1; }");

    std::vector<std::string> expected = { "function:main",
                                          "type:INT",
                                          "Body:",
                                          "stmtBlock",
                                          "localVariable:a",
                                          "default:",
                                          "arithmeticExpr:",
                                          "left term:",
                                          "term:",
                                          "left factor:",
                                          "number:",
                                          "int:",
                                          "1",
                                          "operator:",
                                          "right factor:",
                                          "operator:",
                                          "right term:" };
    std::stringstream ss;
    ss << code;
    Parser parser(ss);
    std::unique_ptr<Nodes::Program> program = std::move(parser.parseProgram());
    parserVisitor tester;
    program->accept(tester);
    auto parsed = tester.getParsed();

    EXPECT_EQ(parsed, expected);
}

TEST(ParserSuite, simpleStringLocal) {
    std::string code("int main() { string a = \"Hello\nworld!\"; }");

    std::vector<std::string> expected = { "function:main",
                                          "type:INT",
                                          "Body:",
                                          "stmtBlock",
                                          "localVariable:a",
                                          "default:",
                                          "arithmeticExpr:",
                                          "left term:",
                                          "term:",
                                          "left factor:",
                                          "string:",
                                          "Hello\nworld!",
                                          "operator:",
                                          "right factor:",
                                          "operator:",
                                          "right term:"};
    std::stringstream ss;
    ss << code;
    Parser parser(ss);
    std::unique_ptr<Nodes::Program> program = std::move(parser.parseProgram());
    parserVisitor tester;
    program->accept(tester);
    auto parsed = tester.getParsed();

    EXPECT_EQ(parsed, expected);
}

TEST(ParserSuite, simpleStringGlobal) {
    std::string code("string a = \"Hello\nworld!\";");

    std::vector<std::string> expected = { "variable:a",
                                          "type:STRING",
                                          "default:",
                                          "arithmeticExpr:",
                                          "left term:",
                                          "term:",
                                          "left factor:",
                                          "string:",
                                          "Hello\nworld!",
                                          "operator:",
                                          "right factor:",
                                          "operator:",
                                          "right term:" };
    std::stringstream ss;
    ss << code;
    Parser parser(ss);
    std::unique_ptr<Nodes::Program> program = std::move(parser.parseProgram());
    parserVisitor tester;
    program->accept(tester);
    auto parsed = tester.getParsed();

    EXPECT_EQ(parsed, expected);
}

TEST(ParserSuite, simpleStringsGlobal) {
    std::string code("string a = \"Hello world!\"; string b = \"Goodbye world!\";");

    std::vector<std::string> expected = { "variable:a",
                                          "type:STRING",
                                          "default:",
                                          "arithmeticExpr:",
                                          "left term:",
                                          "term:",
                                          "left factor:",
                                          "string:",
                                          "Hello world!",
                                          "operator:",
                                          "right factor:",
                                          "operator:",
                                          "right term:",
                                          "variable:b",
                                          "type:STRING",
                                          "default:",
                                          "arithmeticExpr:",
                                          "left term:",
                                          "term:",
                                          "left factor:",
                                          "string:",
                                          "Goodbye world!",
                                          "operator:",
                                          "right factor:",
                                          "operator:",
                                          "right term:" };
    std::stringstream ss;
    ss << code;
    Parser parser(ss);
    std::unique_ptr<Nodes::Program> program = std::move(parser.parseProgram());
    parserVisitor tester;
    program->accept(tester);
    auto parsed = tester.getParsed();

    EXPECT_EQ(parsed, expected);
}

TEST(ParserSuite, simpleStringConcatenation) {
    std::string code("string a = \"b\" + \"c\";");

    std::vector<std::string> expected = { "variable:a",
                                          "type:STRING",
                                          "default:",
                                          "arithmeticExpr:",
                                          "left term:",
                                          "term:",
                                          "left factor:",
                                          "string:",
                                          "b",
                                          "operator:",
                                          "right factor:",
                                          "operator:",
                                          "right term:",
                                          "arithmeticExpr:",
                                          "left term:",
                                          "term:",
                                          "left factor:",
                                          "string:",
                                          "c",
                                          "operator:",
                                          "right factor:",
                                          "operator:",
                                          "right term:" };
    std::stringstream ss;
    ss << code;
    Parser parser(ss);
    std::unique_ptr<Nodes::Program> program = std::move(parser.parseProgram());
    parserVisitor tester;
    program->accept(tester);
    auto parsed = tester.getParsed();

    EXPECT_EQ(parsed, expected);
}

// TODO add access member for dictionary: values, keys type

TEST(ParserSuite, simpleSunnyDict) {
    std::string code("dict a = [(int, int) () ()];");

    std::vector<std::string> expected = { "variable:a",
                                          "type:DICT",
                                          "default:",
                                          "dictionary:",
                                          "key type:INT",
                                          "value type:INT" };
    std::stringstream ss;
    ss << code;
    Parser parser(ss);
    std::unique_ptr<Nodes::Program> program = std::move(parser.parseProgram());
    parserVisitor tester;
    program->accept(tester);
    auto parsed = tester.getParsed();

    EXPECT_EQ(parsed, expected);
}

TEST(ParserSuite, simpleSunnyDictDeclarationFromExisting) {
    std::string code("dict a = [(int, int) () ()]; dict b = a;");

    std::vector<std::string> expected = { "variable:a",
                                          "type:DICT",
                                          "default:",
                                          "dictionary:",
                                          "key type:INT",
                                          "value type:INT",
                                          "variable:b",
                                          "type:DICT",
                                          "default:",
                                          "arithmeticExpr:",
                                          "left term:",
                                          "term:",
                                          "left factor:",
                                          "variableReference:a",
                                          "operator:",
                                          "right factor:",
                                          "operator:",
                                          "right term:" };
    std::stringstream ss;
    ss << code;
    Parser parser(ss);
    std::unique_ptr<Nodes::Program> program = std::move(parser.parseProgram());
    parserVisitor tester;
    program->accept(tester);
    auto parsed = tester.getParsed();

    EXPECT_EQ(parsed, expected);
}

TEST(ParserSuite, simpleSunnyDictDeclarationFromExistingWithMemberAccess) {
    std::string code("dict a = [(int, int) ((1, 2)) ()];"
                     " int b = a.at(1);");

    std::vector<std::string> expected = { "variable:a",
                                          "type:DICT",
                                          "default:",
                                          "dictionary:",
                                          "key type:INT",
                                          "number:",
                                          "int:",
                                          "1",
                                          "value type:INT",
                                          "number:",
                                          "int:",
                                          "2",
                                          "variable:b",
                                          "type:INT",
                                          "default:",
                                          "arithmeticExpr:",
                                          "left term:",
                                          "term:",
                                          "left factor:",
                                          "MemberReference",
                                          "object:",
                                          "a",
                                          "member:",
                                          "at",
                                          "type:",
                                          "number:",
                                          "int:",
                                          "1",
                                          "operator:",
                                          "right factor:",
                                          "operator:",
                                          "right term:" };
    std::stringstream ss;
    ss << code;
    Parser parser(ss);
    std::unique_ptr<Nodes::Program> program = std::move(parser.parseProgram());
    parserVisitor tester;
    program->accept(tester);
    auto parsed = tester.getParsed();

    EXPECT_EQ(parsed, expected);
}

TEST(ParserSuite, dictDeclarationWithInit) {
    std::string code("dict a = [(int, int) ((1, 2), (4, 5)) ()];");

    std::vector<std::string> expected = { "variable:a",
                                          "type:DICT",
                                          "default:",
                                          "dictionary:",
                                          "key type:INT",
                                          "number:",
                                          "int:",
                                          "1",
                                          "number:",
                                          "int:",
                                          "4",
                                          "value type:INT",
                                          "number:",
                                          "int:", "2",
                                          "number:",
                                          "int:",
                                          "5" };
    std::stringstream ss;
    ss << code;
    Parser parser(ss);
    std::unique_ptr<Nodes::Program> program = std::move(parser.parseProgram());
    parserVisitor tester;
    program->accept(tester);
    auto parsed = tester.getParsed();

    EXPECT_EQ(parsed, expected);
}

TEST(ParserSuite, dictDeclarationWithDefaultLocal) {
    std::string code("int main() { dict a = [(int, int) ((1, 2), (4, 5)) ()]; }");

    std::vector<std::string> expected = { "function:main",
                                          "type:INT",
                                          "Body:",
                                          "stmtBlock",
                                          "localVariable:a",
                                          "default:",
                                          "dictionary:",
                                          "key type:INT",
                                          "number:",
                                          "int:",
                                          "1",
                                          "number:",
                                          "int:",
                                          "4",
                                          "value type:INT",
                                          "number:",
                                          "int:",
                                          "2",
                                          "number:",
                                          "int:",
                                          "5" };
    std::stringstream ss;
    ss << code;
    Parser parser(ss);
    std::unique_ptr<Nodes::Program> program = std::move(parser.parseProgram());
    parserVisitor tester;
    program->accept(tester);
    auto parsed = tester.getParsed();

    EXPECT_EQ(parsed, expected);
}

TEST(ParserSuiteClassCheck, dictDeclarationWithDefaultLocal) {
    std::string code("int main() { dict a = [(int, int) ((1, 2), (4, 5)) ()]; }");

    std::string expected = {"Node name: Function declaration"
                               "Node name: Statement block"
                               "Node name: Local variable declaration"};
    std::stringstream ss;
    ss << code;
    Parser parser(ss);
    std::unique_ptr<Nodes::Program> program = std::move(parser.parseProgram());

    testing::internal::CaptureStdout();
    auto const & functions = program->getFunctions().at("main");
    std::cout << functions;
    auto const & body = functions->getFunctionBody();
    std::cout << body;
    auto const & stmtBlock = body->getStatements();
    for (auto const & iter : stmtBlock) {
        std::cout << iter;
    }
    std::string output = testing::internal::GetCapturedStdout();

    EXPECT_EQ(output, expected);
}

TEST(ParserSuiteClassCheck, dictDeclarationWithDefault) {
    std::string code("dict a = [(int, int) ((1, 2), (4, 5)) ()];");

    std::string expected = {"Node name: Declaration"};
    std::stringstream ss;
    ss << code;
    Parser parser(ss);
    std::unique_ptr<Nodes::Program> program = std::move(parser.parseProgram());
    int a = 1;
    testing::internal::CaptureStdout();
    auto const & variables = program->getVariables().at("a");
    std::cout << variables;

    std::string output = testing::internal::GetCapturedStdout();

    EXPECT_EQ(output, expected);
}

TEST(InterpreterSuite, simplePrint) {
    std::pair<std::string, std::vector<std::string>> code = {"int main(){ print(10,11,12,13);}",{"10","11","12","13"}};
    std::stringstream redirected;
    auto prev = std::cout.rdbuf(redirected.rdbuf());

    std::stringstream ss;
    ss << code.first;
    SemanticAnalyzerVisitor semanticAnalyzerVisitor;
    InterpreterVisitor interpreterVisitor;
    Parser parser(ss);
    std::unique_ptr<Nodes::Program> program = parser.parseProgram();
    program->accept(semanticAnalyzerVisitor);
    program->accept(interpreterVisitor);

    std::string output;
    std::vector<std::string> compared;

    while (std::getline(redirected, output)) {
        compared.push_back(output);
    }
    EXPECT_EQ(compared, code.second);
}

TEST(InterpreterSuite, simplePrintVar) {
    std::pair<std::string, std::vector<std::string>> code = {"int main(){ int a=5;float b=10.5; print(a,b,b,b,a,b);}",{"5","10.5","10.5","10.5","5","10.5",}};
    std::stringstream redirected;
    auto prev = std::cout.rdbuf(redirected.rdbuf());

    std::stringstream ss;
    ss << code.first;
    SemanticAnalyzerVisitor semanticAnalyzerVisitor;
    InterpreterVisitor interpreterVisitor;
    Parser parser(ss);
    std::unique_ptr<Nodes::Program> program = parser.parseProgram();
    program->accept(semanticAnalyzerVisitor);
    program->accept(interpreterVisitor);

    std::string output;
    std::vector<std::string> compared;

    while (std::getline(redirected, output)) {
        compared.push_back(output);
    }
    EXPECT_EQ(compared, code.second);
}

TEST(InterpreterSuite, simplePrintWhile) {
    std::pair<std::string, std::vector<std::string>> code = {"int main(){int n=6; while(n>0){print(n); n=n-1;} }",{"6","5","4","3","2","1"}};
    std::stringstream redirected;
    auto prev = std::cout.rdbuf(redirected.rdbuf());

    std::stringstream ss;
    ss << code.first;
    SemanticAnalyzerVisitor semanticAnalyzerVisitor;
    InterpreterVisitor interpreterVisitor;
    Parser parser(ss);
    std::unique_ptr<Nodes::Program> program = parser.parseProgram();
    program->accept(semanticAnalyzerVisitor);
    program->accept(interpreterVisitor);

    std::string output;
    std::vector<std::string> compared;

    while (std::getline(redirected, output)) {
        compared.push_back(output);
    }
    EXPECT_EQ(compared, code.second);
}

TEST(InterpreterSuite, simplePrintRecursion) {
    std::pair<std::string, std::vector<std::string>> code = {"int func(int n){print(n); if(n>0){func(n-1);}} int main(){func(6);}",{"6","5","4","3","2","1","0"}};
    std::stringstream redirected;
    auto prev = std::cout.rdbuf(redirected.rdbuf());

    std::stringstream ss;
    ss << code.first;
    SemanticAnalyzerVisitor semanticAnalyzerVisitor;
    InterpreterVisitor interpreterVisitor;
    Parser parser(ss);
    std::unique_ptr<Nodes::Program> program = parser.parseProgram();
    program->accept(semanticAnalyzerVisitor);
    program->accept(interpreterVisitor);

    std::string output;
    std::vector<std::string> compared;

    while (std::getline(redirected, output)) {
        compared.push_back(output);
    }
    EXPECT_EQ(compared, code.second);
}

TEST(InterpreterSuite, simpleIfElse) {
    std::pair<std::string, std::vector<std::string>> code = {"float cond(){if(4>5){return 2.5;} else {return 7.5;} print(100);} int main(){print(cond());}",{"7.5"}};
    std::stringstream redirected;
    auto prev = std::cout.rdbuf(redirected.rdbuf());

    std::stringstream ss;
    ss << code.first;
    SemanticAnalyzerVisitor semanticAnalyzerVisitor;
    InterpreterVisitor interpreterVisitor;
    Parser parser(ss);
    std::unique_ptr<Nodes::Program> program = parser.parseProgram();
    program->accept(semanticAnalyzerVisitor);
    program->accept(interpreterVisitor);

    std::string output;
    std::vector<std::string> compared;

    while (std::getline(redirected, output)) {
        compared.push_back(output);
    }
    EXPECT_EQ(compared, code.second);
}

TEST(InterpreterSuite, simpleModifyingGlobalVariables) {
    std::pair<std::string, std::vector<std::string>> code = {"int a; int b; int c; int main(){a=4;b=100;c=50; print(a,b,c);}",{"4","100","50"}};
    std::stringstream redirected;
    auto prev = std::cout.rdbuf(redirected.rdbuf());

    std::stringstream ss;
    ss << code.first;
    SemanticAnalyzerVisitor semanticAnalyzerVisitor;
    InterpreterVisitor interpreterVisitor;
    Parser parser(ss);
    std::unique_ptr<Nodes::Program> program = parser.parseProgram();
    program->accept(semanticAnalyzerVisitor);
    program->accept(interpreterVisitor);

    std::string output;
    std::vector<std::string> compared;

    while (std::getline(redirected, output)) {
        compared.push_back(output);
    }
    EXPECT_EQ(compared, code.second);
}

TEST(InterpreterSuite, simpleString) {
    std::pair<std::string, std::vector<std::string>> code = {"int main(){ string a = \"Hello world!\"; print(a);}",{"Hello world!"}};
    std::stringstream redirected;
    auto prev = std::cout.rdbuf(redirected.rdbuf());

    std::stringstream ss;
    ss << code.first;
    SemanticAnalyzerVisitor semanticAnalyzerVisitor;
    InterpreterVisitor interpreterVisitor;
    Parser parser(ss);
    std::unique_ptr<Nodes::Program> program = parser.parseProgram();
    program->accept(semanticAnalyzerVisitor);
    program->accept(interpreterVisitor);

    std::string output;
    std::vector<std::string> compared;

    while (std::getline(redirected, output)) {
        compared.push_back(output);
    }
    EXPECT_EQ(compared, code.second);
}

TEST(InterpreterSuite, simpleStringConcatenation) {
    std::pair<std::string, std::vector<std::string>> code = {"int main(){ string a = \"H\"+\"e\"+\"l\"+\"l\"+\"o\"; print(a);}",{"Hello"}};
    std::stringstream redirected;
    auto prev = std::cout.rdbuf(redirected.rdbuf());

    std::stringstream ss;
    ss << code.first;
    SemanticAnalyzerVisitor semanticAnalyzerVisitor;
    InterpreterVisitor interpreterVisitor;
    Parser parser(ss);
    std::unique_ptr<Nodes::Program> program = parser.parseProgram();
    program->accept(semanticAnalyzerVisitor);
    program->accept(interpreterVisitor);

    std::string output;
    std::vector<std::string> compared;

    while (std::getline(redirected, output)) {
        compared.push_back(output);
    }
    EXPECT_EQ(compared, code.second);
}