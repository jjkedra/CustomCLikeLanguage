//
// Created by Jan Kędra on 02/05/2023.
//

#ifndef LEXER_PARSERVISITOR_H
#define LEXER_PARSERVISITOR_H

#include "AstVisitor.h"

class parserVisitor: public AstVisitor
{
private:
    std::vector<std::string> parsed;
    int ident=0;

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

public:
    void visitProgram(Nodes::Program*)  override;
    std::vector<std::string> getParsed()
    {
        return parsed;
    }

};



#endif //LEXER_PARSERVISITOR_H
