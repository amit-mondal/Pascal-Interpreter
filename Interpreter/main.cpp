#include <string>
#include <fstream>
#include <sstream>
#include <unordered_set>
#include "constants.h"
#include "Token.h"
#include "Lexer.h"
#include "Symbol.h"
#include "ASTNodes.h"
#include "DataVal.h"
#include "CallStack.h"
#include "options.h"

using namespace std;

class AST;
class Parser;	   


/****************************************
 Parser
***************************************/

class Parser {
public:
    Parser(Lexer* lexer);
    int line();
    void error(string errmsg);
    void eat(string tokenType);
    AST* program();
    AST* block();
    vector<AST*>* declarations();
    vector<Param*>* formalParameters();
    vector<Param*>* formalParameterList();
    vector<AST*>* variableDeclarations();
    Type* typeSpec();
    AST* compoundStatement();
    vector<AST*>* statementList();
    AST* statement();
    AST* assignmentStatement();
    AST* variable();
    AST* empty();
    AST* expr();
    AST* term();
    AST* factor();
    AST* parse();
    AST* procedureCall();
    vector<AST*>* actualParameters();
    AST* ifStatement(bool isElseIf = false);
    AST* whileStatement();
private:
    unordered_set<string> validTypes = {ttype::integer, ttype::real, ttype::string};
    Lexer* lexer;
    Token* currentToken;
};

Parser::Parser(Lexer* lexer) {
    this->lexer = lexer;
    this->currentToken = this->lexer->getNextToken();
}

int Parser::line() {
    return lexer->line;
}

void Parser::error(string errmsg) {
    utils::fatalError("Parse error on line " + to_string(lexer->line) + ": " + errmsg);
}

void Parser::eat(string tokenType) {
    if (options::printTokens) {
        cout << "Consumed token " << *currentToken << endl;
    }
    if (currentToken->type == tokenType) {
        currentToken = lexer->getNextToken();
    }
    else {
        this->error("Token of type " + tokenType + " expected, " + currentToken->type + " with value \"" + currentToken->value.toString() + "\" found");
    }
}

AST* Parser::program() {
    this->eat(ttype::program);
    Var* varNode = dynamic_cast<Var*>(this->variable());
    string progName = varNode->value.strVal;
    this->eat(ttype::semi);
    int line = this->line();
    Block* blockNode = dynamic_cast<Block*>(this->block());
    Program* programNode = new Program(progName, blockNode);
    this->eat(ttype::dot);
    programNode->line = line;
    return programNode;
}

AST* Parser::block() {
    int line = this->line();
    vector<AST*>* declarationNodes = this->declarations();
    AST* compoundStatementNode = this->compoundStatement();
    Block* block = new Block(*declarationNodes, compoundStatementNode);
    block->line = line;
    return block;
}

vector<AST*>* Parser::declarations() {
    vector<AST*>* declarations = new vector<AST*>();
    while (true) {
        if (currentToken->type == ttype::var) {
            this->eat(ttype::var);
            while (currentToken->type == ttype::id) {
                vector<AST*>* varDecl = this->variableDeclarations();
                utils::combineArrs<AST>(declarations, varDecl);
                this->eat(ttype::semi);
            }
        }
        else if (currentToken->type == ttype::procedure) {
            this->eat(ttype::procedure);
            string procName = currentToken->value.strVal;
            this->eat(ttype::id);
            vector<Param*>* params;
            if (currentToken->type == ttype::lparen) {
                this->eat(ttype::lparen);
                params = this->formalParameterList();
                this->eat(ttype::rparen);
            }
            else {
                params = new vector<Param*>();
            }
            this->eat(ttype::semi);
            AST* blockNode = this->block();
            AST* procDecl = new ProcedureDecl(procName, params, blockNode);
            procDecl->line = this->line();
            declarations->push_back(procDecl);
            this->eat(ttype::semi);
        }
	else if (currentToken->type == ttype::type) {
	    this->eat(ttype::type);
	    string recordName = currentToken->value.strVal;
	    this->eat(ttype::id);
	    this->eat(ttype::equals);
	    this->eat(ttype::record);	    
	    vector<AST*>* varDecls = new vector<AST*>();
	    while (currentToken->type == ttype::id) {
		vector<AST*>* varDecl = this->variableDeclarations();
		utils::combineArrs<AST>(varDecls, varDecl);
		this->eat(ttype::semi);
	    }
	    RecordDecl* recordDecl = new RecordDecl(recordName, varDecls);
	    if (validTypes.find(recordName) != validTypes.end()) {
		utils::fatalError("Illegal redeclaration of record " + recordName + " on line " + to_string(this->line()));
	    }
	    // Register the newly declared type as a valid type.
	    validTypes.insert(recordName);
	    recordDecl->line = this->line();
	    declarations->push_back(recordDecl);
	    this->eat(ttype::end);
	    this->eat(ttype::semi);
	}
        else break;
    }
    return declarations;
}

