#include "SemanticAnalyzer.h"
#include "builtins.h"

using namespace std;

SemanticAnalyzer::SemanticAnalyzer() : currentScope(nullptr) {
}

void SemanticAnalyzer::error(const string& err, int line) {
    utils::fatalError("Semantic error on line " + to_string(line) + ": " + err);
}

bool SemanticAnalyzer::resolveTypes(Symbol* lhs, Symbol* rhs, int line) {

    if (lhs->name == ttype::any) {
	return true;
    }
    
    bool res = lhs->name == rhs->name;
    if (!res) {
	this->error("type mismatch between value of type " + lhs->name + " and value of type " + rhs->name, line);
    }
    return res;
}

Symbol* SemanticAnalyzer::visit(AST* node) {
    if (node == nullptr) utils::fatalError(string("Parse tree is null"));

    if (node->type() == none) {
	return nullptr;
    }

    auto itr = this->visitorTable.find(node->type());
    if (itr != this->visitorTable.end()) {
	/*
	  "this" is an implied first argument for non-static methods
	*/
	return itr->second(this, node);
    } else {
	this->error("No visitor for node of type index " + to_string(node->type()), node->line);
    }

    return nullptr;
}

Symbol* SemanticAnalyzer::visitBlock(AST* node) {
    Block* blockNode = dynamic_cast<Block*>(node);
    for (AST* declaration : blockNode->declarations) {
	this->visit(declaration);
    }
    this->visit(blockNode->compoundStatement);
    return nullptr;
}

Symbol* SemanticAnalyzer::visitProgram(AST* node) {
    if (options::showST) {
	cout << "ENTER scope: global" << endl;
    }
    ScopedSymbolTable* globalScope = new ScopedSymbolTable("global", 1, currentScope);
    currentScope = globalScope;
    Program* progNode = dynamic_cast<Program*>(node);
    this->visit(progNode->block);
    if (options::showST) {
	cout << globalScope->toString() << endl;
	cout << "LEAVE scope: global" << endl;
    }
    progNode->table = currentScope;
    currentScope = currentScope->enclosingScope;
    return nullptr;
}

Symbol* SemanticAnalyzer::visitCompound(AST* node) {
    Compound* compNode = dynamic_cast<Compound*>(node);
    for (AST* child : compNode->children) {
	this->visit(child);
    }
    return nullptr;
}

Symbol* SemanticAnalyzer::visitBinOp(AST* node) {
    BinOp* binOpNode = dynamic_cast<BinOp*>(node);
    Symbol* lhs = this->visit(binOpNode->left);
    Symbol* rhs = this->visit(binOpNode->right);
    this->resolveTypes(lhs, rhs, binOpNode->line);
    // For now, assume a binary operation's result is the same as its operands.
    return lhs;
}

Symbol* SemanticAnalyzer::visitVarDecl(AST* node) {
    VarDecl* varDeclNode = dynamic_cast<VarDecl*>(node);
    Var* varNode = varDeclNode->varNode;
    Type* typeNode = varDeclNode->typeNode;
    string typeName = typeNode->value.strVal;
    Symbol* typeSymbol;
    if (!(typeSymbol = currentScope->lookup(typeName))) {
	this->error("no type symbol found for type name " + typeName, varNode->token->line);
    }
    string varName = varNode->value.strVal;
    Symbol* varSymbol;
    if ((varSymbol = currentScope->lookup(varName)) && varSymbol->type != nullptr) {
	this->error("duplicate identifier " + varName, varNode->token->line);
    }
    currentScope->define(new VarSymbol(varName, typeSymbol));
    return nullptr;
}

Symbol* SemanticAnalyzer::visitRecordDecl(AST* node) {
    RecordDecl* recordDeclNode = dynamic_cast<RecordDecl*>(node);
    currentScope->define(new UserDefinedTypeSymbol(recordDeclNode->recordName));
    return nullptr;
}

Symbol* SemanticAnalyzer::visitAssign(AST* node) {
    Assign* assignNode = dynamic_cast<Assign*>(node);
    // Make sure we're not assigning to a literal value.
    if (assignNode->left->isLiteral()) {
	this->error("Cannot assign to literal", assignNode->line);
    }
    auto lhs = this->visit(assignNode->left);
    auto rhs = this->visit(assignNode->right);
    if (!rhs) {
	this->error("The right hand side does not return a value", assignNode->line);
    }
    this->resolveTypes(lhs,
		       rhs,
		       assignNode->line);
    return nullptr;
}

