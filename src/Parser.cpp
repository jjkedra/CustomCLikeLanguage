//
// Created by Jan Kędra on 02/05/2023.
//

#include "Parser.h"
#include "LangException.h"

bool Parser::match(TokenType tt) {
    return (currToken_.getType() == tt);
}

void Parser::getNextToken() {
    currToken_ = lexer_.nextToken();
    while (currToken_.getType() == TokenType::Comment) {
        currToken_ = lexer_.nextToken();
    }
    if (currToken_.getType() == TokenType::Unknown) {
        throw LangException("Unknown token", currToken_.getPosition());
    }
}

void Parser::consumeToken(TokenType tt, const std::string &excMsg) {
    if (currToken_.getType() != tt) {
        throw LangException(excMsg, currToken_.getPosition());
    }
    getNextToken();
}

idType Parser::dictionaryType(TokenType t) {
    if (t == TokenType::IntegerToken) {
        getNextToken();
        return INT;
    } else if (t == TokenType::FloatToken) {
        getNextToken();
        return FLOAT;
    } else {
        throw LangException("Invalid type in dictionary declaration", currToken_.getPosition());
    }
}

//TODO type check
void Parser::parseDictionaryDeclaration() {
    std::vector<std::unique_ptr<Nodes::Number>> keyVal;
    std::vector<std::unique_ptr<Nodes::Number>> valueVal;
    getNextToken();
    size_t dict_size = 0;
    std::string identifier;

    if (currToken_.getType() != TokenType::OpenParenthesis) {
        throw LangException("Missing opening parenthesis on dictionary declaration", currToken_.getPosition());
    }
    getNextToken();

    idType type_key = dictionaryType(currToken_.getType());

    if (currToken_.getType() != TokenType::Comma) {
        throw LangException("Missing comma in dictionary declaration", currToken_.getPosition());
    }
    getNextToken();

    idType type_value = dictionaryType(currToken_.getType());

    if (currToken_.getType() != TokenType::ClosingParenthesis) {
        throw LangException("Missing closing parenthesis on dictionary declaration", currToken_.getPosition());
    }
    getNextToken();

    if (currToken_.getType() != TokenType::Identifier) {
        throw LangException("Missing identifier in dictionary declaration", currToken_.getPosition());
    }
    identifier = std::get<std::string>(currToken_.getValue());
    auto lookUpVar = variables_.find(identifier);
    if (lookUpVar != variables_.end()) {
        throw LangException("Variable redefinition", currToken_.getPosition());
    }
    auto lookUpFun = function_.find(identifier);
    if (lookUpFun != function_.end()) {
        throw LangException("Function redefinition", currToken_.getPosition());
    }
    
    getNextToken();

    if (currToken_.getType() != TokenType::OpenParenthesis) {
        std::unique_ptr<Nodes::Dictionary> dicPtr = std::make_unique<Nodes::Dictionary>(type_key, type_value,
                                                                                       currToken_.getPosition(),
                                                                                       std::move(keyVal), std::move(valueVal));
        variables_.insert(std::make_pair(identifier,
                                         std::make_unique<Nodes::Declaration>(DICT, identifier,
                                                                              currToken_.getPosition(),
                                                                              std::move(dicPtr))));
        getNextToken();
        return;
    }



    if (currToken_.getType() == TokenType::OpenParenthesis) {
        getNextToken();
        if (currToken_.getType() == TokenType::OpenBracket) {
            while (currToken_.getType() != TokenType::ClosingParenthesis) {
                getNextToken();
                keyVal.push_back(std::move(parseNumber()));

                // Comma
                if (currToken_.getType() != TokenType::Comma) {
                    throw LangException("Missing comma in dictionary value declaration", currToken_.getPosition());
                }
                getNextToken();
                valueVal.push_back(std::move(parseNumber()));

                // Closing bracket for pair
                if (currToken_.getType() != TokenType::ClosingBracket) {
                    throw LangException("Missing bracket in dictionary pair declaration", currToken_.getPosition());
                }

                getNextToken();
                if (currToken_.getType() == TokenType::Comma) {
                    getNextToken();
                }

                dict_size++;
                if (dict_size > 128) {
                    throw LangException("Dictionary too big or missing closing parenthesis", currToken_.getPosition());
                }
            }
        }
    }
    getNextToken();
    std::unique_ptr<Nodes::Dictionary> dicPtr = std::make_unique<Nodes::Dictionary>(type_key, type_value,
                                                                                    currToken_.getPosition(),
                                                                                    std::move(keyVal), std::move(valueVal));
    variables_.insert(std::make_pair(identifier,
                                     std::make_unique<Nodes::Declaration>(DICT, identifier,
                                                                          currToken_.getPosition(),
                                                                          std::move(dicPtr))));
    getNextToken();
    return;
}

