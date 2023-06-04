//
// Created by Jan Kędra on 02/06/2023.
//

#include "SymbolTable.h"

SymbolTable::Symbol::Symbol(bool isFun, idType type, std::variant<std::variant<int, float, std::string>, Nodes::FunctionDeclaration*> val)
:type_(type), isFunction_(isFun) {
    if (isFun) {
        funPointer_ = std::get<Nodes::FunctionDeclaration*>(val);
    } else {
        value_ = std::get<std::variant<int, float, std::string>>(val);
    }
}

SymbolTable::SymbolTable() {
    table_ = {};
}

std::optional<SymbolTable::Symbol> SymbolTable::getSymbol(const std::string &id) {
    auto found = table_.find(id);
    if (found == table_.end()) {
        return std::nullopt;
    }
    return found->second;
}

bool SymbolTable::insert(const std::string &id, SymbolTable::Symbol symbol) {
    if (table_.find(id) != table_.end()) {
        return false;
    }
    table_.insert(std::make_pair(id, symbol));
    return true;
}

bool SymbolTable::setValue(const std::string &id, std::variant<int, float, std::string> newVal) {
    auto found = table_.find(id);
    if (found == table_.end()) {
        return false;
    }
    found->second.setValue(newVal);
    return true;
}

