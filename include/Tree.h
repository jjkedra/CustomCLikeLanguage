//
// Created by Jan Kędra on 02/05/2023.
//

#ifndef LEXER_TREE_H
#define LEXER_TREE_H

#include <utility>
#include <map>
#include "Position.h"

/**
 * @brief Node is a class for storing the position of a character in a file.
 * It contains functions for returning the line and column of the character.
 *
 * @note This class is part of a larger system for parsing and interpreting a programming language.
 *
 * @see Lexer, Parser, AstVisitor, Tree.h
 */



typedef enum idType
{
    INT,
    FLOAT,
    STRING,
    DICT
} idType;

typedef enum termOperator
{
    PLUS,
    MINUS
} termOperator;

typedef enum factorOperator
{
    MULTIPLY,
    DIVIDE
} factorOperator;

typedef enum relationalOperator
{
    // COMPARISON
    EQUAL,              // ==
    NOTEQUAL,           // !=

    // LINQ
    LAMBDA,             // =>

    GREATER,            // >
    GREATEREQUAL,       // >=
    LESS,               // <
    LESSEQUAL,          // <=
} relationalOperator;

typedef enum NodeType
{
    RELOP,
    TERMOP,
    FACTOROP,
    SIMPLEEXPR,
    IDNODE,
    FUNCCALLARGLIST,
    FUNCCALL,
    FACTOR,
    TERM,
    ARITHMETICEXPR,
    NUMBERNODE,
    EXPR,
    STATEMENT,
    STATEMENTBLOCK,
    ASSIGNMENT,
    IFSTATEMENT,
    WHILESTATEMENT,
    RETURNSTATEMENT,
    FUNCCALLSTATEMENT,
    TYPENODE,
    DECLARATION,
    FUNCDECLARARGLIST,
    FUNCDECL,
    PROGRAM
} NodeType;

class AstVisitor;

class Node {
public:
    virtual ~Node() = default;
    virtual void accept(AstVisitor &) = 0;
    Position getPosition();

protected:
    Position position_;
    std::string nodeName_;
    friend std::ostream &operator<<(std::ostream &os, Node *);
};

namespace Nodes {
    static const std::vector<std::string> relationalOperators{
            "EQUALS", "NOT EQUALS", "GREATER", "GREATER OR EQUAL", "LESS", "LESS OR EQUAL"
    };

    static const std::vector<std::string> factorOperators{
            "MULTIPLY", "DIVIDE"
    };

    static const std::vector<std::string> identiferTypes{
            "INT", "FLOAT", "STRING", "DICT"
    };

    static const std::vector<std::string> termOperators{
            "PLUS", "MINUS"
    };


    class RelationalOperator : public Node
    {
        relationalOperator rop_;
    public:
        explicit RelationalOperator(relationalOperator rop_, Position pos) : rop_(rop_) {
            position_ = pos;
            nodeName_ = "Relational operator: " + relationalOperators[rop_];
        }
        relationalOperator getType() { return rop_; }
        void accept(AstVisitor &v);
    };

    class TermOperator : public Node
    {
        termOperator top_;
    public:
        explicit TermOperator(termOperator top_, Position pos) : top_(top_) {
            position_ = pos;
            nodeName_ = "Between operators: " + termOperators[top_];
        }
        termOperator getType() { return top_; }
        virtual void accept(AstVisitor &v);
    };

    class FactorOperator : public Node
    {
        factorOperator fop_;
    public:
        explicit FactorOperator(factorOperator fop_, Position pos) : fop_(fop_) {
            position_ = pos;
            nodeName_ = "Between operators: " + factorOperators[fop_];
        }
        factorOperator getType() { return fop_; }
        virtual void accept(AstVisitor &v);
    };

    class Factor : public Node {
    public:
        Factor() { nodeName_ = "Factor"; }

        virtual void accept(AstVisitor &v) = 0;
    };

    class Term : public Node
    {
        std::unique_ptr<Factor> left_;
        std::unique_ptr<FactorOperator> fop_;
        std::unique_ptr<Term> right_;
    public:
        Term(std::unique_ptr<Factor> l, std::unique_ptr<FactorOperator> fop, std::unique_ptr<Term> r, Position pos) {
            position_ = pos;
            nodeName_ = "Term";
            this->left_ = std::move(l);
            this->fop_ = std::move(fop);
            this->right_ = std::move(r);
        }
        void acceptLeft(AstVisitor &v) const;
        void acceptOperator(AstVisitor &v) const;
        void acceptRight(AstVisitor &v) const;
        FactorOperator *getOperator() const { return fop_.get(); }
        virtual void accept(AstVisitor &v);
    };