bool Parser::parseFunctionOrDeclaration() {
    if (!match(TokenType::IntegerToken) && !match(TokenType::FloatToken) && !match(TokenType::StringToken)
    && !match(TokenType::DictToken)) {
        return false;
    } else {
        idType type = INT;
        if (currToken_.getType() == TokenType::FloatToken) {
            type = FLOAT;
        }
        if (currToken_.getType() == TokenType::StringToken) {
            type = STRING;
        }
        if (currToken_.getType() == TokenType::DictToken) {
            type = DICT;
            parseDictionaryDeclaration();
            getNextToken();
            return true;
        }

        getNextToken();

        if (!match(TokenType::Identifier)) {
            throw LangException("No identifier after type", currToken_.getPosition());
        }

        std::string identifier = std::get<std::string>(currToken_.getValue());
        auto lookUpVar = variables_.find(identifier);
        if (lookUpVar != variables_.end()) {
            throw LangException("Variable redefinition", currToken_.getPosition());
        }
        auto lookUpFun = function_.find(identifier);
        if (lookUpFun != function_.end()) {
            throw LangException("Function redefinition", currToken_.getPosition());
        }

        getNextToken();
        return (parseRestFunction(type, identifier) || parseRestDeclaration(type, identifier));
    }
}

bool Parser::parseRestFunction(idType type, std::string identifier) {
    if (!match((TokenType::OpenParenthesis))) {
        return false;
    }
    getNextToken();

    // Parsing args
    std::vector<std::unique_ptr<Nodes::LocalVariableDeclaration>> args = std::move(parseFunctionDeclarationArgList());
    consumeToken(TokenType::ClosingParenthesis, "Missing closing parenthesis!");

    // Moving declared arguments to set
    std::set<std::string> declaredIdentifiers;
    for (auto iter = args.begin(); iter != args.end(); iter++) {
        declaredIdentifiers.insert(iter->get()->getIdentifier());
    }

    // Parsing function body
    std::unique_ptr<Nodes::StatementBlock> statementBlock = std::move(parseStatementBlock());

    if (!statementBlock) {
        throw LangException("No body", currToken_.getPosition());
    }

    function_.insert(std::make_pair(identifier,
                                    std::make_unique<Nodes::FunctionDeclaration>
                                            (type, identifier,
                                             std::move(args),
                                             std::move(statementBlock),
                                             currToken_.getPosition())));
    return true;
}

bool Parser::parseRestDeclaration(idType type, std::string identifier) {
    std::unique_ptr<Nodes::ArithmeticExpression> defaultValue = nullptr;
    if (currToken_.getType() == TokenType::Assign) {
        getNextToken();
        if (currToken_.getType() == TokenType::StringLiteral) {
            defaultValue = std::move(parseConcatenation());
            variables_.insert(std::make_pair(identifier,
                                             std::make_unique<Nodes::Declaration>(type, identifier,
                                                      currToken_.getPosition(),
                                                      std::move(defaultValue))));
            return true;
        }
        defaultValue = std::move(parseArithmetic());
        if (!defaultValue) {
            throw LangException("No assignment after operator", currToken_.getPosition());
        }
    }
    consumeToken(TokenType::Semicolon, "Semicolon missing after declaration!");

    variables_.insert(std::make_pair(identifier,
                                     std::make_unique<Nodes::Declaration>(type, identifier,
                                                  currToken_.getPosition(),
                                                  std::move(defaultValue))));
    return true;
}