vector<Param*>* Parser::formalParameters() {
    vector<Param*>* paramNodes = new vector<Param*>();
    vector<Token*> paramTokens = { currentToken };
    this->eat(ttype::id);
    while (currentToken->type == ttype::comma) {
        this->eat(ttype::comma);
        paramTokens.push_back(currentToken);
        this->eat(ttype::id);
    }
    this->eat(ttype::colon);
    Type* typeNode = this->typeSpec();
    for (Token* paramToken : paramTokens) {
        paramNodes->push_back(new Param(new Var(paramToken), typeNode));
    }
    return paramNodes;
}

vector<Param*>* Parser::formalParameterList() {
    if (currentToken->type != ttype::id) {
        return new vector<Param*>();
    }
    vector<Param*>* paramNodes = this->formalParameters();
    vector<Param*>* formalParams;
    while (currentToken->type == ttype::semi) {
        this->eat(ttype::semi);
        formalParams = this->formalParameters();
        utils::combineArrs<Param>(paramNodes, formalParams);
    }
    return paramNodes;
}

vector<AST*>* Parser::variableDeclarations() {
    vector<AST*> varNodes = { new Var(currentToken) };
    this->eat(ttype::id);
    Var* var;
    while (currentToken->type == ttype::comma) {
        this->eat(ttype::comma);
        var = new Var(currentToken);
        var->line = this->line();
        varNodes.push_back(var);
        this->eat(ttype::id);
    }
    this->eat(ttype::colon);
    Type* typeNode = this->typeSpec();
    vector<AST*>* varDeclarations = new vector<AST*>();
    VarDecl* varDecl;
    Var* varNode;
    for (AST* _varNode : varNodes) {
	varNode = dynamic_cast<Var*>(_varNode);
        varDecl = new VarDecl(varNode, typeNode);
        varDecl->line = this->line();
        varDeclarations->push_back(varDecl);
    }
    return varDeclarations;
}

Type* Parser::typeSpec() {
    Token* token = currentToken;
    if (this->validTypes.find(currentToken->value.strVal) == validTypes.end()) {
        this->error(currentToken->value.strVal + " is not a valid type");
    }
    eat(token->type);
    Type* type = new Type(token);
    type->line = this->line();
    return type;
}

AST* Parser::ifStatement(bool isElseIf) {
    int line = this->line();
    this->eat(ttype::tif);
    this->eat(ttype::lparen);
    AST* conditionExpr = expr();
    this->eat(ttype::rparen);
    // There's no then
    if (!isElseIf) {
        this->eat(ttype::then);
    }
    AST* blockNode = block();
    AST* elseNode = nullptr;
    // If there's no semicolon, there's an else branch.
    if (currentToken->type != ttype::semi) {
        this->eat(ttype::telse);
        // This means it's an else-if
        if (currentToken->type == ttype::tif) {
            elseNode = this->ifStatement(true);
        }
        // This means it's not.
        else if (currentToken->type == ttype::begin) {
            elseNode = this->block();
        }
        else error("Semicolon expected after if-statement block on line " + to_string(this->line()));
    }
    IfStatement* ifStatement = new IfStatement(conditionExpr, blockNode, elseNode);
    ifStatement->line = line;
    return ifStatement;
}