    class ArithmeticExpression : public Factor
    {
        std::unique_ptr<Term> left_;
        std::unique_ptr<TermOperator> op_;
        std::unique_ptr<ArithmeticExpression> right_;
    public:
        ArithmeticExpression(std::unique_ptr<Term> l, std::unique_ptr<TermOperator> op,
                             std::unique_ptr<ArithmeticExpression> r, Position pos) {
            position_ = pos;
            nodeName_ = "Arithmetic Expression";
            this->left_ = std::move(l);
            this->op_ = std::move(op);
            this->right_ = std::move(r);
        }
        void acceptLeft(AstVisitor &v) const;
        void acceptTermOperator(AstVisitor &v) const;
        void acceptRight(AstVisitor &v) const;
        bool hasLeft() const { return left_ != nullptr; }
        TermOperator *getTermOperator() const { return op_.get(); }
        virtual void accept(AstVisitor &v);
    };

    class String : public Factor
    {
        std::string value_;
    public:
        String(std::string s, Position pos) : Factor() {
            position_ = pos;
            nodeName_ = "String";
            this->value_ = s;
        }
        std::string getValue() const;
        idType getType() const;
        virtual void accept(AstVisitor &v);
    };

    class Number : public Factor
    {
        std::variant<int, float> value_;
    public:
        Number(std::variant<int, float> v, Position pos) : Factor() {
            position_ = pos;
            nodeName_ = "Number";
            this->value_ = v;
        }
        std::variant<int, float> getValue() const;
        idType getType() const;
        virtual void accept(AstVisitor &v);
    };

    class Dictionary : public Factor
    {
        idType key_;
        idType value_;
        std::vector<std::variant<std::unique_ptr<Nodes::Number>, std::unique_ptr<Nodes::String>>> keyValue_ = {};
        std::vector<std::variant<std::unique_ptr<Nodes::Number>, std::unique_ptr<Nodes::String>>> valueValue_ = {};
    public:
        Dictionary(idType k, idType v, Position pos,
                   std::vector<std::variant<std::unique_ptr<Nodes::Number>, std::unique_ptr<Nodes::String>>> kv = {},
                   std::vector<std::variant<std::unique_ptr<Nodes::Number>, std::unique_ptr<Nodes::String>>> vv = {}
                   ) : Factor() {
            position_ = pos;
            nodeName_ = "Dictionary";
            this->key_ = k;
            this->value_ = v;
            this->keyValue_ = std::move(kv);
            this->valueValue_ = std::move(vv);
        }
        std::vector<std::variant<std::unique_ptr<Nodes::Number>, std::unique_ptr<Nodes::String>>> const& getKeyValue();
        std::vector<std::variant<std::unique_ptr<Nodes::Number>, std::unique_ptr<Nodes::String>>> const& getValueValue();
        void acceptKey(AstVisitor &v) const;
        void acceptValue(AstVisitor &v) const;
        idType getKeyType() const;
        idType getValueType() const;
        virtual void accept(AstVisitor &v);
    };

    class FunctionCall : public Factor
    {
        std::vector<std::unique_ptr<ArithmeticExpression>> argList_;
        std::string identifier_;
    public:
        FunctionCall(std::string id, std::vector<std::unique_ptr<ArithmeticExpression>> al, Position pos) : Factor() {
            position_ = pos;
            nodeName_ = "Function call";
            this->identifier_ = std::move(id);
            this->argList_ = std::move(al);
        }
        std::optional<std::vector<ArithmeticExpression *>> getArgList() const;
        std::string getIdentifier() const;
        virtual void accept(AstVisitor &v);
    };
    class MemberReference : public Factor
    {
        std::string identifier_;
        std::string member_;
        std::variant<std::unique_ptr<Nodes::Number>, std::unique_ptr<Nodes::String>> memberLiteral_;
    public:
        MemberReference(std::string id, std::string m, std::variant<std::unique_ptr<Nodes::Number>, std::unique_ptr<Nodes::String>> ml, Position pos) : Factor() {
            position_ = pos;
            nodeName_ = "Member reference";
            this->identifier_ = std::move(id);
            this->member_ = std::move(m);
            this->memberLiteral_ = std::move(ml);
        }
        std::string getIdentifier() const;
        std::string getMember() const;
        std::variant<std::unique_ptr<Nodes::Number>, std::unique_ptr<Nodes::String>> const& getMemberLiteral();
        virtual void accept(AstVisitor &v);
    };