Symbol* SemanticAnalyzer::visitVar(AST* node) {
    Var* varNode = dynamic_cast<Var*>(node);
    Symbol* _varSymbol;
    if (!( _varSymbol = currentScope->lookup(varNode->value.strVal))) {
	this->error("symbol not found for variable " + varNode->value.strVal, varNode->token->line);
    }
    VarSymbol* varSymbol = dynamic_cast<VarSymbol*>(_varSymbol);
    if (!varSymbol) {
	this->error("Cannot use symbol \"" + _varSymbol->name + "\" of type \"" + string(Symbol::TYPE_TO_NAME[_varSymbol->stype()]) + "\" as a variable name", node->line);
    }
    return varSymbol->type;    
}

Symbol* SemanticAnalyzer::visitNum(__attribute__((unused)) AST* node) {
    return GET_BUILT_IN_SYMBOL(REAL);
}

Symbol* SemanticAnalyzer::visitUnaryOp(AST* node) {
    UnaryOp* unaryOp = dynamic_cast<UnaryOp*>(node);
    auto numType = this->visit(unaryOp->expr);

    if (numType == GET_BUILT_IN_SYMBOL(REAL) ||
	numType == GET_BUILT_IN_SYMBOL(INT)) {
	return numType;
    }

    string chStr;
    chStr += unaryOp->op->value.charVal;

    this->error("Unary operator \"" + chStr + "\" can only be used on numeric types", node->line);
    return nullptr;
}

Symbol* SemanticAnalyzer::visitStringLiteral(__attribute__((unused)) AST* node) {
    return GET_BUILT_IN_SYMBOL(STRING);
}

Symbol* SemanticAnalyzer::visitProcedureDecl(AST* node) {
    ProcedureDecl* procDecNode = dynamic_cast<ProcedureDecl*>(node);
    string procName = procDecNode->procName;
    ProcedureSymbol* procSymbol = new ProcedureSymbol(procName);

    if (currentScope->lookup(procName)) {
	this->error("redefinition of procedure " + procName, procDecNode->line);
    }
	
    currentScope->define(procSymbol);
    procedureTable[procSymbol] = procDecNode;
    if (options::showST) {
	cout << "ENTER scope: " << procName << endl;
    }
    ScopedSymbolTable* procedureScope = new ScopedSymbolTable(procName, currentScope->scopeLevel + 1, currentScope);
    currentScope = procedureScope;
            
    for (AST* param : *(procDecNode->params)) {
	Param* paramNode = dynamic_cast<Param*>(param);
	Type* paramType = paramNode->typeNode;
	Symbol* paramTypeSymbol = currentScope->lookup(paramType->value.strVal);
	Var* paramVarNode = paramNode->varNode;
	VarSymbol* varSymbol = new VarSymbol(paramVarNode->value.strVal, paramTypeSymbol);
	currentScope->define(varSymbol);
	procSymbol->params->push_back(varSymbol);
    }
    procDecNode->table = currentScope;
    this->visit(procDecNode->blockNode);
    currentScope = currentScope->enclosingScope;
    if (options::showST) {
	cout << procedureScope->toString() << endl;
	cout << "LEAVE scope: " << procName << endl;
    }
    return nullptr;
}