std::vector<std::unique_ptr<Nodes::LocalVariableDeclaration>> Parser::parseFunctionDeclarationArgList() {
    std::vector<std::unique_ptr<Nodes::LocalVariableDeclaration>> args;
    std::set<std::string> identifier;
    std::unique_ptr<Nodes::LocalVariableDeclaration> arg = parseLocalVariableDeclaration(identifier);

    if (!arg) {
        return args;
    } else {
        args.push_back(std::move(arg));
        while (currToken_.getType() == TokenType::Comma) {
            getNextToken();
            arg = parseLocalVariableDeclaration(identifier);
            if (!arg) {
                throw LangException("Expected next declaration after comma", currToken_.getPosition());
            }
            args.push_back(std::move(arg));
        }
        return args;
    }
}
std::unique_ptr<Nodes::String> Parser::parseString() {
    if (currToken_.getType() == TokenType::StringLiteral) {
        std::string value = std::get<std::string>(currToken_.getValue());
        getNextToken();
        return std::make_unique<Nodes::String>(value, currToken_.getPosition());
    }
    return nullptr;
}

std::unique_ptr<Nodes::Factor> Parser::parseStringFactor() {
    if (currToken_.getType() == TokenType::OpenParenthesis) {
        getNextToken();
        std::unique_ptr<Nodes::ArithmeticExpression> concatenationExpression = std::move(parseConcatenation());

        if (!concatenationExpression) {
            throw LangException("Invalid arithmetic expression", currToken_.getPosition());
        }
        consumeToken(TokenType::ClosingParenthesis, "Missing closing parenthesis");
        return concatenationExpression;
    }
    std::unique_ptr<Nodes::String> string = std::move(parseString());
    if (string) {
        return string;
    }
    return parseFunctionCallOrVariableRef();
}

std::unique_ptr<Nodes::Term> Parser::parseStringTerm() {
    std::unique_ptr<Nodes::Factor> left = std::move(parseStringFactor());
    if (!left) {
        return nullptr;
    }
    return std::make_unique<Nodes::Term>(std::move(left), nullptr, nullptr, currToken_.getPosition());

}

std::unique_ptr<Nodes::ArithmeticExpression> Parser::parseConcatenation() {
    std::unique_ptr<Nodes::Term> left = parseStringTerm();
    std::unique_ptr<Nodes::TermOperator> op = parseTermOperator();

    if (left) {
        if (!op) {
            return std::make_unique<Nodes::ArithmeticExpression>(std::move(left), nullptr, nullptr, currToken_.getPosition());
        }
    } else {
        if (!op) {
            return nullptr;
        } else {
            if (op) {
                throw LangException("Cannot put plus term before string", currToken_.getPosition());
            }
        }
    }

    std::unique_ptr<Nodes::ArithmeticExpression> right = parseConcatenation();

    if (!right) {
        throw LangException("Expected value after operator", currToken_.getPosition());
    }
    return std::make_unique<Nodes::ArithmeticExpression>(std::move(left), std::move(op), std::move(right), currToken_.getPosition());
}

std::unique_ptr<Nodes::ArithmeticExpression> Parser::parseArithmetic() {
    std::unique_ptr<Nodes::Term> left = parseTerm();
    std::unique_ptr<Nodes::TermOperator> op = parseTermOperator();

    if (left) {
        if (!op) {
            return std::make_unique<Nodes::ArithmeticExpression>(std::move(left), nullptr, nullptr, currToken_.getPosition());
        }
    } else {
        if (!op) {
            return nullptr;
        } else {
            if (op->getType() != termOperator::MINUS) {
                throw LangException("Cannot put plus before term", currToken_.getPosition());
            }
        }
    }
    std::unique_ptr<Nodes::ArithmeticExpression> right = parseArithmetic();

    if (!right) {
        throw LangException("Expected value after operator", currToken_.getPosition());
    }
    return std::make_unique<Nodes::ArithmeticExpression>(std::move(left), std::move(op), std::move(right), currToken_.getPosition());
}

std::unique_ptr<Nodes::Term> Parser::parseTerm() {
    std::unique_ptr<Nodes::Factor> left = std::move(parseFactor());
    if (!left) {
        return nullptr;
    }

    std::unique_ptr<Nodes::FactorOperator> op = std::move(parseFactorOperator());
    std::unique_ptr<Nodes::Term> right = std::move(parseTerm());

    if (right && !op) {
        throw LangException("Missing operator", currToken_.getPosition());
    } else if (op && !right) {
        throw LangException("Missing right side of term", currToken_.getPosition());
    }
    return std::make_unique<Nodes::Term>(std::move(left), std::move(op), std::move(right), currToken_.getPosition());
}

