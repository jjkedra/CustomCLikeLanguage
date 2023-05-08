//
// Created by Jan Kędra on 15/04/2023.
//

#ifndef LEXER_POSITION_H
#define LEXER_POSITION_H


#include <utility>
#include <istream>

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
