//
// Created by Jan Kędra on 14/04/2023.
//

#ifndef LEXER_TOKEN_H
#define LEXER_TOKEN_H

#include <istream>
#include <map>
#include <variant>
#include "Position.h"

/**
 * @brief TokenType is an enum class for storing the type of a token.
 * It contains all the types of tokens that can be found in the language.
 *
 * @note This class is part of a larger system for parsing and interpreting a programming language.
 *
 * @see Lexer, Parser, AstVisitor, Tree.h
 */

enum class TokenType
{
    // TOKENS
    Identifier,          // identifier
    IntegerToken,
    FloatToken,
    StringToken,
    // Dictionary
    DictToken,
    Comment,

    // ACCESS
    Assign,             // =

    // COMPARISON
    Equal,              // ==
    NotEqual,           // !=

    // LINQ
    Lambda,             // =>

    Greater,            // >
    GreaterEqual,       // >=
    Less,               // <
    LessEqual,          // <=

    // MATHEMATICAL
    Plus,               // +
    Minus,              // -
    Multiply,           // *
    Divide,             // /
    Mod,                // %

    // SYMBOLS
    OpenParenthesis,    // (
    ClosingParenthesis, // )
    OpenBracket,        // {
    ClosingBracket,     // }
    Semicolon,          // ;
    Comma,              // ,
    Dot,                // .

    If,                 // if
    Else,               // else
    While,              // while

    Return,             // return

    // LINQ
    Select,
    Where,

    IntegerLiteral,     // 0-9
    FloatLiteral,       // 1.23 9.99
    StringLiteral,

    Unknown,
    EoF
};

class Token
{
public:
    // Constructors
    Token(TokenType, Position);

    Token(TokenType, Position, const std::string&);
    Token(TokenType, Position, int);
    Token(TokenType, Position, float);
    // Define value container, which holds the token value
    typedef std::variant<int, float, std::string> valueContainer;
    // Getters
    TokenType getType() const;
    Position getPosition();
    valueContainer getValue();
private:
    TokenType type_;
    Position position_;
    valueContainer tokenValue_;
};


#endif //LEXER_TOKEN_H
