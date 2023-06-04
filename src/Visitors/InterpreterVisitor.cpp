//
// Created by Jan Kędra on 02/06/2023.
//

#include <iostream>
#include "InterpreterVisitor.h"

void InterpreterVisitor::visitProgram(Nodes::Program *program) {
    symbolManager_.enterNewContext();
    symbolManager_.enterNewScope();

    for (const auto & iter : program->getVariables()) {
        iter.second->accept(*this);
    }

    for (const auto & iter : program->getFunctions()) {
        symbolManager_.insertSymbol(iter.first, SymbolTable::Symbol(true, iter.second->getType(), iter.second.get()));
    }

    program->getFunctions().find("main")->second->accept(*this);
    symbolManager_.exitContext();
}

void InterpreterVisitor::visitNumber(Nodes::Number *number) {
    if (number->getType() == idType::INT) {
        int value = std::get<int>(number->getValue());
        currentValue_ = value;
    }
    if (number->getType() == idType::FLOAT) {
        float value = std::get<float>(number->getValue());
        currentValue_ = value;
    }
}

void InterpreterVisitor::visitString(Nodes::String *string) {
    currentValue_ = string->getValue();
}

void InterpreterVisitor::visitTerm(Nodes::Term *term) {
    term->acceptLeft(*this);
    std::variant<int, float, std::string> left = currentValue_;

    if (term->getOperator()) {
        factorOperator fop = term->getOperator()->getType();
        term->acceptRight(*this);

        if (expectedType_ == INT) {
            if (fop == factorOperator::MULTIPLY) {
                int value = std::get<int>(left) * std::get<int>(currentValue_);
                currentValue_ = value;
            } else {
                int value = std::get<int>(left) / std::get<int>(currentValue_);
                currentValue_ = value;
            }
        } else if (expectedType_ == FLOAT) {
            if (fop == factorOperator::MULTIPLY) {
                float value = std::get<float>(left) * std::get<float>(currentValue_);
                currentValue_ = value;
            } else {
                float value = std::get<float>(left) / std::get<float>(currentValue_);
                currentValue_ = value;
            }
        }
    }
}

void InterpreterVisitor::visitArithmeticExpression(Nodes::ArithmeticExpression *ae) {
    std::variant<int, float, std::string> left;

    if (ae->hasLeft()) {
        ae->acceptLeft(*this);
        left = currentValue_;
    }

    bool flag_nullopt = false;
    if (!expectedType_.has_value()) {
        flag_nullopt = true;
        if (currentValue_.index() == 0) {
            expectedType_ = INT;
        } else if (currentValue_.index() == 1) {
            expectedType_ = FLOAT;
        } else {
            expectedType_ = STRING;
        }
    }

    if (ae->getTermOperator()) {
        termOperator top = ae->getTermOperator()->getType();
        ae->acceptRight(*this);

        if (expectedType_ == INT) {
            if (top == termOperator::PLUS) {
                int value = std::get<int>(left) + std::get<int>(currentValue_);
                currentValue_ = value;
            } else {
                int value = std::get<int>(left) - std::get<int>(currentValue_);
                currentValue_ = value;
            }
        } else if (expectedType_ == FLOAT) {
            if (top == termOperator::PLUS) {
                float value = std::get<float>(left) + std::get<float>(currentValue_);
                currentValue_ = value;
            } else {
                float value = std::get<float>(left) - std::get<float>(currentValue_);
                currentValue_ = value;
            }
        } else if (expectedType_ == STRING) {
            if (top == termOperator::PLUS) {
                std::string value = std::get<std::string>(left) + std::get<std::string>(currentValue_);
                currentValue_ = value;
            } else {
                throw std::runtime_error("Cannot subtract strings");
            }
        }
    }
    if (flag_nullopt) {
        expectedType_ = std::nullopt;
    }
}

void InterpreterVisitor::visitVariableReference(Nodes::VariableReference *variableReference) {
    auto symbol = symbolManager_.getSymbol(variableReference->getIdentifier(), false);

    if (symbol.has_value()) {
        currentValue_ = symbol.value().getValue();
    } else {
        auto type = symbol->getType();
        if (type == INT) {
            currentValue_.emplace<int>(0);
        } else if (type == FLOAT) {
            currentValue_.emplace<float>(0.0);
        } else {
            currentValue_.emplace<std::string>("");
        }
    }
}

