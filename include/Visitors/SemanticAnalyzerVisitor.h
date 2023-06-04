//
// Created by Jan Kędra on 02/06/2023.
//

#ifndef LEXER_SEMANTICANALYZERVISITOR_H
#define LEXER_SEMANTICANALYZERVISITOR_H

#include "AstVisitor.h"
#include "SymbolManager.h"

class SemanticAnalyzerVisitor : public AstVisitor {
private:
    SymbolManager symbolManager_;
    std::optional<idType> expectedType_;
public:
    void visitProgram(Nodes::Program*) override;
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
};


#endif //LEXER_SEMANTICANALYZERVISITOR_H