    class VariableReference : public Factor
    {
        std::string identifier_;
    public:
        explicit VariableReference(std::string id, Position pos) : Factor() {
            position_ = pos;
            nodeName_ = "Variable reference";
            this->identifier_ = std::move(id);
        }
        std::string getIdentifier() const;
        virtual void accept(AstVisitor &v);
    };

    class Expression : public Node
    {
        std::unique_ptr<ArithmeticExpression> left_, right_;
        std::unique_ptr<RelationalOperator> rop_;
    public:
        explicit Expression(std::unique_ptr<ArithmeticExpression> l, std::unique_ptr<RelationalOperator> rop,
                            std::unique_ptr<ArithmeticExpression> r, Position pos) : Node() {
            position_ = pos;
            nodeName_ = "Expression";
            this->left_ = std::move(l);
            this->rop_ = std::move(rop);
            this->right_ = std::move(r);
        }
        void acceptLeft(AstVisitor &v) const;
        void acceptRight(AstVisitor &v) const;
        void acceptOperator(AstVisitor &v) const;
        RelationalOperator *getOperator() const { return rop_.get(); }
        virtual void accept(AstVisitor &v);
    };

    class Statement : public Node
    {
    public:
        explicit Statement() : Node() {
            nodeName_ = "Statement";
        }
        virtual void accept(AstVisitor &v) = 0;
    };

    class LocalVariableDeclaration : public Statement
    {
        idType type_;
        std::string identifier_;
        std::variant<std::unique_ptr<ArithmeticExpression>, std::unique_ptr<String>, std::unique_ptr<Dictionary>> defaultValue_;

    public:
        LocalVariableDeclaration(idType t, std::string id, Position pos,
                                 std::variant<std::unique_ptr<ArithmeticExpression>, std::unique_ptr<String>, std::unique_ptr<Dictionary>> dv) : Statement()
        {
            position_ = pos;
            nodeName_ = "Local variable declaration";
            this->type_ = t;
            this->identifier_ = std::move(id);
            this->defaultValue_ = std::move(dv);
        }

        idType getType() const;
        std::string getIdentifier() const;
        bool hasDefault() const;
        void acceptDefault(AstVisitor &v) const;
        virtual void accept(AstVisitor &v);
    };

    class StatementBlock : public Node
    {
        std::vector<std::unique_ptr<Statement>> statement_;
    public:
        explicit StatementBlock(std::vector<std::unique_ptr<Statement>> s, Position pos) : Node() {
            position_ = pos;
            nodeName_ = "Statement block";
            this->statement_ = std::move(s);
        }
        std::vector<std::unique_ptr<Statement>> const& getStatements() const;
        void acceptStatements(AstVisitor &v) const;
        virtual void accept(AstVisitor &v);
    };

    class Assign : public Statement
    {
        std::string assignedIdentifier_;
        std::unique_ptr<ArithmeticExpression> assignedExpression_;
    public:
        explicit Assign(std::string id, std::unique_ptr<ArithmeticExpression> expre, Position pos) : Statement() {
            position_ = pos;
            nodeName_ = "Assign";
            this->assignedIdentifier_ = std::move(id);
            this->assignedExpression_ = std::move(expre);
        }

        std::string getAssignedIdentifier() const;
        void acceptAssignedExpression(AstVisitor &v) const;
        virtual void accept(AstVisitor &v);
    };

    class IfStatement : public Statement
    {
        std::unique_ptr<Expression> condition_;
        std::unique_ptr<StatementBlock> ifBlock_;
        std::unique_ptr<StatementBlock> elseBlock_;
    public:
        explicit IfStatement(std::unique_ptr<Expression> con, std::unique_ptr<StatementBlock> ib,
                             std::unique_ptr<StatementBlock> eb, Position pos) : Statement() {
            position_ = pos;
            nodeName_ = "If statement";
            this->condition_ = std::move(con);
            this->ifBlock_ = std::move(ib);
            this->elseBlock_ = std::move(eb);
        }
        void acceptCondition(AstVisitor &v) const;
        void acceptIfBlock(AstVisitor &v);
        void acceptElseBlock(AstVisitor &v);
        bool hasElseBlock() const { return elseBlock_ != nullptr; }
        virtual void accept(AstVisitor &v);
    };

