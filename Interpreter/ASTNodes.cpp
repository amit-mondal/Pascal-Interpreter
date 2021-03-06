#include "ASTNodes.h"

using namespace std;

bool AST::isLiteral() {
    switch (this->type()) {
    case NodeType::stringLiteral:
    case NodeType::num:
	return true;
    default:
	return false;
    }
}

BinOp::BinOp(AST* left, Token* op, AST* right) : left(left), op(op), right(right) {
}

NodeType BinOp::type() const {
    return NodeType::binOp;
}

Num::Num(Token* token) : value(DataVal::allocator.allocate(token->value.numVal)), token(token) {
}

NodeType Num::type() const {
    return NodeType::num;
}

StringLiteral::StringLiteral(Token* token) : value(DataVal::allocator.allocate(token->value.strVal)), token(token) {
}

NodeType StringLiteral::type() const {
    return NodeType::stringLiteral;
}

UnaryOp::UnaryOp(Token* op, AST* expr) : op(op), token(op), expr(expr) {
}

NodeType UnaryOp::type() const {
    return NodeType::unaryOp;
}

Compound::Compound() {
}

NodeType Compound::type() const {
    return NodeType::compound;
}

Assign::Assign(AST* left, Token* op, AST* right) : left(left), right(right), token(op), op(op) {
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

Program::Program(string name, AST* block) : name(name), block(block) {
}

NodeType Program::type() const {
    return NodeType::program;
}

Block::Block(vector<AST*>& declarations, AST* compoundStatement) : declarations(declarations),
								   compoundStatement(compoundStatement) {
}

NodeType Block::type() const {
    return NodeType::block;
}

VarDecl::VarDecl(Var* varNode, Type* typeNode) : varNode(varNode), typeNode(typeNode) {   
}

NodeType VarDecl::type() const {
    return NodeType::varDecl;
}

Type::Type(Token* token) : token(token), value(token->value) {
}

NodeType Type::type() const {
    return NodeType::varType;
}

ProcedureDecl::ProcedureDecl(string procName, vector<Param*>* params, AST* blockNode, Type* returnType) : procName(procName), blockNode(blockNode), returnTypeNode(returnType), params(params) {
    table = nullptr;
}

NodeType ProcedureDecl::type() const {
    return NodeType::procedureDecl;
}

RecordDecl::RecordDecl(string recordName, vector<AST*>* memberDecls) : recordName(recordName) {
    Var* varNode;
    VarDecl* v;
    for (size_t i = 0;i < memberDecls->size(); i++) {
	v = dynamic_cast<VarDecl*>(memberDecls->at(i));
	varNode = dynamic_cast<Var*>(v->varNode);
	this->memberIndex[varNode->value.strVal] =
	    make_pair(dynamic_cast<Type*>(v->typeNode), i);
    }
    // We won't need this anymore.
    delete memberDecls;
}

NodeType RecordDecl::type() const {
    return NodeType::recordDecl;
}


Param::Param(Var* varNode, Type* typeNode) : varNode(varNode), typeNode(typeNode) {
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

ReturnStatement::ReturnStatement(AST* expr, ProcedureDecl* procDecl) : expr(expr), procDecl(procDecl)  {}

NodeType ReturnStatement::type() const {
    return NodeType::returnStatement;
}