void InterpreterVisitor::visitExpression(Nodes::Expression *expression) {
    expression->acceptLeft(*this);
    std::variant<int, float, std::string> left = currentValue_;

    if (expression->getOperator()) {
        relationalOperator rop = expression->getOperator()->getType();
        expression->acceptRight(*this);

        switch (rop) {
            case relationalOperator::LESS:
            {
                currentValue_.emplace<int>(left < currentValue_);
                break;
            }
            case relationalOperator::LESSEQUAL:
            {
                currentValue_.emplace<int>(left <= currentValue_);
                break;
            }
            case relationalOperator::GREATER:
            {
                currentValue_.emplace<int>(left > currentValue_);
                break;
            }
            case relationalOperator::GREATEREQUAL:
            {
                currentValue_.emplace<int>(left >= currentValue_);
                break;
            }
            case relationalOperator::EQUAL:
            {
                currentValue_.emplace<int>(left == currentValue_);
                break;
            }
            case relationalOperator::NOTEQUAL:
            {
                currentValue_.emplace<int>(left != currentValue_);
                break;
            }
            default:
            {
                throw std::runtime_error("Unknown relational operator");
            }
        }
    }
}

void InterpreterVisitor::visitLocalVariableDeclaration(Nodes::LocalVariableDeclaration *declaration) {
    std::variant<int, float, std::string> value;
    if (declaration->getType() == INT) {
        value.emplace<int>(0);
    } else if (declaration->getType() == FLOAT) {
        value.emplace<float>(0.0);
    } else {
        value.emplace<std::string>("");
    }

    symbolManager_.insertSymbol(declaration->getIdentifier(), SymbolTable::Symbol(false, declaration->getType(), value));

    if (declaration->hasDefault()) {
        std::optional<idType> previousType = expectedType_;
        expectedType_.emplace(declaration->getType());
        declaration->acceptDefault(*this);
        symbolManager_.setValue(declaration->getIdentifier(), currentValue_);
        expectedType_ = previousType;
    }
}

void InterpreterVisitor::visitStatementBlock(Nodes::StatementBlock *block) {
    symbolManager_.enterNewScope();
    block->acceptStatements(*this);
    symbolManager_.exitScope();
}

void InterpreterVisitor::visitAssign(Nodes::Assign *assign) {
    if (returned_) {
        return;
    }

    auto symbol = symbolManager_.getSymbol(assign->getAssignedIdentifier(), false);
    auto previous = expectedType_;
    expectedType_ = symbol.value().getType();
    assign->acceptAssignedExpression(*this);
    symbol.value().setValue(currentValue_);
    symbolManager_.setValue(assign->getAssignedIdentifier(), currentValue_);

    if (symbolManager_.isGlobal(assign->getAssignedIdentifier())) {
        variables_[assign->getAssignedIdentifier()] = currentValue_;
    }
    expectedType_ = previous;
}

void InterpreterVisitor::visitIfStatement(Nodes::IfStatement *statement) {
    if (returned_) {
        return;
    }

    auto previous = expectedType_;
    expectedType_ = std::nullopt;

    statement->acceptCondition(*this);

    if (currentValue_.index() == 0) {
        int cond = std::get<int>(currentValue_);
        if (cond) {
            statement->acceptIfBlock(*this);
        } else if (statement->hasElseBlock()) {
            statement->acceptElseBlock(*this);
        }
    } else if (currentValue_.index() == 1) {
        float cond = std::get<float>(currentValue_);
        if (cond) {
            statement->acceptIfBlock(*this);
        } else if (statement->hasElseBlock()) {
            statement->acceptElseBlock(*this);
        }
    }
    expectedType_ = previous;
}

void InterpreterVisitor::visitWhileStatement(Nodes::WhileStatement *statement) {
    if (returned_) {
        return;
    }

    auto previous = expectedType_;
    expectedType_ = std::nullopt;

    statement->acceptCondition(*this);

    if (currentValue_.index() == 0) {
        int cond = std::get<int>(currentValue_);
        while (cond) {
            statement->acceptWhileBlock(*this);
            statement->acceptCondition(*this);
            cond = std::get<int>(currentValue_);
        }
    } else if (currentValue_.index() == 1) {
        float cond = std::get<float>(currentValue_);
        while (cond) {
            statement->acceptWhileBlock(*this);
            statement->acceptCondition(*this);
            cond = std::get<float>(currentValue_);
        }
    }
    expectedType_ = previous;
}

void InterpreterVisitor::visitReturnStatement(Nodes::ReturnStatement *statement) {
    if (returned_) {
        return;
    }

    statement->acceptReturned(*this);
    returned_ = true;
}

