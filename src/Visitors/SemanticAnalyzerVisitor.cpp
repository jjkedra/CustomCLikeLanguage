//
// Created by Jan Kędra on 02/06/2023.
//

#include "SemanticAnalyzerVisitor.h"
#include "LangException.h"

void SemanticAnalyzerVisitor::visitProgram(Nodes::Program* program) {
    if (program->getFunctions().empty()) {
        throw LangException("Program must contain at least one function", program->getPosition(), __FUNCTION__ , __LINE__ );
    }
    if (program->getFunctions().find("main") == program->getFunctions().end()) {
        throw LangException("Program must contain a main function", program->getPosition(), __FUNCTION__ , __LINE__ );
    }

    symbolManager_.enterNewContext();
    symbolManager_.enterNewScope();

    for (const auto & iter : program->getVariables()) {
        iter.second->accept(*this);
    }

    for (const auto& iter : program->getFunctions()) {
        symbolManager_.insertSymbol(iter.first, SymbolTable::Symbol(true, iter.second->getType(), iter.second.get()));
    }

    for (const auto & iter : program->getFunctions()) {
        iter.second->accept(*this);
    }

    symbolManager_.exitContext();
}

void SemanticAnalyzerVisitor::visitNumber(Nodes::Number *number) {
    if (expectedType_ == std::nullopt) {
        return;
    }
    if (number->getType() != expectedType_) {
        throw LangException("Invalid assignment!\nExpected "+
        Nodes::identiferTypes[expectedType_.value()]+" but received "+
        Nodes::identiferTypes[number->getType()], number->getPosition());
    }
}

void SemanticAnalyzerVisitor::visitTerm(Nodes::Term *term) {
    term->acceptLeft(*this);
    term->acceptOperator(*this);
    term->acceptRight(*this);
}

void SemanticAnalyzerVisitor::visitArithmeticExpression(Nodes::ArithmeticExpression *ae) {
    ae->acceptLeft(*this);
    ae->acceptRight(*this);
}

void SemanticAnalyzerVisitor::visitFunctionCall(Nodes::FunctionCall *fc) {
    auto symbol = symbolManager_.getSymbol(fc->getIdentifier(), true);

    if (fc->getIdentifier() == "print") {
        throw LangException("Cannot use print() as a r-value or argument!", fc->getPosition());
    }
    if (!symbol.has_value()) {
        throw LangException("Function "+fc->getIdentifier()+" is not defined!", fc->getPosition());
    }
    if (!symbol.value().isFunction()) {
        throw LangException("Cannot use variable as a function " + fc->getIdentifier(), fc->getPosition());
    }
    if (expectedType_ != std::nullopt) {
        if (symbol.value().getType() != expectedType_) {
            throw LangException("Invalid function type: "+fc->getIdentifier()+
            "() returns: " + Nodes::identiferTypes[symbol.value().getType()] +
            " but expected: " + Nodes::identiferTypes[expectedType_.value()], fc->getPosition());
        }
    }

    auto declaredArgs = symbol.value().getFunPointer().value()->getArgsDeclaration();
    auto calledArgs = fc->getArgList();

    if (declaredArgs.has_value()) {
        if (calledArgs.has_value()) {
            if (declaredArgs.value().size() != calledArgs.value().size()) {
                throw LangException("Invalid number of arguments in function call: " + fc->getIdentifier(), fc->getPosition());
            }

            for (int i = 0; i < declaredArgs.value().size(); i++) {
                auto previous = expectedType_;
                expectedType_ = declaredArgs.value()[i]->getType();
                calledArgs.value()[i]->accept(*this);
                expectedType_ = previous;
            }
        } else {
            throw LangException("The amount of args not a match with declaration: " + fc->getIdentifier(), fc->getPosition());
        }
    } else {
        if (!calledArgs.has_value()) {
            return;
        }
        throw LangException("The amount of args not a match with declaration: " + fc->getIdentifier(), fc->getPosition());
    }
}

void SemanticAnalyzerVisitor::visitVariableReference(Nodes::VariableReference *ref) {
    auto symbol = symbolManager_.getSymbol(ref->getIdentifier(), false);

    if (!symbol.has_value()) {
        throw LangException("Variable "+ref->getIdentifier()+" is not defined!", ref->getPosition());
    }
    if (symbol.value().isFunction()) {
        throw LangException("Cannot use function as a variable: " + ref->getIdentifier(), ref->getPosition());
    }
    if (expectedType_.has_value() && symbol.value().getType() != expectedType_) {
        throw LangException("Invalid variable type: "+ref->getIdentifier()+
        " is of type: " + Nodes::identiferTypes[symbol.value().getType()] +
        " but expected: " + Nodes::identiferTypes[expectedType_.value()], ref->getPosition());
    }
}


void SemanticAnalyzerVisitor::visitExpression(Nodes::Expression *expression) {
    auto previous = expectedType_;
    expectedType_ = std::nullopt;
    expression->acceptLeft(*this);
    expression->acceptRight(*this);
    expectedType_ = previous;
}

void SemanticAnalyzerVisitor::visitLocalVariableDeclaration(Nodes::LocalVariableDeclaration *dec) {
    std::variant<int, float, std::string> value;
    if (dec->getType() == INT) {
        value.emplace<int>(0);
    } else if (dec->getType() == FLOAT) {
        value.emplace<float>(0.0f);
    } else if (dec->getType() == STRING) {
        value.emplace<std::string>("");
    }

    if (!symbolManager_.insertSymbol(dec->getIdentifier(), SymbolTable::Symbol(false, dec->getType(), value))) {
        throw LangException("Variable "+dec->getIdentifier()+" already defined!", dec->getPosition());
    }

    if (dec->hasDefault()) {
        std::optional<idType> previous = std::nullopt;

        if (expectedType_.has_value()) {
            previous = expectedType_;
        }
        expectedType_.emplace(dec->getType());
        dec->acceptDefault(*this);
        expectedType_ = previous;
    }
}

