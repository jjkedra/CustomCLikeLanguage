//
// Created by Jan Kędra on 14/04/2023.
//

#ifndef LEXER_LEXER_H
#define LEXER_LEXER_H

#define MAX_STR_LEN                     1000
#define MAX_DIGITS                      50
#define MAX_ERROR_MSG_LEN               100

#include <istream>
#include "Token.h"
#include "Position.h"

/**
 * @brief Lexer is a class for lexical analysis of a programming language.
 * It contains functions for reading the source code and returning tokens.
 * The tokens are used by the parser to build an abstract syntax tree.
 * The lexer also contains functions for skipping whitespace and comments.
 * The lexer is implemented as a finite state machine.
 * The states are represented by the functions in the class.
 *
 * @note This class is part of a larger system for parsing and interpreting a programming language.
 *
 * @see Token, Position, Parser, AstVisitor, Tree.h
 */

class Lexer
{
public:
    explicit Lexer(std::istream &sourceCode);
    Token nextToken();
private:
    // Members
    std::istream *codeStream_;
    char currChar_ = ' ';
    Position currPosition_;
    Position tokenStartPosition_;
    Token currToken_ = Token(TokenType::Unknown, Position());
    // Basic
    bool nextChar();
    bool match(char c);
    // Complex
    bool checkEoF();
    bool checkCharToTT();

    bool checkRelational();
    bool checkString();
    bool checkCommentOrMath();
    bool checkKeywordOrIdentifier();

    bool checkNumber();
    // Helper functions
    bool noSkipToken();
    void skipToken();
    // Token map
    std::map<char, TokenType> charToTT {
            {';', TokenType::Semicolon},
            {',', TokenType::Comma},
            {'.', TokenType::Dot},
            // Bracket
            {'(', TokenType::OpenParenthesis},
            {')', TokenType::ClosingParenthesis},
            {'{', TokenType::OpenBracket},
            {'}', TokenType::ClosingBracket},
            {'[', TokenType::OpenSquareBracket},
            {']', TokenType::ClosingSquareBracket},
    };

    std::map<std::string, TokenType> stringToTT {
            // Keyword or identifier
            {"int", TokenType::IntegerToken},
            {"float", TokenType::FloatToken},
            {"string", TokenType::StringToken},
            {"dict", TokenType::DictToken},
            {"if", TokenType::If},
            {"else", TokenType::Else},
            {"while", TokenType::While},
            {"return", TokenType::Return},
            // LINQ
            {"select", TokenType::Select},
            {"where", TokenType::Where}
    };
};


#endif //LEXER_LEXER_H
