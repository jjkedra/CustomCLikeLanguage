//
// Created by Jan Kędra on 14/04/2023.
//

#include "../include/Token.h"

Token::Token(TokenType type, Position pos) {
    type_ = type;
    Position pos_ = pos;
}

Token::Token(TokenType type, Position pos, const std::string& lexeme) {
    type_ = type;
    position_ = pos;
    tokenValue_.emplace<std::string>(lexeme);
}

Token::Token(TokenType type, Position pos, int val) {
    type_ = type;
    position_ = pos;
    tokenValue_.emplace<int>(val);
}

Token::Token(TokenType type, Position pos, float val) {
    type_ = type;
    position_ = pos;
    tokenValue_.emplace<float>(val);
}

TokenType Token::getType() const {
    return type_;
}

Position Token::getPosition() {
    return position_;
}

Token::valueContainer Token::getValue() {
    return tokenValue_;
}

