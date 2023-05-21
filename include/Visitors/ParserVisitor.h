//
// Created by Jan Kędra on 02/05/2023.
//

#ifndef LEXER_PARSERVISITOR_H
#define LEXER_PARSERVISITOR_H

#include "AstVisitor.h"

/**
 * @brief ParserVisitor is a concrete class for visiting the nodes in an abstract syntax tree.
 * This code declares a C++ class called parserVisitor that inherits from the abstract base class AstVisitor.
 * The class is used for visiting nodes of an Abstract Syntax Tree (AST) representing a programming language.
 * The parserVisitor class has private member variables parsed and ident of type std::vector<std::string> and int, respectively.
 * The parsed vector is used to store the parsed results during the AST traversal process,
 * while ident is used to keep track of the current indentation level.
 */

class parserVisitor: public AstVisitor
{
private:
    std::vector<std::string> parsed;

    void visitDictionary(Nodes::Dictionary*) override;
    void visitString(Nodes::String*) override;
    void visitNumber(Nodes::Number*) override;
    void visitTerm(Nodes::Term*)  override;
    void visitArithmeticExpression(Nodes::ArithmeticExpression*)  override;
    void visitFunctionCall(Nodes::FunctionCall*)  override;
    void visitVariableReference(Nodes::VariableReference*)  override;
    void visitExpression(Nodes::Expression*)  override;
    void visitLocalVariableDeclaration(Nodes::LocalVariableDeclaration*)  override;
    void visitStatementBlock(Nodes::StatementBlock*)  override;
    void visitAssign(Nodes::Assign*)  override;
    void visitIfStatement(Nodes::IfStatement*)  override;
    void visitWhileStatement(Nodes::WhileStatement*)  override;
    void visitReturnStatement(Nodes::ReturnStatement*)  override;
    void visitFunctionCallStatement(Nodes::FunctionCallStatement*)  override;
    void visitDeclaration(Nodes::Declaration*)  override;
    void visitFunctionDeclaration(Nodes::FunctionDeclaration*)  override;
    void visitRelationalOperator(Nodes::RelationalOperator*)  override;
    void visitTermOperator(Nodes::TermOperator*)  override;
    void visitFactorOperator(Nodes::FactorOperator*)  override;
    void visitMemberReference(Nodes::MemberReference*)  override;

public:
    void visitProgram(Nodes::Program*)  override;
    std::vector<std::string> getParsed()
    {
        return parsed;
    }

};



#endif //LEXER_PARSERVISITOR_H
