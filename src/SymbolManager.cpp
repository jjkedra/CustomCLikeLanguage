//
// Created by Jan Kędra on 02/06/2023.
//

#include "SymbolManager.h"

std::optional<SymbolTable::Symbol> SymbolManager::getSymbol(const std::string &id, bool isFun=false) {
    if (isFun) {
        auto symbol = contextTable_.front().front().getSymbol(id);
        return symbol;
    } else {
        if (contextTable_.back().size() > 0) {
            for (int i = contextTable_.back().size() - 1; i >= 0; i--) {
                if (contextTable_.back()[i].getSymbol(id).has_value()) {
                    return contextTable_.back()[i].getSymbol(id);
                }
            }
        }
        if (contextTable_.front().front().getSymbol(id).has_value()) {
            return contextTable_.front().front().getSymbol(id);
        }
        return std::nullopt;
    }
}

bool SymbolManager::insertSymbol(const std::string & id, SymbolTable::Symbol symbol) {
    return contextTable_.back().back().insert(id, symbol);
}

void SymbolManager::setValue(const std::string &id, std::variant<int, float, std::string> newVal) {
    for (int i = contextTable_.back().size() - 1; i >= 0; i--) {
        auto val = contextTable_.back()[i].getSymbol(id);
        if (val.has_value()) {
            contextTable_.back()[i].setValue(id, newVal);
            return;
        }
    }
    if (contextTable_.front().front().getSymbol(id)->hasValue()) {
        contextTable_.front().front().setValue(id, newVal);
    }
}

void SymbolManager::enterNewScope() {
    contextTable_.back().emplace_back();
}

bool SymbolManager::exitScope() {
    if (contextTable_.back().size() != 0) {
        contextTable_.back().pop_back();
        return true;
    }
    return false;
}

void SymbolManager::enterNewContext() {
    contextTable_.emplace_back();
}

bool SymbolManager::exitContext() {
    if (contextTable_.size() != 0) {
        contextTable_.pop_back();
        return true;
    }
    return false;
}

bool SymbolManager::isGlobal(const std::string &id) {
    if (contextTable_.size() > 1) {
        for (int i = contextTable_.back().size() - 1; i >= 0; i--) {
            if (contextTable_.back()[i].getSymbol(id).has_value()) {
                return false;
            }
        }
    }
    if (contextTable_.front()[0].getSymbol(id).has_value()) {
        return true;
    }
    return false;
}