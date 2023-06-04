//
// Created by Jan Kędra on 02/06/2023.
//

#ifndef LEXER_SYMBOLMANAGER_H
#define LEXER_SYMBOLMANAGER_H

#include "SymbolTable.h"

class SymbolManager {
private:
    std::vector<std::vector<SymbolTable>> contextTable_;

public:
    std::optional<SymbolTable::Symbol> getSymbol(const std::string&, bool);
    bool insertSymbol(const std::string&, SymbolTable::Symbol);
    void setValue(const std::string&, std::variant<int, float, std::string>);

    void enterNewScope();
    bool exitScope();

    void enterNewContext();
    bool exitContext();

    bool isGlobal(const std::string&);
};


#endif //LEXER_SYMBOLMANAGER_H
