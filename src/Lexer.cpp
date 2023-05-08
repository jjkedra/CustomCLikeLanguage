//
// Created by Jan Kędra on 14/04/2023.
//

#include <map>
#include <cmath>
#include "../include/Lexer.h"
#include "../include/LangException.h"
#include "../util/utils.h"

Lexer::Lexer(std::istream &sourceCode) {
    codeStream_ = &sourceCode;
    currPosition_ = Position(sourceCode);
}

Token Lexer::nextToken()
{
    while (isspace(currChar_)) {
        nextChar();
    }
    tokenStartPosition_ = currPosition_;
    tokenStartPosition_.savePosition();

    if (!noSkipToken()) {
        skipToken();
        currToken_ = Token(TokenType::Unknown, tokenStartPosition_);
    }

    return currToken_;
}


/**
 * Gets next character from the istream if possible (no eof or smth)
 *
 * @return true when gets new character
 */
bool Lexer::nextChar()
{
    currPosition_.move(currChar_);
    currChar_ = codeStream_->get();
    if (currChar_ == EOF) {
        return false;
    }
    return true;
}

bool Lexer::match(char c) {
    if (currChar_ == c) {
        nextChar();
        return true;
    }
    return false;
}

bool Lexer::checkEoF() {
    if (currChar_ == EOF) {
        currToken_ = Token(TokenType::EoF, tokenStartPosition_);
        return true;
    }
    return false;
}

bool Lexer::checkCharToTT() {
    if (charToTT.find(currChar_) == charToTT.end()) {
        return false;
    }
    currToken_ = Token(charToTT[currChar_], tokenStartPosition_);
    nextChar();
    return true;
}

bool Lexer::checkRelational() {
    if (match('>')) {
        if (match('=')) {
            currToken_ = Token(TokenType::GreaterEqual, tokenStartPosition_);
        } else {
            currToken_ = Token(TokenType::Greater, tokenStartPosition_);
        }
        return true;
    } else if (match('<')) {
        if (match('=')) {
            currToken_ = Token(TokenType::LessEqual, tokenStartPosition_);
        } else {
            currToken_ = Token(TokenType::Less, tokenStartPosition_);
        }
        return true;
    } else if (match('!')) {
        if (match('=')) {
            currToken_ = Token(TokenType::NotEqual, tokenStartPosition_);
            return true;
        }
    } else if (match('=')) {
        if (match('=')) {
            currToken_ = Token(TokenType::Equal, tokenStartPosition_);
        } else if (match('>')) {
            currToken_ = Token(TokenType::Lambda, tokenStartPosition_);
        } else {
            currToken_ = Token(TokenType::Assign, tokenStartPosition_);
        }
        return true;
    }
    return false;
}

bool Lexer::checkString()
{
    if (match('"')) {
        std::vector<char> str;
        str.push_back('"');
        int str_len = 1;

        do {
            str.push_back(currChar_);
            nextChar();
            str_len++;
            if (str_len > MAX_STR_LEN && currChar_ == EOF) {
                return false;
            }
        } while (currChar_ != '"');

        str.push_back('"');
        nextChar();

        std::string buildStr = buildString(str, '"', '"');
        currToken_ = Token(TokenType::StringLiteral, tokenStartPosition_, buildStr);
        return true;
    }
    return false;
}

bool Lexer::checkCommentOrMath()
{
    if (match('+')) {
        currToken_ = Token(TokenType::Plus, tokenStartPosition_);
        return true;
    } else if (match('-')) {
        currToken_ = Token(TokenType::Minus, tokenStartPosition_);
        return true;
    } else if (match('*')) {
        currToken_ = Token(TokenType::Multiply, tokenStartPosition_);
        return true;
    } else if (match('%')) {
        currToken_ = Token(TokenType::Mod, tokenStartPosition_);
        return true;
    } else if (match('/')) {
        if (match('/')) {
            // Until end of comment we go
            std::vector<char> comment;
            while ((currChar_ != '\n' && currChar_ != '\r') && nextChar())
            {
                comment.push_back(currChar_);
            }
            std::string commentBuild = buildString(comment);
            currToken_ = Token(TokenType::Comment, tokenStartPosition_, commentBuild);
            return true;
        }
        currToken_ = Token(TokenType::Divide, tokenStartPosition_);
        return true;
    }
    return false;
}

