#ifndef SEMANTIC_ANALYZER_H
#define SEMANTIC_ANALYZER_H

#include <map>
#include <functional>
#include <string>
#include "Symbol.h"
#include "ASTNodes.h"
#include "ScopedSymbolTable.h"

class SemanticAnalyzer {
public:
    SemanticAnalyzer();
    Symbol* visit(AST* node);
private:
    ScopedSymbolTable* currentScope;
    std::map<ProcedureSymbol*, AST*> procedureTable;
    void error(const std::string& err, int line);
    Symbol* visitBlock(AST* node);
    Symbol* visitProgram(AST* node);
    Symbol* visitCompound(AST* node);
    Symbol* visitBinOp(AST* node); 
    Symbol* visitUnaryOp(AST* node);   
    Symbol* visitVarDecl(AST* node);
    Symbol* visitRecordDecl(AST* node);
    Symbol* visitAssign(AST* node);
    Symbol* visitVar(AST* node);
    Symbol* visitNum(AST* node);
    Symbol* visitStringLiteral(AST* node);
    Symbol* visitProcedureDecl(AST* node);
    Symbol* visitProcedureCall(AST* node);
    Symbol* visitIfStatement(AST* node);
    Symbol* visitWhileStatement(AST* node);
    Symbol* visitReturnStatement(AST* node);
    bool resolveTypes(Symbol* lhs, Symbol* rhs, int line);

    const std::map<int, std::function<Symbol*(SemanticAnalyzer*, AST*)> > visitorTable = {
		    { block, &SemanticAnalyzer::visitBlock },
		    { program, &SemanticAnalyzer::visitProgram },
		    { compound, &SemanticAnalyzer::visitCompound },
		    { binOp, &SemanticAnalyzer::visitBinOp },
		    { unaryOp, &SemanticAnalyzer::visitUnaryOp },
		    { varDecl, &SemanticAnalyzer::visitVarDecl },
		    { recordDecl, &SemanticAnalyzer::visitRecordDecl },
		    { assign, &SemanticAnalyzer::visitAssign },
		    { var, &SemanticAnalyzer::visitVar },
		    { num, &SemanticAnalyzer::visitNum },		    
		    { stringLiteral, &SemanticAnalyzer::visitStringLiteral },
		    { procedureDecl, &SemanticAnalyzer::visitProcedureDecl },
		    { procedureCall, &SemanticAnalyzer::visitProcedureCall, },
		    { ifStatement, &SemanticAnalyzer::visitIfStatement },
		    { whileStatement, &SemanticAnalyzer::visitWhileStatement },
		    { returnStatement, &SemanticAnalyzer::visitReturnStatement }
    };    
};

#endif
