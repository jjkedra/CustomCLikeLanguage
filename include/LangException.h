//
// Created by Jan Kędra on 15/04/2023.
//

#ifndef LEXER_LANGEXCEPTION_H
#define LEXER_LANGEXCEPTION_H


#include <stdexcept>
#include "Position.h"

class LangException : public std::runtime_error
{
public:
    explicit LangException(std::string errorMsg, Position pos) : std::runtime_error(what2(std::move(errorMsg), pos, true))
    {
        position_ = pos;
        positional_ = true;
    }
    explicit LangException(const std::string &errorMsg) : std::runtime_error(what2(std::move(errorMsg))) {}

    std::string what2(std::string errorMsg, Position position_ = {}, bool positional_ = {}) const
    {
        if (positional_) {
            std::string error;
            error += errorMsg;
            error += "\n\tAT LINE:";
            error += std::to_string(position_.getLine());
            error += " COLUMN: ";
            error += std::to_string(position_.getColumn());
            return error;
        }
        return errorMsg;
    }
private:
    Position position_;
    bool positional_ = false;
};


#endif //LEXER_LANGEXCEPTION_H
