//
// Created by Jan Kędra on 02/05/2023.
//

#ifndef LEXER_PARSER_H
#define LEXER_PARSER_H

#include <set>
#include "Lexer.h"
#include "Tree.h"

class Parser {
    Lexer lexer_;
    Token currToken_;

    std::map<std::string, std::unique_ptr<Nodes::FunctionDeclaration>> function_;
    std::map<std::string, std::unique_ptr<Nodes::Declaration>> variables_;

    bool match(TokenType);

    void getNextToken();
    void consumeToken(TokenType, const std::string &);

    bool parseFunctionOrDeclaration();
    bool parseRestFunction(idType, std::string);
    bool parseRestDeclaration(idType, std::string);

    std::vector<std::unique_ptr<Nodes::LocalVariableDeclaration>> parseFunctionDeclarationArgList();
    std::unique_ptr<Nodes::Declaration> parseDeclaration();
    std::unique_ptr<Nodes::ArithmeticExpression> parseArithmetic();
    std::unique_ptr<Nodes::Term> parseTerm();
    std::unique_ptr<Nodes::TermOperator> parseTermOperator();
    std::unique_ptr<Nodes::Factor> parseFactor();
    std::unique_ptr<Nodes::FactorOperator> parseFactorOperator();
    std::unique_ptr<Nodes::Number> parseNumber();
    std::unique_ptr<Nodes::Factor> parseFunctionCallOrVariableRef();
    std::vector<std::unique_ptr<Nodes::ArithmeticExpression>> parseFunctionArgList();
    std::unique_ptr<Nodes::Expression> parseExpression();
    std::unique_ptr<Nodes::StatementBlock> parseStatementBlock();
    std::unique_ptr<Nodes::Statement> parseStatement(std::set<std::string> &);
    std::unique_ptr<Nodes::IfStatement> parseIfStatement();
    std::unique_ptr<Nodes::WhileStatement> parseWhileStatement();
    std::unique_ptr<Nodes::ReturnStatement> parseReturnStatement();
    std::unique_ptr<Nodes::Statement> parseIdentifierStatement();
    std::unique_ptr<Nodes::LocalVariableDeclaration> parseLocalVariableDeclaration(std::set<std::string> &);
public:
    Parser(std::istream &code) : lexer_(code), currToken_(lexer_.nextToken()) {}
    std::unique_ptr<Nodes::Program> parseProgram();
};


#endif //LEXER_PARSER_H