AST* Parser::whileStatement() {
    int line = this->line();
    this->eat(ttype::twhile);
    this->eat(ttype::lparen);
    AST* conditionExpr = expr();
    this->eat(ttype::rparen);
    this->eat(ttype::tdo);
    AST* blockNode = block();
    if (currentToken->type != ttype::semi) {
        error("Semicolon expected after while-statement block on line " + to_string(this->line()));
    }
    WhileStatement* whileStatement = new WhileStatement(conditionExpr, blockNode);
    whileStatement->line = line;
    return whileStatement;
}

AST* Parser::compoundStatement() {
    this->eat(ttype::begin);
    vector<AST*>* nodes = this->statementList();
    this->eat(ttype::end);    
    Compound* root = new Compound();
    root->line = this->line();
    for (AST* node : *nodes) {
        root->children.push_back(node);
    }

    return root;
}

vector<AST*>* Parser::statementList() {
    AST* node = this->statement();
    vector<AST*>* results = new vector<AST*>();
    results->push_back(node);
    while (currentToken->type == ttype::semi) {
        this->eat(ttype::semi);
        results->push_back(this->statement());
    }
    return results;
}

AST* Parser::statement() {
    if (currentToken->type == ttype::begin) {
        return this->compoundStatement();
    }
    else if (currentToken->type == ttype::tif) {
        return this->ifStatement();
    }
    else if (currentToken->type == ttype::twhile) {
        return this->whileStatement();
    }
    else if (currentToken->type == ttype::id) {
        // Okay, so this is either an assignment or a function call
        if (lexer->currentChar == '(') {
            return this->procedureCall();
        }
        else {
            return this->assignmentStatement();
        }
    }
    return this->empty();
}

AST* Parser::procedureCall() {
    int line = this->line();
    string procName = currentToken->value.strVal;
    vector<AST*>* actualParams = new vector<AST*>();
    eat(ttype::id);
    eat(ttype::lparen);
    while (currentToken->type != ttype::rparen){
        actualParams->push_back(this->expr());
        if (currentToken->type == ttype::comma) {
            eat(ttype::comma);
            continue;
        }
    }
    eat(ttype::rparen);
    ProcedureCall* procedureCall = new ProcedureCall(procName, actualParams);
    procedureCall->line = line;
    return procedureCall;
}

AST* Parser::assignmentStatement() {
    AST* left = this->variable();
    Token* token = currentToken;
    this->eat(ttype::assign);
    AST* right = this->expr();
    return new Assign(left, token, right);
}

AST* Parser::variable() {
    AST* node = new Var(currentToken);
    node->line = this->line();
    this->eat(ttype::id);
    return node;
}

AST* Parser::empty() {
    return new NoOp();
}

AST* Parser::expr() {
    AST* node = this->term();
    unordered_set<string> types = {ttype::plus, ttype::minus};
    while (types.find(currentToken->type) != types.end()) {
        Token* token = currentToken;
        if (token->type == ttype::plus) {
            this->eat(ttype::plus);
        }
        else if (token->type == ttype::minus) {
            this->eat(ttype::minus);
        }
        node = new BinOp(node, token, this->term());
        node->line = this->line();
    }
    return node;
}