    class WhileStatement : public Statement
    {
        std::unique_ptr<Expression> condition_;
        std::unique_ptr<StatementBlock> whileBlock_;
    public:
        explicit WhileStatement(std::unique_ptr<Expression> con, std::unique_ptr<StatementBlock> wh,
                                Position pos) : Statement() {
            position_ = pos;
            nodeName_ = "While statement";
            this->condition_ = std::move(con);
            this->whileBlock_ = std::move(wh);
        }
        void acceptCondition(AstVisitor &v);
        void acceptWhileBlock(AstVisitor &v);
        virtual void accept(AstVisitor &v);
    };

    class ReturnStatement : public Statement
    {
        std::unique_ptr<ArithmeticExpression> returnedExpression_;
    public:
        explicit ReturnStatement(Position pos, std::unique_ptr<ArithmeticExpression> re = nullptr) : Statement() {
            position_ = pos;
            nodeName_ = "Return statement";
            this->returnedExpression_ = std::move(re);
        }
        void acceptReturned(AstVisitor &v);
        virtual void accept(AstVisitor &v);
    };

    class FunctionCallStatement : public Statement
    {
        std::vector<std::unique_ptr<ArithmeticExpression>> argList_;
        std::string identifier_;
    public:
        explicit FunctionCallStatement(std::string id, std::vector<std::unique_ptr<ArithmeticExpression>> al,
                                       Position pos) : Statement() {
            position_ = pos;
            nodeName_ = "Function call";
            this->identifier_ = id;
            this->argList_ = std::move(al);
        }

        std::optional<std::vector<ArithmeticExpression *>> getArgList();

        std::string getIdentifier() const;

        virtual void accept(AstVisitor &v);
    };

    class Declaration : public Node
    {
        idType type_;
        std::string identifier_;
        std::variant<std::unique_ptr<ArithmeticExpression>, std::unique_ptr<String>, std::unique_ptr<Dictionary>> defaultValue_;

    public:
        explicit Declaration(idType t, std::string id, Position pos,
                             std::variant<std::unique_ptr<ArithmeticExpression>, std::unique_ptr<String>, std::unique_ptr<Dictionary>> dv) : Node() {
            position_ = pos;
            nodeName_ = "Declaration";
            this->type_ = t;
            this->identifier_ = std::move(id);
            this->defaultValue_ = std::move(dv);
        }

        idType getType() const;
        std::string getIdentifier();
        void acceptDefault(AstVisitor &v) const;
        bool hasDefault() const;
        virtual void accept(AstVisitor &v);
    };

    class FunctionDeclaration : public Node {
        idType type_;
        std::string identifier_;
        std::vector<std::unique_ptr<LocalVariableDeclaration>> argsDeclaration_;
        std::unique_ptr<StatementBlock> functionBody_;
    public:
        FunctionDeclaration(idType t, std::string id, std::vector<std::unique_ptr<LocalVariableDeclaration>> ad,
                            std::unique_ptr<StatementBlock> fb, Position pos) : Node() {
            position_ = pos;
            nodeName_ = "Function declaration";
            this->type_ = t;
            this->identifier_ = id;
            this->argsDeclaration_ = std::move(ad);
            this->functionBody_ = std::move(fb);
        }

        idType getType() const;
        std::string getIdentifier() const;
        std::optional<std::vector<LocalVariableDeclaration *>> getArgsDeclaration();
        std::unique_ptr<StatementBlock> const& getFunctionBody();
        void acceptFunctionBody(AstVisitor &v);
        virtual void accept(AstVisitor &v);
    };

    class Program : public Node
    {
        std::map<std::string, std::unique_ptr<Nodes::FunctionDeclaration>> functions_;
        std::map<std::string, std::unique_ptr<Nodes::Declaration>> variables_;
    public:
        explicit Program(std::map<std::string, std::unique_ptr<Nodes::FunctionDeclaration>> f,
                         std::map<std::string, std::unique_ptr<Nodes::Declaration>> v) : Node()
        {
            nodeName_ = "PROGRAM";
            this->functions_ = std::move(f);
            this->variables_ = std::move(v);
        }
        std::map<std::string , std::unique_ptr<Nodes::FunctionDeclaration>> const& getFunctions() const;
        std::map<std::string , std::unique_ptr<Nodes::Declaration>> const& getVariables() const;
        virtual void accept(AstVisitor &v);
    };
}

#endif //LEXER_TREE_H