std::unique_ptr<Nodes::TermOperator> Parser::parseTermOperator() {
    if (currToken_.getType() == TokenType::Plus) {
        getNextToken();
        return std::make_unique<Nodes::TermOperator>(PLUS, currToken_.getPosition());
    } else if (currToken_.getType() == TokenType::Minus) {
        getNextToken();
        return std::make_unique<Nodes::TermOperator>(MINUS, currToken_.getPosition());
    }
    return nullptr;
}

std::unique_ptr<Nodes::Factor> Parser::parseFactor() {
    if (currToken_.getType() == TokenType::OpenParenthesis) {
        getNextToken();
        std::unique_ptr<Nodes::ArithmeticExpression> arithmeticExpression = std::move(parseArithmetic());

        if (!arithmeticExpression) {
            throw LangException("Invalid arithmetic expression", currToken_.getPosition());
        }
        consumeToken(TokenType::ClosingParenthesis, "Missing closing parenthesis");
        return arithmeticExpression;
    }
    std::unique_ptr<Nodes::Number> number = std::move(parseNumber());
    if (number) {
        return number;
    }
    return parseFunctionCallOrVariableRef();
}

std::unique_ptr<Nodes::FactorOperator> Parser::parseFactorOperator() {
    if (currToken_.getType() == TokenType::Multiply) {
        getNextToken();
        return std::make_unique<Nodes::FactorOperator>(MULTIPLY, currToken_.getPosition());
    } else if (currToken_.getType() == TokenType::Divide) {
        getNextToken();
        return std::make_unique<Nodes::FactorOperator>(DIVIDE, currToken_.getPosition());
    }
    return nullptr;
}

std::unique_ptr<Nodes::Number> Parser::parseNumber() {
    if (currToken_.getType() == TokenType::IntegerLiteral) {
        int value = std::get<int>(currToken_.getValue());
        getNextToken();
        return std::make_unique<Nodes::Number>(value, currToken_.getPosition());
    } else if (currToken_.getType() == TokenType::FloatLiteral) {
        float value = std::get<float>(currToken_.getValue());
        getNextToken();
        return std::make_unique<Nodes::Number>(value, currToken_.getPosition());
    }
    return nullptr;
}

std::unique_ptr<Nodes::Factor> Parser::parseFunctionCallOrVariableRef() {
    if (currToken_.getType() != TokenType::Identifier) {
        return nullptr;
    } else {
        std::string identifer = std::get<std::string>(currToken_.getValue());
        getNextToken();

        if (currToken_.getType() != TokenType::OpenParenthesis) {
            return std::make_unique<Nodes::VariableReference>(identifer, currToken_.getPosition());
        }
        getNextToken();
        std::vector<std::unique_ptr<Nodes::ArithmeticExpression>> args = std::move(parseFunctionArgList());
        consumeToken(TokenType::ClosingParenthesis, "Missing closing parenthesis");

        return std::make_unique<Nodes::FunctionCall>(identifer, std::move(args), currToken_.getPosition());
    }
}

std::vector<std::unique_ptr<Nodes::ArithmeticExpression>> Parser::parseFunctionArgList() {
    std::vector<std::unique_ptr<Nodes::ArithmeticExpression>> args;
    std::unique_ptr<Nodes::ArithmeticExpression> arg = std::move(parseArithmetic());
    if (arg) {
        args.push_back(std::move(arg));
    } else {
        return args;
    }
    while (currToken_.getType() == TokenType::Comma) {
        getNextToken();
        arg = std::move(parseArithmetic());
        if (!arg) {
            throw LangException("Invalid expression in argument list", currToken_.getPosition());
        }
        args.push_back(std::move(arg));
    }
    return std::move(args);
}