Symbol* SemanticAnalyzer::visitProcedureCall(AST* node) {
    ProcedureCall* procCallNode = dynamic_cast<ProcedureCall*>(node);
    string procName = procCallNode->procName;

    /*
      Handle built-in functions
    */
    auto itr = builtin::FUNCTIONS.find(procName);
    if (itr != builtin::FUNCTIONS.end()) {
	unsigned int nFormalParams = itr->second.paramTypes.size();
	unsigned int nActualParams = procCallNode->paramVals->size();
	if (nFormalParams != nActualParams) {
	    this->error("expected " + to_string(nFormalParams) + " arguments, got " + to_string(nActualParams) + " in call to built-in " + procName, procCallNode->line); 
	}
	auto fiter = itr->second.paramTypes.begin();
	auto aiter = procCallNode->paramVals->begin();
	for (;fiter != itr->second.paramTypes.end(); fiter++, aiter++) {
	    Symbol* typeSymbol = this->visit(*aiter);
	    string argTypeName = ScopedSymbolTable::builtInsMap[*fiter]->name;
	    if (!options::staticTypeChecking && argTypeName == ttype::any) {
		// If dynamic types are allowed, ignore assignments to "any" type.
		continue;
	    }
	    if (argTypeName != typeSymbol->name) {
		this->error("type mismatch between value of type " + argTypeName + " and " \
			    "value of type " + typeSymbol->name + " in call to built-in " + procName,
			    procCallNode->line);
	    }	    
	}
	return itr->second.returnType;
    }
    
    Symbol* result;
    if (!(result = currentScope->lookup(procName))) {
	this->error("no procedure found with name " + procName, procCallNode->line);
    }
    ProcedureSymbol* procSymbol = dynamic_cast<ProcedureSymbol*>(result);
    map<ProcedureSymbol*, AST*>::iterator iter;
    if ((iter = procedureTable.find(procSymbol)) == procedureTable.end()) {
	this->error("procedure declaration node could not be found in program tree", procCallNode->line);
    }
    ProcedureDecl* procDeclNode = dynamic_cast<ProcedureDecl*>(iter->second);
    // Check for matching number of formal and actual params.
    if (procCallNode->paramVals->size() != procDeclNode->params->size()) {
	this->error("wrong number of parameters in call to " + procDeclNode->procName, procCallNode->line);
    }
    auto fiter = procDeclNode->params->begin(); // Vector of Param*
    auto aiter = procCallNode->paramVals->begin();
    for (;fiter != procDeclNode->params->end(); fiter++, aiter++) {
	Symbol* typeSymbol = this->visit(*aiter);
	if (!options::staticTypeChecking && (*fiter)->typeNode->value.strVal == ttype::any) {
	    // If dynamic types are allowed, ignore assignments to "any" type.
	    continue;
	}	
	if ((*fiter)->typeNode->value.strVal != typeSymbol->name) {
	    this->error("type mismatch between value of type " + (*fiter)->typeNode->value.strVal + " and " \
			"value of type " + typeSymbol->name + " in call to " + procName, procCallNode->line);
	}
	    
    }
    VarSymbol* paramSymbol;	
    for (Symbol* param : *(procSymbol->params)) {
	paramSymbol = dynamic_cast<VarSymbol*>(param);
	cout << paramSymbol->name << endl;
	    
    }
    procCallNode->procDeclNode = iter->second;

    ProcedureDecl* pdNode = dynamic_cast<ProcedureDecl*>(procCallNode->procDeclNode);
    if (!pdNode->returnTypeNode) {
	return nullptr;
    }    
    auto retSymbol = currentScope->lookup(pdNode->returnTypeNode->value.strVal);
    if (!retSymbol) {
	this->error("procedure \"" + procName + "\" does not have a valid return type", pdNode->line);
    }
    return retSymbol;
}

Symbol* SemanticAnalyzer::visitIfStatement(AST* node) {
    IfStatement* ifStatementNode = dynamic_cast<IfStatement*>(node);
    this->visit(ifStatementNode->conditionNode);
    this->visit(ifStatementNode->blockNode);
    if (ifStatementNode->elseBranch) {
	this->visit(ifStatementNode->elseBranch);
    }
    return nullptr;
}

Symbol* SemanticAnalyzer::visitWhileStatement(AST* node) {
    WhileStatement* whileStatementNode = dynamic_cast<WhileStatement*>(node);
    this->visit(whileStatementNode->conditionNode);
    this->visit(whileStatementNode->blockNode);
    return nullptr;
}

Symbol* SemanticAnalyzer::visitReturnStatement(AST* node) {
    ReturnStatement* returnStatementNode = dynamic_cast<ReturnStatement*>(node);
    Symbol* retStatementType = this->visit(returnStatementNode->expr);
    Symbol* procType = currentScope->lookup(returnStatementNode->procDecl->returnTypeNode->value.strVal);
    this->resolveTypes(procType, retStatementType, returnStatementNode->line);
    return nullptr;
}
