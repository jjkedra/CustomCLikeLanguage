//
// Created by Jan Kędra on 02/05/2023.
//

#include "Tree.h"
#include "../include/Visitors/AstVisitor.h"

std::ostream &operator<<(std::ostream &os, Node *node)
{
    os << "Node name: " << node->nodeName_;
    return os;
}

Position Node::getPosition() { return position_; }

void Nodes::Number::accept(AstVisitor &v)                               { v.visitNumber(this); }
void Nodes::TermOperator::accept(AstVisitor &v)                         { v.visitTermOperator(this); }
void Nodes::FactorOperator::accept(AstVisitor &v)                       { v.visitFactorOperator(this); }
void Nodes::Term::accept(AstVisitor &v)                                 { v.visitTerm(this); }
void Nodes::ArithmeticExpression::accept(AstVisitor &v)                 { v.visitArithmeticExpression(this); }
void Nodes::FunctionDeclaration::accept(AstVisitor &v)                  { v.visitFunctionDeclaration(this); }
void Nodes::Declaration::accept(AstVisitor &v)                          { v.visitDeclaration(this); }
void Nodes::RelationalOperator::accept(AstVisitor &v)                   { v.visitRelationalOperator(this); }
void Nodes::Expression::accept(AstVisitor &v)                           { v.visitExpression(this); }
void Nodes::VariableReference::accept(AstVisitor &v)                    { v.visitVariableReference(this); }
void Nodes::Assign::accept(AstVisitor &v)                               { v.visitAssign(this); }
void Nodes::StatementBlock::accept(AstVisitor &v)                       { v.visitStatementBlock(this); }
void Nodes::IfStatement::accept(AstVisitor &v)                          { v.visitIfStatement(this); }
void Nodes::WhileStatement::accept(AstVisitor &v)                       { v.visitWhileStatement(this); }
void Nodes::ReturnStatement::accept(AstVisitor &v)                      { v.visitReturnStatement(this); }
void Nodes::LocalVariableDeclaration::accept(AstVisitor &v)             { v.visitLocalVariableDeclaration(this); }
void Nodes::Program::accept(AstVisitor &v)                              { v.visitProgram(this); }
void Nodes::FunctionCall::accept(AstVisitor &v)                         { v.visitFunctionCall(this); }
void Nodes::FunctionCallStatement::accept(AstVisitor &v)                { v.visitFunctionCallStatement(this); }

std::string Nodes::Assign::getAssignedIdentifier() const {
    return assignedIdentifier_;
}

void Nodes::Assign::acceptAssignedExpression(AstVisitor &v) const {
    if (assignedExpression_) {
        assignedExpression_->accept(v);
    }
}

void Nodes::StatementBlock::acceptStatements(AstVisitor &v) const {
    for (const auto & statement : statement_) {
        statement->accept(v);
    }
}

void Nodes::IfStatement::acceptCondition(AstVisitor &v) const {
    condition_->accept(v);
}

idType Nodes::FunctionDeclaration::getType() const {
    return type_;
}

std::string Nodes::FunctionDeclaration::getIdentifier() const {
    return identifier_;
}

std::optional<std::vector<Nodes::LocalVariableDeclaration*>> Nodes::FunctionDeclaration::getArgsDeclaration() {
    if (argsDeclaration_.empty()) {
        return std::nullopt;
    } else {
        std::vector<Nodes::LocalVariableDeclaration*> args;
        for (auto iter = argsDeclaration_.begin(); iter != argsDeclaration_.end(); iter++) {
            args.push_back(iter->get());
        }
        return args;
    }
}

void Nodes::FunctionDeclaration::acceptFunctionBody(AstVisitor &v) {
    functionBody_->accept(v);
}

void Nodes::IfStatement::acceptIfBlock(AstVisitor &v) {
    ifBlock_->accept(v);
}