std::unique_ptr<Nodes::Expression> Parser::parseExpression() {
    std::unique_ptr<Nodes::ArithmeticExpression> left = std::move(parseArithmetic());
    if (!left) {
        return nullptr;
    }

    if (currToken_.getType() == TokenType::Equal ||
            currToken_.getType() == TokenType::GreaterEqual ||
            currToken_.getType() == TokenType::Greater ||
            currToken_.getType() == TokenType::LessEqual ||
            currToken_.getType() == TokenType::Less ||
            currToken_.getType() == TokenType::NotEqual
    ) {
        relationalOperator op;
        switch (currToken_.getType()) {
            case TokenType::Equal:
                op = relationalOperator::EQUAL;
                break;
            case TokenType::NotEqual:
                op = relationalOperator::NOTEQUAL;
                break;
            case TokenType::Greater:
                op = relationalOperator::GREATER;
                break;
            case TokenType::GreaterEqual:
                op = relationalOperator::GREATEREQUAL;
                break;
            case TokenType::Less:
                op = relationalOperator::LESS;
                break;
            case TokenType::LessEqual:
                op = relationalOperator::LESSEQUAL;
                break;
            default:
                return nullptr;
        }
        std::unique_ptr<Nodes::RelationalOperator> rop = std::make_unique<Nodes::RelationalOperator>(op, currToken_.getPosition());
        getNextToken();
        std::unique_ptr<Nodes::ArithmeticExpression> right = parseArithmetic();

        if (!right) {
            throw LangException("Invalid relational expression", currToken_.getPosition());
        }
        return std::make_unique<Nodes::Expression>(std::move(left), std::move(rop), std::move(right), currToken_.getPosition());
    }
    return std::make_unique<Nodes::Expression>(std::move(left), nullptr, nullptr, currToken_.getPosition());
}

std::unique_ptr<Nodes::StatementBlock> Parser::parseStatementBlock() {
    std::set<std::string> declaredIdentifiers;
    consumeToken(TokenType::OpenBracket, "Missing opening bracket");

    if (currToken_.getType() == TokenType::DictToken) {
        parseDictionaryDeclaration();
    }

    std::vector<std::unique_ptr<Nodes::Statement>> statements;
    std::unique_ptr<Nodes::Statement> statement;
    while ((statement = std::move(parseStatement(declaredIdentifiers)))) {
        statements.push_back(std::move(statement));
    }
    consumeToken(TokenType::ClosingBracket, "Missing closing bracket");
    return std::make_unique<Nodes::StatementBlock>(std::move(statements), currToken_.getPosition());
}

std::unique_ptr<Nodes::Statement> Parser::parseStatement(std::set<std::string> &declaredIdentifiers) {
    std::unique_ptr<Nodes::Statement> statement;


    statement = std::move(parseIfStatement());
    if (statement) { return statement; }

    statement = std::move(parseWhileStatement());
    if (statement) { return statement; }

    statement = std::move(parseReturnStatement());
    if (statement) {
        consumeToken(TokenType::Semicolon, "Missing semicolon");
        return statement;
    }

    statement = parseIdentifierStatement();
    if (statement) {
        consumeToken(TokenType::Semicolon, "Missing semicolon");
        return statement;
    }

    statement = parseLocalVariableDeclaration(declaredIdentifiers);
    if (statement) {
        consumeToken(TokenType::Semicolon, "Missing semicolon");
        return statement;
    }

    return nullptr;
}

std::unique_ptr<Nodes::IfStatement> Parser::parseIfStatement() {
    if (currToken_.getType() != TokenType::If) {
        return nullptr;
    } else {
        getNextToken();
        consumeToken(TokenType::OpenParenthesis, "Missing opening parenthesis next to if keyword");

        std::unique_ptr<Nodes::Expression> expression = std::move(parseExpression());
        if (!expression) {
            throw LangException("Invalid expression in if statement", currToken_.getPosition());
        }
        consumeToken(TokenType::ClosingParenthesis, "Missing closing parenthesis next to if keyword");
        std::unique_ptr<Nodes::StatementBlock> ifBlock = std::move(parseStatementBlock());
        if (!ifBlock) {
            throw LangException("Invalid body of if statement", currToken_.getPosition());
        }
        std::unique_ptr<Nodes::StatementBlock> elseBlock = nullptr;
        if (currToken_.getType() == TokenType::Else) {
            getNextToken();
            elseBlock = parseStatementBlock();
            if (!elseBlock) {
                throw LangException("Invalid body of else statement", currToken_.getPosition());
            }
        }
        return std::make_unique<Nodes::IfStatement>(std::move(expression), std::move(ifBlock),
                                                    std::move(elseBlock), currToken_.getPosition());
    }
}