void InterpreterVisitor::visitFunctionCall(Nodes::FunctionCall *call) {
    auto symbol = symbolManager_.getSymbol(call->getIdentifier(), true);
    auto callArgs = call->getArgList();

    arguments_.clear();

    if (callArgs.has_value()) {
        for (int i = 0; i < callArgs.value().size(); i++) {
            callArgs.value()[i]->accept(*this);
        }
    }
    symbol.value().getFunPointer().value()->accept(*this);
}

void InterpreterVisitor::visitFunctionCallStatement(Nodes::FunctionCallStatement *statement) {
    if (returned_) {
        return;
    }

    auto symbol = symbolManager_.getSymbol(statement->getIdentifier(), true);

    if (statement->getIdentifier() == "print") {
        if (statement->getArgList()) {
            for (int i = 0; i < statement->getArgList().value().size(); i++) {
                statement->getArgList().value()[i]->accept(*this);
                if (currentValue_.index() == 0) {
                    std::cout << std::get<int>(currentValue_) << std::endl;
                } else if (currentValue_.index() == 1) {
                    std::cout << std::get<float>(currentValue_) << std::endl;
                } else {
                    std::cout << std::get<std::string>(currentValue_) << std::endl;
                }
            }
        } else {
            std::cout << "no arguments to display in print() function" << std::endl;
        }
        return;
    }

    auto calledArgs = statement->getArgList();
    arguments_.clear();

    if (calledArgs.has_value()) {
        for (int i = 0; i < calledArgs.value().size(); i++) {
            calledArgs.value()[i]->accept(*this);
            arguments_.push_back(currentValue_);
        }
    }
    symbol.value().getFunPointer().value()->accept(*this);
}

void InterpreterVisitor::visitDeclaration(Nodes::Declaration *declaration) {
    std::variant<int, float, std::string> value;
    if (declaration->getType() == INT) {
        value.emplace<int>(0);
    } else if (declaration->getType() == FLOAT) {
        value.emplace<float>(0.0);
    } else {
        value.emplace<std::string>("");
    }

    symbolManager_.insertSymbol(declaration->getIdentifier(), SymbolTable::Symbol(false, declaration->getType(), value));

    if (declaration->hasDefault()) {
        auto previous = expectedType_;
        expectedType_.emplace(declaration->getType());

        if (expectedType_ == idType::INT) {
            currentValue_.emplace<int>(0);
        } else if (expectedType_ == idType::FLOAT) {
            currentValue_.emplace<float>(0.0);
        } else {
            currentValue_.emplace<std::string>("");
        }

        declaration->acceptDefault(*this);
        symbolManager_.setValue(declaration->getIdentifier(), currentValue_);
        variables_.insert(std::make_pair(declaration->getIdentifier(), currentValue_));
        expectedType_ = previous;
    } else {
        variables_.insert(std::make_pair(declaration->getIdentifier(), 0));
    }
}

void InterpreterVisitor::visitFunctionDeclaration(Nodes::FunctionDeclaration *declaration) {
    symbolManager_.insertSymbol(declaration->getIdentifier(), SymbolTable::Symbol(true, declaration->getType(), declaration));

    auto previous = expectedType_;
    expectedType_ = declaration->getType();

    symbolManager_.enterNewContext();
    symbolManager_.enterNewScope();

    if (declaration->getArgsDeclaration().has_value()) {
        for (int i = 0; i < arguments_.size(); i++) {
            idType type = INT;
            if (arguments_[i].index() == 1) {
                type = FLOAT;
            } else if (arguments_[i].index() == 2) {
                type = STRING;
            }
            symbolManager_.insertSymbol(declaration->getArgsDeclaration().value()[i]->getIdentifier(), SymbolTable::Symbol(false, type, arguments_[i]));
        }
    }

    declaration->acceptFunctionBody(*this);
    if (returned_) {
        returned_ = false;
    }
    symbolManager_.exitContext();
    expectedType_ = previous;
}

void InterpreterVisitor::visitDictionary(Nodes::Dictionary *) {}

void InterpreterVisitor::visitRelationalOperator(Nodes::RelationalOperator *) {}

void InterpreterVisitor::visitTermOperator(Nodes::TermOperator *) {}

void InterpreterVisitor::visitFactorOperator(Nodes::FactorOperator *) {}

void InterpreterVisitor::visitMemberReference(Nodes::MemberReference *) {}