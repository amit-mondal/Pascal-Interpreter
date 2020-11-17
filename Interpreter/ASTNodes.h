#ifndef ASTNODES_H
#define ASTNODES_H


#include "Token.h"
#include "Symbol.h"
#include "utils.h"
#include "DataVal.h"

/*
 * Define node types
 */

enum NodeType {
	       none = 0,
	       binOp,
	       num,
	       stringLiteral,
	       unaryOp,
	       assign,
	       var,
	       compound,
	       program,
	       block,
	       varDecl,
	       varType,
	       procedureDecl,
	       param,
	       procedureCall,
	       ifStatement,
	       whileStatement,
	       recordDecl,
	       returnStatement
};

class ScopedSymbolTable;


//Pure abstract class AST
class AST {
public:
    virtual NodeType type() const = 0;
    virtual bool isLiteral();
    int line = -2;
};

//Binary operation node
class BinOp: public AST {
public:
    AST* left;
    AST* right;
    Token* op;
    BinOp(AST* left, Token* op, AST* right);
    NodeType type() const;
};

//Number leaf node
class Num: public AST {
public:
    DataVal value;
    Token* token;
    Num(Token* token);
    virtual NodeType type() const;
};

class StringLiteral: public AST {
public:
    DataVal value;
    Token* token;
    StringLiteral(Token* token);
    virtual NodeType type() const;
};

//Unary operation node
class UnaryOp: public AST {
public:
    Token* op;
    Token* token;
    AST* expr;
    UnaryOp(Token* op, AST* expr);
    virtual NodeType type() const;
};

//Compound statement node
class Compound: public AST {
public:
    std::vector<AST*> children;
    Compound();
    virtual NodeType type() const;
};

//Assignment statement node
class Assign: public AST {
public:
    AST* left;
    AST* right;
    Token* token;
    Token* op;
    Assign(AST* left, Token* op, AST* right);
    virtual NodeType type() const;
};

//Variable node
class Var: public AST {
public:
    Token* token;
    TokenVal value;
    Var(Token* token);
    virtual NodeType type() const;
};

//Empty statement node
class NoOp: public AST {
public:
    NodeType type() const;
};


//Program node
class Program: public AST {
public:
    std::string name;
    AST* block;
    Program(std::string name, AST* block);
    ScopedSymbolTable* table;
    virtual NodeType type() const;
};

//Block node
class Block: public AST {
public:
    std::vector<AST*> declarations;
    AST* compoundStatement;
    Block(std::vector<AST*>& declarations, AST* compoundStatement);
    virtual NodeType type() const;
};

class Type: public AST {
public:
    Token* token;
    TokenVal value;
    Type(Token* token);
    virtual NodeType type() const;
};

//Var declaration node
class VarDecl: public AST {
public:
    Var* varNode;
    Type* typeNode;
    VarDecl(Var* varNode, Type* typeNode);
    virtual NodeType type() const;
};

class Param: public AST {
public:
    Var* varNode;
    Type* typeNode;
    Param(Var* varNode, Type* typeNode);
    virtual NodeType type() const;
};

class ProcedureDecl: public AST {
public:
    std::string procName;
    AST* blockNode;
    Type* returnTypeNode;
    std::vector<Param*>* params;
    ScopedSymbolTable* table;
    ProcedureDecl(std::string procName, std::vector<Param*>* params, AST* blockNode, Type* returnType);
    virtual NodeType type() const;
};

class RecordDecl: public AST {
 public:
    std::string recordName;
    std::unordered_map<std::string, std::pair<Type*, int> > memberIndex;
    RecordDecl(std::string recordName, std::vector<AST*>* memberDecls);
    virtual NodeType type() const;
};

class ProcedureCall: public AST {
public:
    ProcedureCall(std::string procName, std::vector<AST*>* paramVals);
    std::string procName;
    std::vector<AST*>* paramVals;
    AST* procDeclNode;
    virtual NodeType type() const;
};

class IfStatement: public AST {
public:
    IfStatement(AST* conditionNode, AST* blockNode, AST* elseBranch);
    AST* conditionNode;
    AST* blockNode;
    AST* elseBranch;
    virtual NodeType type() const;
};

class WhileStatement: public AST {
public:
    WhileStatement(AST* conditionNode, AST* blockNode);
    AST* conditionNode;
    AST* blockNode;
    virtual NodeType type() const;
};

class ReturnStatement: public AST {
public:
    ReturnStatement(AST* expr, ProcedureDecl* procDecl);
    AST* expr;
    ProcedureDecl* procDecl;
    virtual NodeType type() const;
};

#endif