void SemanticAnalyzerVisitor::visitStatementBlock(Nodes::StatementBlock *statement) {
    symbolManager_.enterNewScope();
    statement->acceptStatements(*this);
    symbolManager_.exitScope();
}

void SemanticAnalyzerVisitor::visitAssign(Nodes::Assign *assign) {
    auto symbol = symbolManager_.getSymbol(assign->getAssignedIdentifier(), false);

    if (!symbol.has_value()) {
        throw LangException("Variable "+assign->getAssignedIdentifier()+" is not defined!", assign->getPosition());
    }
    if (symbol.value().isFunction()) {
        throw LangException("Cannot assign to function " + assign->getAssignedIdentifier(), assign->getPosition());
    }
    auto previous = expectedType_;
    expectedType_ = symbol.value().getType();
    assign->acceptAssignedExpression(*this);
    expectedType_ = previous;
}

void SemanticAnalyzerVisitor::visitIfStatement(Nodes::IfStatement *statement) {
    statement->acceptCondition(*this);
    statement->acceptIfBlock(*this);
    if (statement->hasElseBlock()) {
        statement->acceptElseBlock(*this);
    }
}

void SemanticAnalyzerVisitor::visitWhileStatement(Nodes::WhileStatement *statement) {
    statement->acceptCondition(*this);
    statement->acceptWhileBlock(*this);
}

void SemanticAnalyzerVisitor::visitReturnStatement(Nodes::ReturnStatement *statement) {
    statement->acceptReturned(*this);
}

void SemanticAnalyzerVisitor::visitFunctionCallStatement(Nodes::FunctionCallStatement *statement) {
    auto symbol = symbolManager_.getSymbol(statement->getIdentifier(), true);

    if (statement->getIdentifier() == "print") {
        auto previous = expectedType_;
        expectedType_ = std::nullopt;
        auto calledArgs = statement->getArgList();
        for (int i = 0; i < calledArgs->size(); i++) {
            calledArgs.value()[i]->accept(*this);
        }
        expectedType_ = previous;
        return;
    }
    if (!symbol.has_value()) {
        throw LangException("Function "+statement->getIdentifier()+" is not defined!", statement->getPosition());
    }
    if (!symbol.value().isFunction()) {
        throw LangException("Cannot use variable as a function " + statement->getIdentifier(), statement->getPosition());
    }

    auto calledArgs = statement->getArgList();
    auto declaredArgs = symbol.value().getFunPointer().value()->getArgsDeclaration();

    if (declaredArgs.has_value()) {
        if (calledArgs.has_value()) {
            if (calledArgs.value().size() != declaredArgs.value().size()) {
                throw LangException("Invalid number of arguments in function call: " + statement->getIdentifier(), statement->getPosition());
            }

            for (int i = 0; i < calledArgs.value().size(); i++) {
                auto previous = expectedType_;
                expectedType_ = declaredArgs.value()[i]->getType();
                calledArgs.value()[i]->accept(*this);
                expectedType_ = previous;
            }
        } else {
            throw LangException("The amount of args not a match with declaration: " + statement->getIdentifier(), statement->getPosition());
        }
    } else {
        if (!calledArgs.has_value()) {
            return;
        }
        throw LangException("The amount of args not a match with declaration: " + statement->getIdentifier(), statement->getPosition());
    }
}

void SemanticAnalyzerVisitor::visitDeclaration(Nodes::Declaration *dec) {
    std::variant<int, float, std::string> value;
    if (dec->getType() == INT) {
        value.emplace<int>(0);
    } else if (dec->getType() == FLOAT) {
        value.emplace<float>(0.0f);
    } else if (dec->getType() == STRING) {
        value.emplace<std::string>("");
    }

    if (!symbolManager_.insertSymbol(dec->getIdentifier(), SymbolTable::Symbol(false, dec->getType(), value))) {
        throw LangException("Variable "+dec->getIdentifier()+" already defined!", dec->getPosition());
    }

    if (dec->hasDefault()) {
        std::optional<idType> previous = std::nullopt;

        if (expectedType_.has_value()) {
            previous = expectedType_;
        }
        expectedType_.emplace(dec->getType());
        dec->acceptDefault(*this);
        expectedType_ = previous;
    }
}

void SemanticAnalyzerVisitor::visitFunctionDeclaration(Nodes::FunctionDeclaration *dec) {
    auto previous = expectedType_;
    expectedType_ = dec->getType();

    symbolManager_.enterNewContext();
    symbolManager_.enterNewScope();

    if (dec->getArgsDeclaration().has_value()) {
        for (int i = 0; i < dec->getArgsDeclaration().value().size(); i++) {
           dec->getArgsDeclaration().value()[i]->accept(*this);
        }
    }
    dec->acceptFunctionBody(*this);
    expectedType_ = previous;
    symbolManager_.exitContext();
}

void SemanticAnalyzerVisitor::visitString(Nodes::String *string) {}

void SemanticAnalyzerVisitor::visitDictionary(Nodes::Dictionary *) {}

void SemanticAnalyzerVisitor::visitRelationalOperator(Nodes::RelationalOperator *) {}

void SemanticAnalyzerVisitor::visitTermOperator(Nodes::TermOperator *) {}

void SemanticAnalyzerVisitor::visitFactorOperator(Nodes::FactorOperator *) {}

void SemanticAnalyzerVisitor::visitMemberReference(Nodes::MemberReference *) {}