std::unique_ptr<Nodes::WhileStatement> Parser::parseWhileStatement() {
    if (currToken_.getType() != TokenType::While) {
        return nullptr;
    } else {
        getNextToken();
        consumeToken(TokenType::OpenParenthesis, "Missing opening parenthesis after keyword");

        std::unique_ptr<Nodes::Expression> expression = std::move(parseExpression());
        if (!expression) {
            throw LangException("Invalid expression in while statement", currToken_.getPosition());
        }
        consumeToken(TokenType::ClosingParenthesis, "Missing closing parenthesis after keyword");
        std::unique_ptr<Nodes::StatementBlock> whileBlock = std::move(parseStatementBlock());
        if (!whileBlock) {
            throw LangException("Invalid body of while statement", currToken_.getPosition());
        }
        return std::make_unique<Nodes::WhileStatement>(std::move(expression), std::move(whileBlock),
                                                       currToken_.getPosition());
    }
}

std::unique_ptr<Nodes::ReturnStatement> Parser::parseReturnStatement() {
    if (currToken_.getType() != TokenType::Return) {
        return nullptr;
    } else {
        getNextToken();
        if (currToken_.getType() == TokenType::Semicolon) {
            return std::make_unique<Nodes::ReturnStatement>(currToken_.getPosition());
        }
        std::unique_ptr<Nodes::ArithmeticExpression> expression = std::move(parseArithmetic());
        if (!expression) {
            throw LangException("Invalid return statement", currToken_.getPosition());
        }
        return std::make_unique<Nodes::ReturnStatement>(currToken_.getPosition(), std::move(expression));
    }
}

std::unique_ptr<Nodes::Statement> Parser::parseIdentifierStatement() {
    if (currToken_.getType() != TokenType::Identifier) {
        return nullptr;
    } else {
        std::string identifier = std::get<std::string>(currToken_.getValue());
        getNextToken();

        if (currToken_.getType() == TokenType::OpenParenthesis) {
            getNextToken();
            std::vector<std::unique_ptr<Nodes::ArithmeticExpression>> args = std::move(parseFunctionArgList());
            consumeToken(TokenType::ClosingParenthesis, "Missing closing parenthesis");
            return std::make_unique<Nodes::FunctionCallStatement>(identifier, std::move(args), currToken_.getPosition());
        }
        consumeToken(TokenType::Assign, "Missing assignment sign");
        std::unique_ptr<Nodes::ArithmeticExpression> assignedValue = parseArithmetic();
        if (!assignedValue) {
            throw LangException("Invalid assignment value", currToken_.getPosition());
        }
        return std::make_unique<Nodes::Assign>(identifier, std::move(assignedValue), currToken_.getPosition());
    }
}

std::unique_ptr<Nodes::LocalVariableDeclaration> Parser::parseLocalVariableDeclaration(std::set<std::string> & declaredIdentifers) {
    if (!match(TokenType::IntegerToken) && !match(TokenType::FloatToken) && !match(TokenType::StringToken) && !match(TokenType::DictToken)) {
        return nullptr;
    } else {
        idType type;
        if (currToken_.getType() == TokenType::FloatToken) {
            type = idType::FLOAT;
        }
        if (currToken_.getType() == TokenType::IntegerToken) {
            type = idType::INT;
        }
        if (currToken_.getType() == TokenType::StringToken) {
            type = idType::STRING;
        }
        getNextToken();
        if (currToken_.getType() != TokenType::Identifier) {
            throw LangException("Invalid local variable declaration", currToken_.getPosition());
        }
        std::string identifier = std::get<std::string>(currToken_.getValue());
        if (!declaredIdentifers.insert(identifier).second) {
            throw LangException("Redefinition of local variable", currToken_.getPosition());
        }
        getNextToken();
        std::unique_ptr<Nodes::ArithmeticExpression> defaultValue = nullptr;
        if (currToken_.getType() == TokenType::Assign) {
            getNextToken();
            if (currToken_.getType() == TokenType::StringLiteral) {
                defaultValue = std::move(parseConcatenation());
            } else {
                defaultValue = std::move(parseArithmetic());
                if (!defaultValue) {
                    throw LangException("Invalid default value", currToken_.getPosition());
                }
            }
        }
        return std::make_unique<Nodes::LocalVariableDeclaration>(type, identifier,
                                                                 currToken_.getPosition(), std::move(defaultValue));
    }
}

std::unique_ptr<Nodes::Program> Parser::parseProgram() {
    while (currToken_.getType() == TokenType::Comment) {
        getNextToken();
    }
    while (parseFunctionOrDeclaration()) {}

    return std::make_unique<Nodes::Program>(std::move(function_), std::move(variables_));
}
