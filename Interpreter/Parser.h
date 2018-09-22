
#ifndef PARSER_H
#define PARSER_H

#include <string>
#include <vector>
#include <unordered_set>
#include "ASTNodes.h"
#include "Lexer.h"

/****************************************
 Parser
***************************************/

class Parser {
public:
    Parser(Lexer* lexer);
    int line();
    void error(std::string errmsg);
    void eat(std::string tokenType);
    AST* program();
    AST* block();
    std::vector<AST*>* declarations();
    std::vector<Param*>* formalParameters();
    std::vector<Param*>* formalParameterList();
    std::vector<AST*>* variableDeclarations();
    Type* typeSpec();
    AST* compoundStatement();
    std::vector<AST*>* statementList();
    AST* statement();
    AST* assignmentStatement();
    AST* variable();
    AST* empty();
    AST* expr();
    AST* term();
    AST* factor();
    AST* parse();
    AST* procedureCall();
    std::vector<AST*>* actualParameters();
    AST* ifStatement(bool isElseIf = false);
    AST* whileStatement();
private:
    std::unordered_set<std::string> validTypes = {ttype::integer, ttype::real, ttype::string, ttype::any};
    Lexer* lexer;
    Token* currentToken;
};

#endif
