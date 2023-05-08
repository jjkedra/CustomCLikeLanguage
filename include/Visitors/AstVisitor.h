//
// Created by Jan Kędra on 02/05/2023.
//

#ifndef LEXER_ASTVISITOR_H
#define LEXER_ASTVISITOR_H

#include <memory>
#include <variant>
#include <vector>
#include "Tree.h"

/**
 * @brief AstVisitor is an abstract base class for visiting the nodes in an abstract syntax tree.
 * It contains virtual functions to visit each type of node in the tree, which must be implemented by derived classes.
 * The nodes that can be visited include expressions, statements, declarations, and operators.
 *
 * @note This class is part of a larger system for parsing and interpreting a programming language.
 *
 * @see Nodes::Number, Nodes::Term, Nodes::ArithmeticExpression, Nodes::FunctionCall, Nodes::VariableReference,
 * Nodes::Expression, Nodes::LocalVariableDeclaration, Nodes::StatementBlock, Nodes::Assign, Nodes::IfStatement,
 * Nodes::WhileStatement, Nodes::ReturnStatement, Nodes::FunctionCallStatement, Nodes::Declaration,
 * Nodes::FunctionDeclaration, Nodes::Program, Nodes::RelationalOperator, Nodes::TermOperator, Nodes::FactorOperator,
 * Tree.h
 */

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
