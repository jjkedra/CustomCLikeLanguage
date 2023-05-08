//
// Created by Jan Kędra on 02/05/2023.
//

#include "../../include/Visitors/ParserVisitor.h"
#include <sstream>

void parserVisitor::visitProgram(Nodes::Program* program) {
    for(auto iter= program->getVariables().begin(); iter != program->getVariables().end(); iter++) {
        parsed.push_back("variable:" + iter->first);
        iter->second->accept(*this);
    }

    for(auto it = program->getFunctions().begin(); it != program->getFunctions().end(); it++) {
        parsed.push_back("function:" + it->first);
        it->second->accept(*this);
    }
}

void parserVisitor::visitNumber(Nodes::Number *number) {
    std::stringstream stream;
    parsed.push_back("number:");
    if(number->getType()==idType::INT) {
    parsed.push_back("int:");
    stream<<std::get<int>(number->getValue());
    } else {
        parsed.push_back("float:");
        stream<<std::get<float>(number->getValue());
    }
    std::string str;
    stream>>str;
    parsed.push_back(str);
}

void parserVisitor::visitTerm(Nodes::Term * term) {
    parsed.push_back("term:");
    parsed.push_back("left factor:");
    term->acceptLeft(*this);
    parsed.push_back("operator:");
    term->acceptOperator(*this);
    parsed.push_back("right factor:");
    term->acceptRight(*this);
}

void parserVisitor::visitArithmeticExpression(Nodes::ArithmeticExpression *arithmeticExpr) {
    parsed.push_back("arithmeticExpr:");
    parsed.push_back("left term:");
    arithmeticExpr->acceptLeft(*this);
    parsed.push_back("operator:");
    arithmeticExpr->acceptTermOperator(*this);
    parsed.push_back("right term:");
    arithmeticExpr->acceptRight(*this);
}

void parserVisitor::visitFunctionCall(Nodes::FunctionCall *funcCall) {
    return;
}

void parserVisitor::visitVariableReference(Nodes::VariableReference *variable) {
    parsed.push_back("variableReference:" + variable->getIdentifier());
}

void parserVisitor::visitExpression(Nodes::Expression *expr) {
    parsed.push_back("expression");
    parsed.push_back("leftSide:");
    expr->acceptLeft(*this);
    parsed.push_back("operator:");
    expr->acceptOperator(*this);
    parsed.push_back("rightSide:");
    expr->acceptRight(*this);
}

void parserVisitor::visitLocalVariableDeclaration(Nodes::LocalVariableDeclaration *localVarDeclaration) {
    parsed.push_back("localVariable:"+localVarDeclaration->getIdentifier());
    if (localVarDeclaration->hasDefault()) {
        parsed.push_back("default:");
        localVarDeclaration->acceptDefault(*this);
    }
}

void parserVisitor::visitStatementBlock(Nodes::StatementBlock *stmtBlock) {
    parsed.push_back("stmtBlock");
    stmtBlock->acceptStatements(*this);
}

void parserVisitor::visitAssign(Nodes::Assign *assign) {
    parsed.push_back("assigning:");
    parsed.push_back(assign->getAssignedIdentifier());
    parsed.push_back("assigned expr:");
    assign->acceptAssignedExpression(*this);
}

void parserVisitor::visitIfStatement(Nodes::IfStatement *ifStmt) {
    parsed.push_back("if:");
    parsed.push_back("condition:");
    ifStmt->acceptCondition(*this);
    parsed.push_back("body:");
    ifStmt->acceptIfBlock(*this);
    if(ifStmt->hasElseBlock()) {
        parsed.push_back("else:");
        ifStmt->acceptElseBlock(*this);
    }
}

void parserVisitor::visitWhileStatement(Nodes::WhileStatement *whileStmt) {
    parsed.push_back("while:");
    parsed.push_back("condition");
    whileStmt->acceptCondition(*this);
    parsed.push_back("body:");
    whileStmt->acceptWhileBlock(*this);
}

void parserVisitor::visitReturnStatement(Nodes::ReturnStatement *returnStmt) {
    parsed.push_back("return:");
    returnStmt->acceptReturned(*this);
}

void parserVisitor::visitFunctionCallStatement(Nodes::FunctionCallStatement *funcCallStmt) {
    parsed.push_back("FunctionCall:"+funcCallStmt->getIdentifier());
    if(funcCallStmt->getArgList().has_value()) {
        parsed.push_back("arguments:");
        for(int i = 0; i< funcCallStmt->getArgList().value().size() ; i++) {
            funcCallStmt->getArgList().value()[i]->accept(*this);
        }
    }
}

void parserVisitor::visitDeclaration(Nodes::Declaration *declaration) {
    parsed.push_back("type:"+ Nodes::identiferTypes[declaration->getType()]);
    if(declaration->hasDefault()) {
        parsed.push_back("default:");
        declaration->acceptDefault(*this);
    }
}

void parserVisitor::visitFunctionDeclaration(Nodes::FunctionDeclaration *funcDecl) {
    parsed.push_back("type:" + Nodes::identiferTypes[funcDecl->getType()]);
    parsed.push_back("Body:");
    funcDecl->acceptFunctionBody(*this);
}

void parserVisitor::visitRelationalOperator(Nodes::RelationalOperator *relOp) {
    return;
}

void parserVisitor::visitTermOperator(Nodes::TermOperator *termOp) {
    return;
}

void parserVisitor::visitFactorOperator(Nodes::FactorOperator *factorOp) {
    return;
}
