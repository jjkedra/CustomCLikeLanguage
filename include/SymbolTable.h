//
// Created by Jan Kędra on 02/06/2023.
//

#ifndef LEXER_SYMBOLTABLE_H
#define LEXER_SYMBOLTABLE_H

#include "Tree.h"

class SymbolTable {
public:
    class Symbol
    {
    private:
        idType type_;
        bool isFunction_;
        std::optional<std::variant<int, float, std::string>> value_;
        std::optional<Nodes::FunctionDeclaration*> funPointer_;
    public:
        Symbol(bool, idType, std::variant<std::variant<int, float, std::string>, Nodes::FunctionDeclaration*>);

        std::optional<Nodes::FunctionDeclaration*> getFunPointer() { return funPointer_.value(); }
        idType getType() { return type_; }
        bool isFunction() const { return isFunction_; }
        bool hasValue() { return value_.has_value(); }
        void setValue(std::variant<int, float, std::string> val) { this->value_ = val; }
        std::variant<int, float, std::string> getValue() { return value_.value(); }
    };

    SymbolTable();
    bool insert(const std::string&, Symbol);
    std::optional<Symbol> getSymbol(const std::string&);
    bool setValue(const std::string&, std::variant<int, float, std::string> newVal);
private:
    std::map<std::string, Symbol> table_;
};


#endif //LEXER_SYMBOLTABLE_H
