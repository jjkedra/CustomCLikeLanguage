//
// Created by Jan Kędra on 18/04/2023.
//

#ifndef LEXER_UTILS_H
#define LEXER_UTILS_H

#include <vector>
/**
 * Function throws overflow error when trying to add numbers resultin in >INT_MAX result
 * @param number to be added to
 * @param number we add
 */
extern void checkOverflow(long long a, int b);

extern std::string buildString(const std::vector<char> &input, char startSign = '\0', char endSign = '\0');

#endif //LEXER_UTILS_H