AST* Parser::term() {
    AST* node = this->factor();
    unordered_set<string> types = {ttype::mul, ttype::int_div, ttype::float_div, ttype::equals, ttype::not_equals};
    while (types.find(currentToken->type) != types.end()) {
        Token* token = currentToken;
        if (token->type == ttype::mul) {
            this->eat(ttype::mul);
        }
        else if (token->type == ttype::int_div) {
            this->eat(ttype::int_div);
        }
        else if (token->type == ttype::float_div) {
            this->eat(ttype::float_div);
        }
        else if (token->type == ttype::equals) {
            this->eat(ttype::equals);
        }
        else if (token->type == ttype::not_equals) {
            this->eat(ttype::not_equals);
        }
        else if (token->type == ttype::greater_than) {
            this->eat(ttype::greater_than);
        }
        node = new BinOp(node, token, this->factor());
        node->line = this->line();
    }
    return node;
}

AST* Parser::factor() {
    Token* token = currentToken;
    if (token->type == ttype::plus) {
        this->eat(ttype::plus);
        return new UnaryOp(token, this->factor());
    }
    else if (token->type == ttype::minus) {
        this->eat(ttype::minus);
        return new UnaryOp(token, this->factor());
    }
    else if (token->type == ttype::int_const) {
        this->eat(ttype::int_const);
        return new Num(token);
    }
    else if (token->type == ttype::real_const) {
        this->eat(ttype::real_const);
        return new Num(token);
    }
    else if (token->type == ttype::string_literal) {
	this->eat(ttype::string_literal);
	return new StringLiteral(token);
    }
    else if (token->type == ttype::lparen) {
        AST* node;
        this->eat(ttype::lparen);
        node = this->expr();
        this->eat(ttype::rparen);
        return node;
    }
    else {
        return this->variable();
    }
}

AST* Parser::parse() {
    AST* node = this->program();
    if (currentToken->type != ttype::eof) {
        this->error("parsing terminated before end of file");
    }
    return node;
}

class SemanticAnalyzer {
public:
    SemanticAnalyzer();
    enum AssignmentState {
			  NONE,
			  LHS,
			  RHS
    };
    void visit(AST* node, AssignmentState aState = NONE);    
private:
    ScopedSymbolTable* currentScope;
    map<ProcedureSymbol*, AST*> procedureTable;
    Symbol* lhsType, *rhsType;
};

SemanticAnalyzer::SemanticAnalyzer() : currentScope(nullptr) {
}

