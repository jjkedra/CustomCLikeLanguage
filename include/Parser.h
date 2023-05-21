//
// Created by Jan Kędra on 02/05/2023.
//

#ifndef LEXER_PARSER_H
#define LEXER_PARSER_H

#include <set>
#include "Lexer.h"
#include "Tree.h"

/**
 * @brief Parser is a class for parsing a sequence of tokens into an abstract syntax tree.
 * It contains functions for parsing expressions, statements, declarations, and operators.
 * The parser is used by the interpreter to parse the tokens generated by the lexer.
 * The parser is implemented as a recursive descent parser.
 * The functions in the class correspond to the grammar rules of the programming language.
 *
 * @note This class is part of a larger system for parsing and interpreting a programming language.
 *
 * @see Lexer, Token, AstVisitor, Tree.h
 */

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

    std::unique_ptr<Nodes::Dictionary> parseDictionary();
    idType dictionaryKeyType(Token);
    TokenType dictionaryValueType(Token);

    std::unique_ptr<Nodes::ArithmeticExpression> parseConcatenation();
    std::unique_ptr<Nodes::Term> parseStringTerm();
    std::unique_ptr<Nodes::Factor> parseStringFactor();
    std::unique_ptr<Nodes::String> parseString();

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
    std::variant<std::unique_ptr<Nodes::Number>, std::unique_ptr<Nodes::String>> parseMemberLiteral();

public:
    Parser(std::istream &code) : lexer_(code), currToken_(lexer_.nextToken()) {}
    std::unique_ptr<Nodes::Program> parseProgram();
};


#endif //LEXER_PARSER_H
