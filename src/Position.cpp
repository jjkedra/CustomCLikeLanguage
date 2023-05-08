//
// Created by Jan Kędra on 15/04/2023.
//

#include "../include/Position.h"

Position::Position() = default;

Position::Position(std::istream &sourceCode) {
    codeStream_ = &sourceCode;
}

std::pair<unsigned int, unsigned int> Position::getPosition() {
    return std::make_pair(column_, line_);
}

unsigned int Position::getLine() const {
    return line_;
}

unsigned int Position::getColumn() const {
    return column_;
}

std::streampos Position::getByOffset() {
    return currPos_;
}

void Position::move(char c) {
    if (!c || c == EOF)
    {
        return;
    }
    else if (c == '\n' || c == '\r')
    {
       line_ += 1;
       column_ += 1;
    }
    else {
        column_ += 1;
    }
}

/**
 * Use tellg to do what it does which is "Returns the position of the current character in the input stream"
 */
void Position::savePosition() {
    currPos_ = codeStream_->tellg();
}