void SemanticAnalyzer::visit(AST* node, AssignmentState aState) {
    if (node == nullptr) utils::fatalError(string("Parse tree is null"));
    switch(node->type()) {
    case NodeType::block: {
	Block* blockNode = dynamic_cast<Block*>(node);
	for (AST* declaration : blockNode->declarations) {
	    this->visit(declaration);
	}
	this->visit(blockNode->compoundStatement);
	break;
    }
    case NodeType::program: {
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
	break;
    }
    case NodeType::compound: {
	Compound* compNode = dynamic_cast<Compound*>(node);
	for (AST* child : compNode->children) {
	    this->visit(child);
	}
	break;
    }
    case NodeType::none: {
	break;
    }
    case NodeType::binOp: {
	BinOp* binOpNode = dynamic_cast<BinOp*>(node);
	this->visit(binOpNode->left);
	this->visit(binOpNode->right);
	break;
    }
    case NodeType::varDecl: {
	VarDecl* varDeclNode = dynamic_cast<VarDecl*>(node);
	Var* varNode = varDeclNode->varNode;
	Type* typeNode = varDeclNode->typeNode;
	string typeName = typeNode->value.strVal;
	Symbol* typeSymbol;
	if (!(typeSymbol = currentScope->lookup(typeName))) {
	    utils::fatalError("Semantic error: no type symbol found for type name " + typeName + " on line " + to_string(varNode->token->line));
	}
	string varName = varNode->value.strVal;
	Symbol* varSymbol;
	if ((varSymbol = currentScope->lookup(varName)) && varSymbol->type != nullptr) {
	    utils::fatalError("Semantic error: duplicate identifier " + varName + " found on line " + to_string(varNode->token->line));
	}
	currentScope->define(new VarSymbol(varName, typeSymbol));
	break;
    }
    case NodeType::recordDecl: {
	RecordDecl* recordDeclNode = dynamic_cast<RecordDecl*>(node);
	currentScope->define(new UserDefinedTypeSymbol(recordDeclNode->recordName));
	break;
    }
    case NodeType::assign: {
	Assign* assignNode = dynamic_cast<Assign*>(node);
	// Let recursive calls know to collect types for type-checking.	
	this->visit(assignNode->left, LHS);
	this->visit(assignNode->right, RHS);
	break;
    }
    case NodeType::var: {
	Var* varNode = dynamic_cast<Var*>(node);
	if (!currentScope->lookup(varNode->value.strVal)) {
	    utils::fatalError("Semantic error: symbol not found for variable " + varNode->value.strVal + " on line " + to_string(varNode->token->line));
	}
	break;
    }
    case NodeType::procedureDecl: {
	ProcedureDecl* procDecNode = dynamic_cast<ProcedureDecl*>(node);
	string procName = procDecNode->procName;
	ProcedureSymbol* procSymbol = new ProcedureSymbol(procName);
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
	break;
    }
    case NodeType::procedureCall: {
	ProcedureCall* procCallNode = dynamic_cast<ProcedureCall*>(node);
	string procName = procCallNode->procName;
	Symbol* result;
	if (!(result = currentScope->lookup(procName))) {
	    utils::fatalError("Semantic error: no procedure found with name " + procName + " on line " + to_string(procCallNode->line));
	}
	ProcedureSymbol* procSymbol = dynamic_cast<ProcedureSymbol*>(result);
	map<ProcedureSymbol*, AST*>::iterator iter;
	if ((iter = procedureTable.find(procSymbol)) == procedureTable.end()) {
	    utils::fatalError("Semantic error: procedure declaration node could not be found in program tree");
	}
	ProcedureDecl* procDeclNode = dynamic_cast<ProcedureDecl*>(iter->second);
	// Check for matching number of formal and actual params.
	if (procCallNode->paramVals->size() != procDeclNode->params->size()) {
	    utils::fatalError("Semantic error: wrong number of parameters in call to " + procDeclNode->procName + " on line " + to_string(procCallNode->line));
	}
	/*
	auto fiter = procDeclNode->params->begin();
	auto aiter = procCallNode->paramVals->begin();
	for (;fiter != procDeclNode->params->end(); fiter++, aiter++) {
	    
	}
	*/
	VarSymbol* paramSymbol;	
	for (Symbol* param : *(procSymbol->params)) {
	    paramSymbol = dynamic_cast<VarSymbol*>(param);
	    
	}
	procCallNode->procDeclNode = iter->second;
	break;
    }
    case NodeType::ifStatement: {
	IfStatement* ifStatementNode = dynamic_cast<IfStatement*>(node);
	this->visit(ifStatementNode->conditionNode);
	this->visit(ifStatementNode->blockNode);
	if (ifStatementNode->elseBranch) {
	    this->visit(ifStatementNode->elseBranch);
	}
	break;
    }
    case NodeType::whileStatement: {
	WhileStatement* whileStatementNode = dynamic_cast<WhileStatement*>(node);
	this->visit(whileStatementNode->conditionNode);
	this->visit(whileStatementNode->blockNode);
	break;
    }
    default:
	break;
    }
}


/****************************************
 Interpreter
***************************************/

/*
 * Interpreter class definition
 */

class Interpreter {
public:
    Parser* parser;
    Interpreter(Parser* parser);
    DataVal visit(AST* node);
    DataVal interpret();
private:
    CallStack stack;
};

Interpreter::Interpreter(Parser* parser) {
    this->parser = parser;
}

