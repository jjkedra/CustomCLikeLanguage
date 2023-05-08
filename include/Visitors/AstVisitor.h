//
// Created by Jan Kędra on 02/05/2023.
//

#ifndef LEXER_ASTVISITOR_H
#define LEXER_ASTVISITOR_H

#include "Tree.h"

class AstVisitor {
public:
    virtual void visitNumber(Nodes::Number*) = 0;
    virtual void visitTerm(Nodes::Term*) = 0;
    virtual void visitArithmeticExpression(Nodes::ArithmeticExpression*) = 0;
    virtual void visitFunctionCall(Nodes::FunctionCall*) = 0;
    virtual void visitVariableReference(Nodes::VariableReference*) = 0;
    virtual void visitExpression(Nodes::Expression*) = 0;
    virtual void visitLocalVariableDeclaration(Nodes::LocalVariableDeclaration*) = 0;
    virtual void visitStatementBlock(Nodes::StatementBlock*) = 0;
    virtual void visitAssign(Nodes::Assign*) = 0;
    virtual void visitIfStatement(Nodes::IfStatement*) = 0;
    virtual void visitWhileStatement(Nodes::WhileStatement*) = 0;
    virtual void visitReturnStatement(Nodes::ReturnStatement*) = 0;
    virtual void visitFunctionCallStatement(Nodes::FunctionCallStatement*) = 0;
    virtual void visitDeclaration(Nodes::Declaration*) = 0;
    virtual void visitFunctionDeclaration(Nodes::FunctionDeclaration*) = 0;
    virtual void visitProgram(Nodes::Program*) = 0;
    virtual void visitRelationalOperator(Nodes::RelationalOperator*) = 0;
    virtual void visitTermOperator(Nodes::TermOperator*) = 0;
    virtual void visitFactorOperator(Nodes::FactorOperator*) = 0;
};


#endif //LEXER_ASTVISITOR_H
