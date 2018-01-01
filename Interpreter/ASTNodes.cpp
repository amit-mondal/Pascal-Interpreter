#include "ASTNodes.h"

using namespace std;

BinOp::BinOp(AST* left, Token* op, AST* right) {
    this->left = left;
    this->op = op;
    this->right = right;
}

NodeType BinOp::type() const {
    return NodeType::binOp;
}

Num::Num(Token* token) {
    this->token = token;
    this->value = token->value.numVal;
}

NodeType Num::type() const {
    return NodeType::num;
}

UnaryOp::UnaryOp(Token* op, AST* expr) {
    this->token = op;
    this->op = op;
    this->expr = expr;
}

NodeType UnaryOp::type() const {
    return NodeType::unaryOp;
}

Compound::Compound() {
    children = {};
}

NodeType Compound::type() const {
    return NodeType::compound;
}

Assign::Assign(AST* left, Token* op, AST* right) {
    this->left = left;
    this->right = right;
    this->op = op;
    this->token = op;
}

NodeType Assign::type() const {
    return NodeType::assign;
}

Var::Var(Token* token) : token(token), value(token->value) {
    
}

NodeType Var::type() const {
    return NodeType::var;
}

NodeType NoOp::type() const {
    return NodeType::none;
}

Program::Program(string name, AST* block) {
    this->name = name;
    this->block = block;
}

NodeType Program::type() const {
    return NodeType::program;
}

Block::Block(vector<AST*>& declarations, AST* compoundStatement) {
    this->declarations = declarations;
    this->compoundStatement = compoundStatement;
}

NodeType Block::type() const {
    return NodeType::block;
}

VarDecl::VarDecl(AST* varNode, AST* typeNode) {
    this->varNode = varNode;
    this->typeNode = typeNode;
}

NodeType VarDecl::type() const {
    return NodeType::varDecl;
}

Type::Type(Token* token) : token(token), value(token->value) {
}

NodeType Type::type() const {
    return NodeType::varType;
}

ProcedureDecl::ProcedureDecl(string procName, vector<AST*>* params, AST* blockNode) : procName(procName), blockNode(blockNode), params(params) {
    table = nullptr;
}

NodeType ProcedureDecl::type() const {
    return NodeType::procedureDecl;
}

Param::Param(AST* varNode, AST* typeNode) : varNode(varNode), typeNode(typeNode) {
}

NodeType Param::type() const {
    return NodeType::param;
}

ProcedureCall::ProcedureCall(string procName, vector<AST*>* paramVals) : procName(procName), paramVals(paramVals) {
    procDeclNode = nullptr;
}

NodeType ProcedureCall::type() const {
    return NodeType::procedureCall;
}

IfStatement::IfStatement(AST* conditionNode, AST* blockNode, AST* elseBranch) : conditionNode(conditionNode), blockNode(blockNode), elseBranch(elseBranch) {
}

NodeType IfStatement::type() const {
    return NodeType::ifStatement;
}

WhileStatement::WhileStatement(AST* conditionNode, AST* blockNode) : conditionNode(conditionNode), blockNode(blockNode) {
}

NodeType WhileStatement::type() const {
    return NodeType::whileStatement;
}