void Nodes::IfStatement::acceptElseBlock(AstVisitor &v) {
    if(elseBlock_) {
        elseBlock_->accept(v);
    }
}

void Nodes::WhileStatement::acceptCondition(AstVisitor &v) {
    condition_->accept(v);
}

void Nodes::WhileStatement::acceptWhileBlock(AstVisitor &v) {
    whileBlock_->accept(v);
}

void Nodes::ReturnStatement::acceptReturned(AstVisitor &v) {
    returnedExpression_->accept(v);
}

idType Nodes::LocalVariableDeclaration::getType() const {
    return type_;
}

std::string Nodes::LocalVariableDeclaration::getIdentifier() const {
    return identifier_;
}

void Nodes::LocalVariableDeclaration::acceptDefault(AstVisitor &v) const {
    if (defaultValue_) {
        defaultValue_->accept(v);
    }
}

std::optional<std::vector<Nodes::ArithmeticExpression*>> Nodes::FunctionCallStatement::getArgList() {
    if (argList_.empty()) {
        return std::nullopt;
    } else {
        std::vector<Nodes::ArithmeticExpression*> args;
        for (auto & iter : argList_) {
            args.push_back(iter.get());
        }
        return args;
    }
}

std::string Nodes::FunctionCallStatement::getIdentifier() const {
    return identifier_;
}

idType Nodes::Number::getType() const {
    if (value_.index() == 0) {
        return idType::INT;
    } else if (value_.index() == 1){
        return idType::FLOAT;
    } else {
        return idType::STRING;
    }
}

std::variant<int, float, std::string> Nodes::Number::getValue() const {
    return value_;
}

std::string Nodes::Declaration::getIdentifier() {
    return identifier_;
}

idType Nodes::Declaration::getType() const {
    return type_;
}

void Nodes::Declaration::acceptDefault(AstVisitor &v) const {
    if (defaultValue_) {
        defaultValue_->accept(v);
    }
}

void Nodes::Term::acceptLeft(AstVisitor &v) const {
    left_->accept(v);
}

void Nodes::Term::acceptRight(AstVisitor &v) const {
    if (right_) {
        right_->accept(v);
    }
}

void Nodes::Term::acceptOperator(AstVisitor &v) const {
    if (fop_) {
        fop_->accept(v);
    }
}

std::string Nodes::VariableReference::getIdentifier() const {
    return identifier_;
}

void Nodes::ArithmeticExpression::acceptLeft(AstVisitor &v) const {
    if (left_) {
        left_->accept(v);
    }
}

void Nodes::ArithmeticExpression::acceptRight(AstVisitor &v) const {
    if (right_) {
        right_->accept(v);
    }
    
}

void Nodes::ArithmeticExpression::acceptTermOperator(AstVisitor &v) const {
    if (op_) {
        op_->accept(v);
    }
}

void Nodes::Expression::acceptLeft(AstVisitor &v) const {
    left_->accept(v);
}

void Nodes::Expression::acceptRight(AstVisitor &v) const {
    if (right_) {
        right_->accept(v);
    }
}

void Nodes::Expression::acceptOperator(AstVisitor &v) const {
    if (rop_) {
        rop_->accept(v);
    }
}

std::optional<std::vector<Nodes::ArithmeticExpression*>> Nodes::FunctionCall::getArgList() const {
    if (argList_.empty()) {
        return std::nullopt;
    } else {
        std::vector<Nodes::ArithmeticExpression*> args;

        for (const auto & iter : argList_) {
            args.push_back(iter.get());
        }
        return args;
    }
}

std::string Nodes::FunctionCall::getIdentifier() const {
    return identifier_;
}

std::map<std::string, std::unique_ptr<Nodes::FunctionDeclaration>> const &Nodes::Program::getFunctions() const {
    return functions_;
}

std::map<std::string, std::unique_ptr<Nodes::Declaration>> const &Nodes::Program::getVariables() const {
    return variables_;
}