bool Lexer::checkKeywordOrIdentifier()
{
    std::vector<char> lexeme;

    if (!isalpha(currChar_)) {
        return false;
    } else {
        lexeme.push_back(currChar_);
        nextChar();
        while (isalnum(currChar_) || currChar_ == '_') {
            if ((lexeme.size() + 1) > MAX_STR_LEN) {
                char errorMsg[MAX_ERROR_MSG_LEN] = {};
                snprintf(errorMsg, MAX_ERROR_MSG_LEN, "ERROR: EXCEEDED MAX IDENTIFIER LENGTH");
                throw LangException(errorMsg, tokenStartPosition_);
            }
            lexeme.push_back(currChar_);
            nextChar();
        }
    }
    std::string lexemeBuilt = buildString(lexeme);
    if (stringToTT.find(lexemeBuilt) == stringToTT.end()) {
        currToken_ = Token(TokenType::Identifier, tokenStartPosition_, lexemeBuilt);
    } else {
        currToken_ = Token(stringToTT[lexemeBuilt], tokenStartPosition_);
    }
    return true;
}

bool Lexer::checkNumber()
{
    if (!isdigit(currChar_)) {
        return false;
    }
    // ASCII number - '0' = number
    int val = currChar_ - '0';
    int digitCounter = 1;
    // Need to check zero first - special case
    if (match('0')) {
        if (isdigit(currChar_)) {
            char errorMsg[MAX_ERROR_MSG_LEN] = {};
            snprintf(errorMsg, MAX_ERROR_MSG_LEN, "ERROR: NUMBER CANNOT START WITH ZERO");
            throw LangException(errorMsg, tokenStartPosition_);
        } else {
            currToken_ = Token(TokenType::IntegerLiteral, tokenStartPosition_, 0);
        }
    }
        // Number multiple > 9
    else {
        nextChar();
        while (isdigit(currChar_)) {
            if (digitCounter > MAX_DIGITS) {
                char errorMsg[MAX_ERROR_MSG_LEN] = {};
                snprintf(errorMsg, MAX_ERROR_MSG_LEN, "ERROR: NUMBER TOO BIG");
                throw LangException(errorMsg, tokenStartPosition_);
            }
            // Check for overflow using util function, need to cast beforehand - otherwise val becomes literal
            checkOverflow(static_cast<long long>(val) * 10, currChar_ - '0');
            val = val * 10 + (currChar_ - '0');
            nextChar();
            digitCounter++;
        }
    }
    // float
    if (match('.')) {
        int exponent = 1;
        if (!isdigit(currChar_)) {
            currToken_ = Token(TokenType::FloatLiteral,tokenStartPosition_, static_cast<float>(val));
            return true;
        } else {
            int fractionPart = currChar_ - '0';
            nextChar();
            digitCounter++;
            while(isdigit(currChar_) && digitCounter < MAX_DIGITS) {
                fractionPart = fractionPart * 10 + (currChar_ - '0');
                nextChar();
                digitCounter++;
                exponent++;
            }
            if (digitCounter > MAX_DIGITS) {
                char errorMsg[MAX_ERROR_MSG_LEN] = {};
                snprintf(errorMsg, MAX_ERROR_MSG_LEN, "ERROR: NUMBER TOO BIG OCCURRED");
                throw LangException(errorMsg, tokenStartPosition_);
            }
            float fraction = fractionPart / (pow(10, exponent));
            currToken_ = Token(TokenType::FloatLiteral, tokenStartPosition_, val + fraction);
            return true;
        }
    }
    currToken_ = Token(TokenType::IntegerLiteral, tokenStartPosition_, val);
    return true;
}

/**
 * Returns true if the token is one of the identifiable types
 * as well as sets the token to that type
 *
 * @return true if token identifiable
 */
bool Lexer::noSkipToken()
{
    return checkEoF() ||
            checkCharToTT() ||
            checkNumber() ||
            checkRelational() ||
            checkString() ||
            checkCommentOrMath() ||
            checkKeywordOrIdentifier();
}

/**
 * Iterates over unidentifiable token until blank space is found
 */
void Lexer::skipToken()
{
    while (!isspace(currChar_) && nextChar()) {
        // Iterating moment
    }
}