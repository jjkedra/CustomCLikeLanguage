//
// Created by Jan Kędra on 18/04/2023.
//

#include <climits>
#include <stdexcept>
#include <sstream>
#include "utils.h"

void checkOverflow(long long a, int b)
{
    long long c = a + b;
    if (c < INT_MIN || c > INT_MAX) {
        throw std::overflow_error("Integer overflow detected");
    }
}

std::string buildString(const std::vector<char>& input, char startSign, char endSign)
{
    std::vector<char> output;

    bool inEscape = false;
    bool inString = false;

    std::size_t i = 0;

    if (startSign != '\0') {
        if (!input.empty() && input[0] == startSign) {
            inString = true;
            i++;
        } else {
            // throw an exception or return an empty string to indicate that the input is invalid
        }
    } else {
        inString = true;
    }

    for (; i < input.size(); ++i) {
        if (inString) {
            if (input[i] == '\\') {
                inEscape = true;
            } else if (input[i] == endSign) {
                inString = false;
                break;
            } else {
                if (inEscape) {
                    switch (input[i]) {
                        case 'n':
                            output.push_back('\n');
                            break;
                        case 't':
                            output.push_back('\t');
                            break;
                            // add more cases for other escape characters as needed
                        default:
                            output.push_back(input[i]);
                            break;
                    }
                    inEscape = false;
                } else {
                    output.push_back(input[i]);
                }
            }
        } else if (input[i] == startSign) {
            inString = true;
        }
    }

    if (inString && endSign != '\0') {
        // throw an exception or return an empty string to indicate that the input is invalid
    }

    return {output.begin(), output.end()};
}