DataVal Interpreter::visit(AST* node) {
    if (node == nullptr) utils::fatalError(string("Parse tree is null"));
    switch(node->type()) {
    case NodeType::binOp: {
	BinOp binNode = dynamic_cast<BinOp&>(*node);
	string opType = binNode.op->type;
	/*
	if (opType == ttype::plus) {
	    return visit(binNode.left) + visit(binNode.right);
	}
	else if (opType == ttype::minus) {
	    return visit(binNode.left) - visit(binNode.right);
	}
	else if (opType == ttype::mul) {
	    return visit(binNode.left) * visit(binNode.right);
	}
	else if (opType == ttype::int_div) {
	    return (int(visit(binNode.left)) / int(visit(binNode.right)));
	}
	else if (opType == ttype::float_div) {
	    return visit(binNode.left) / visit(binNode.right);
	}
	*/
	if (opType == ttype::equals) {
	    DataVal left = visit(binNode.left);
	    DataVal right = visit(binNode.right);
	    if (options::showConditions) {
		cout << "left: " << left.toString() << " right: " << right.toString() << endl;
	    }
	    return DataVal(left == right);
	}
	else if (opType == ttype::not_equals) {
	    DataVal left = visit(binNode.left);
	    DataVal right = visit(binNode.right);
	    if (options::showConditions) {
		cout << "left: " << left.toString() << " right: " << right.toString() << endl;
	    }
	    return DataVal(left != right);
	}
	else {
	    utils::fatalError(opType + " on line " + to_string(binNode.line) + " is not a known binary operation");
	}
	break;
    }
    case NodeType::num: {
	Num num = dynamic_cast<Num&>(*node);
	return num.value;
	break;
    }
    case NodeType::stringLiteral: {
	StringLiteral str = dynamic_cast<StringLiteral&>(*node);
	return str.value;
	break;
    }
    case NodeType::unaryOp: {
	UnaryOp unaryNode = dynamic_cast<UnaryOp&>(*node);
	string opType = unaryNode.op->type;
	DataVal exprResult = visit(unaryNode.expr);
	if (exprResult.isNumeric()) {
	    if (opType == ttype::minus) {
		switch(exprResult.type) {
		case DataVal::D_INT: {
		    return DataVal(-1 * DATAVAL_GET_VAL(int, exprResult.data));
		}
		case DataVal::D_REAL:
		    return DataVal(-1 * DATAVAL_GET_VAL(double, exprResult.data));
		}		    
	    }
	}
	else {
	    utils::fatalError(opType + " on line " + to_string(unaryNode.line) + " is not a known unary operation for value of type " + to_string(exprResult.type));
	}
	break;
    }
    case NodeType::compound: {
	Compound compoundNode = dynamic_cast<Compound&>(*node);
	for (AST* child : compoundNode.children) {
	    visit(child);
	}
	break;
    }
    case NodeType::none: {
	break;
    }
    case NodeType::assign: {
	Assign* assignNode = dynamic_cast<Assign*>(node);
	Var* varNode = dynamic_cast<Var*>(assignNode->left);
	string varName = varNode->value.strVal;
	DataVal rvalue = visit(assignNode->right);
	stack.assign(varName, rvalue, assignNode->line);
	break;
    }
    case NodeType::var: {
	Var varNode = dynamic_cast<Var&>(*node);
	return stack.lookup(varNode.value.strVal, varNode.line);
	break;
    }
    case NodeType::program: {
	Program progNode = dynamic_cast<Program&>(*node);
	stack.pushFrame(progNode.table);
	visit(progNode.block);
	stack.popFrame();
	break;
    }
    case NodeType::block: {
	Block block = dynamic_cast<Block&>(*node);
	for (AST* decl : block.declarations) {
	    visit(decl);
	}
	visit(block.compoundStatement);
	break;
    }
    case NodeType::varDecl: {
	break;
    }
    case NodeType::recordDecl: {
	break;
    }
    case NodeType::varType: {
	break;
    }
    case NodeType::procedureDecl: {
	break;
    }
    case NodeType::procedureCall: {
	ProcedureCall* procCallNode = dynamic_cast<ProcedureCall*>(node);
	ProcedureDecl* procDeclNode = dynamic_cast<ProcedureDecl*>(procCallNode->procDeclNode);
	ssize_t numParams = procCallNode->paramVals->size();
	Param* paramNode;
	Var* varNode;
	// Make an array for each of the formal and actual params.
	DataVal finalParamVals[numParams];
	string paramNames[numParams];
	for (unsigned int i = 0;i<numParams;i++) {
	    paramNode = dynamic_cast<Param*>(procDeclNode->params->at(i));
	    varNode = paramNode->varNode;
	    finalParamVals[i] = visit(procCallNode->paramVals->at(i));
	    paramNames[i] = varNode->value.strVal;
	}
	// Push a new stack frame and assign params.
	stack.pushFrame(procDeclNode->table, paramNames, finalParamVals, numParams);
	// Run procedure body.
	visit(procDeclNode->blockNode);
	// Pop stack frame.
	stack.popFrame();
	break;
    }
    case NodeType::ifStatement: {
	IfStatement* ifStatementNode = dynamic_cast<IfStatement*>(node);
	// Well isn't this code convenient...
	DataVal condition = visit(ifStatementNode->conditionNode);
	if (options::showConditions) {
	    cout << "If Condition result: " << condition.toString() << endl;
	}
	if (condition.toBool()) {
	    visit(ifStatementNode->blockNode);
	}
	else if (ifStatementNode->elseBranch) {
	    visit(ifStatementNode->elseBranch);
	}
	break;
    }
    case NodeType::whileStatement: {
	WhileStatement* whileStatementNode = dynamic_cast<WhileStatement*>(node);
	while (visit(whileStatementNode->conditionNode).toBool()) {
	    visit(whileStatementNode->blockNode);
	}
	break;
    }
    default:
	utils::fatalError(string("Syntax tree node of type-index ") + to_string(node->type()) + string(" cannot be visited"));
    }
    return DataVal();
}

