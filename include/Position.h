//
// Created by Jan Kędra on 15/04/2023.
//

#ifndef LEXER_POSITION_H
#define LEXER_POSITION_H


#include <utility>
#include <istream>
#include <memory>
#include <variant>
#include <vector>

/**
 * @brief Position is a class for storing the position of a character in a file.
 * It contains functions for returning the line and column of the character.
 * The position is used by the lexer to return the position of a token.
 *
 * @note This class is part of a larger system for parsing and interpreting a programming language.
 *
 * @see Lexer, Parser, AstVisitor, Tree.h
 */

class Position
{
public:
    // Constructors
    Position();
    explicit Position(std::istream &sourceCode);
    // Methods
    std::pair<unsigned int, unsigned int> getPosition();
    unsigned int getLine() const;
    unsigned int getColumn() const;
    std::streampos getByOffset();
    void move(char c);
    void savePosition();
private:
    std::istream *codeStream_{};
    std::streampos currPos_;
    unsigned int column_ = 0;
    unsigned int line_ = 1;
};


#endif //LEXER_POSITION_H
