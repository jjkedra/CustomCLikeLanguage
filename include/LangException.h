//
// Created by Jan Kędra on 15/04/2023.
//

#ifndef LEXER_LANGEXCEPTION_H
#define LEXER_LANGEXCEPTION_H


#include <stdexcept>
#include "Position.h"

/**
 * @brief LangException is a class for throwing exceptions in the interpreter.
 * It contains functions for returning the error message and the position of the error.
 * The error message is used by the interpreter to print the error message.
 *
 * @note This class is part of a larger system for parsing and interpreting a programming language.
 *
 * @see Lexer, Parser, AstVisitor, Tree.h
 */

class LangException : public std::runtime_error
{
public:
    explicit LangException(std::string errorMsg, Position pos, std::string fun = "", unsigned int line = 0) : std::runtime_error(what2(std::move(errorMsg), pos, fun, line, true))
    {
        position_ = pos;
        positional_ = true;
        fun_ = fun;
        line_ = line;
    }
    explicit LangException(const std::string &errorMsg) : std::runtime_error(what2(std::move(errorMsg))) {}

    std::string what2(std::string errorMsg, Position position_ = {}, std::string fun = "", unsigned int lane=0, bool positional_ = {}) const
    {
        if (positional_) {
            std::string error;
            error += errorMsg;
            error += "\n\tAT LINE:";
            error += std::to_string(position_.getLine());
            error += " COLUMN: ";
            error += std::to_string(position_.getColumn());
            error += "\n\tIN FUNCTION: ";
            error += fun;
            error += "\n\tAT LINE: ";
            error += std::to_string(lane);

            return error;
        }
        return errorMsg;
    }
private:
    Position position_;
    bool positional_ = false;
    std::string fun_;
    unsigned int line_;
};


#endif //LEXER_LANGEXCEPTION_H