DataVal Interpreter::interpret() {
    AST* tree = parser->parse();
    SemanticAnalyzer analyzer;
    analyzer.visit(tree);
    return visit(tree);
}

/*********************************
 Command Line Args Parser
********************************/

class InputParser{
public:
    InputParser (int &argc, char **argv){
        for (int i=1; i < argc; ++i)
            this->tokens.push_back(std::string(argv[i]));
    }
    const std::string& getCmdOption(const std::string &option) const{
        auto itr =  std::find(this->tokens.begin(), this->tokens.end(), option);
        if (itr != this->tokens.end() && ++itr != this->tokens.end()){
            return *itr;
        }
        return empty;
    }
    bool cmdOptionExists(const std::string &option) const{
        return std::find(this->tokens.begin(), this->tokens.end(), option)
	    != this->tokens.end();
    }
private:
    const std::string empty;
    std::vector <std::string> tokens;
};

int main(int argc, char *argv[]) {
    InputParser input(argc, argv);
    const string fileName = input.getCmdOption("-f");
    options::printTokens = input.cmdOptionExists("-pt") || input.cmdOptionExists("--print-tokens");
    options::dumpVars = input.cmdOptionExists("-dv") || input.cmdOptionExists("--dump-vars");
    options::showST = input.cmdOptionExists("-sst") || input.cmdOptionExists("--show-symbol-table");
    options::showConditions = input.cmdOptionExists("-sc") || input.cmdOptionExists("--show-conditions");
    
    if (!fileName.empty()) {
        cout << "File: " << fileName << endl;
        ifstream file(fileName);
        stringstream buffer;
        buffer << file.rdbuf();
        string str = buffer.str();
        Lexer lexer = Lexer(str);
        Parser parser = Parser(&lexer);
        Interpreter interpreter = Interpreter(&parser);
	interpreter.interpret();
    }
    return 0;
}
