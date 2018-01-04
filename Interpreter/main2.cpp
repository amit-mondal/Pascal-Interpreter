#include <string>
#include <fstream>
#include <sstream>
#include <unordered_set>
#include "utils.h"
#include "constants.h"
#include "Token.h"
#include "Symbol.h"
#include "ASTNodes.h"

using namespace std;

/*
 * Runtime options
 */

namespace options {
    bool printTokens = false;
    bool dumpVars = false;
    bool showST = false;
}

class AST;
class Parser;


/*
 * Symbol table class definition
 */

class ScopedSymbolTable {
public:
    utils::OrderedMap<string, Symbol> symbols;
    ScopedSymbolTable(string scopeName, int scopeLevel, ScopedSymbolTable* enclosingScope);
    void define(Symbol* symbol);
    Symbol* lookup(string name, bool currScope);
    void initBuiltIns();
    string toString() const;
    ScopedSymbolTable* enclosingScope;
    int scopeLevel;
    string name();
private:
    string scopeName;
};

string ScopedSymbolTable::name() {
    return scopeName;
}

ScopedSymbolTable::ScopedSymbolTable(string scopeName, int scopeLevel, ScopedSymbolTable* enclosingScope = nullptr) : scopeLevel(scopeLevel), scopeName(scopeName) {
    this->enclosingScope = enclosingScope;
    this->initBuiltIns();
}

void ScopedSymbolTable::define(Symbol* symbol) {
    if (options::showST) cout << "Define: " + symbol->toString() << endl;
    symbols.add(symbol->name, symbol);
}

Symbol* ScopedSymbolTable::lookup(string name, bool currScope = false) {
    if (options::showST) cout << "Lookup: " + name << endl;
    Symbol* res = symbols.get(name);
    if (res) return res;
    if (currScope) return nullptr;
    else if (enclosingScope) return enclosingScope->lookup(name);
    return nullptr;
}

void ScopedSymbolTable::initBuiltIns() {
    define(new BuiltInTypeSymbol("INTEGER"));
    define(new BuiltInTypeSymbol("REAL"));
}

string ScopedSymbolTable::toString() const {
    string result = "Symbols in " + scopeName + " scope, level " + to_string(scopeLevel) + '\n';
    result += "Enclosing scope: ";
    result += !enclosingScope ? "none" : enclosingScope->scopeName;
    result += "\n";
    for (string key : symbols.order) {
        result += key + " : " + this->symbols.get(key)->toString() + ", \n";
    }
    return result;
}

	    
/*
 * Lexer class definition
 */

class Lexer {
public:
    string input;
    unsigned int pos;
    int line;
    char currentChar;
    Lexer(string input);
    void error(char ch);
    void advance();
    void skipWhiteSpace();
    void skipComment();
    Token* number();
    Token* getNextToken();
    char peek();
    Token* id();
    
private:
    const map<string, Token*> RESERVED_KEYWORDS = {
        { "BEGIN" , new Token(ttype::begin, "BEGIN", -1) },
        { "END", new Token(ttype::end, "END", -1) },
        { "PROGRAM", new Token(ttype::program, "PROGRAM", -1) },
        { "VAR", new Token(ttype::var, "VAR", -1) },
        { "DIV", new Token(ttype::int_div, "DIV", -1) },
        { "INTEGER", new Token(ttype::integer, "INTEGER", -1) },
        { "REAL", new Token(ttype::real, "REAL", -1) },
        { "PROCEDURE", new Token(ttype::procedure, "PROCEDURE", -1) },
        { "WHILE", new Token(ttype::twhile, "WHILE", -1)},
        { "IF", new Token(ttype::tif, "IF", -1)},
        { "THEN", new Token(ttype::then, "THEN", -1)},
        { "ELSE", new Token(ttype::telse, "ELSE", -1)},
        { "DO", new Token(ttype::tdo, "DO", -1)}
    };
};

Lexer::Lexer(string input) {
    this->input = input;
    pos = 0;
    line = 1;
    currentChar = input[pos];
}

void Lexer::error(char ch) {
    utils::fatalError(string("Invalid character at line " + to_string(line) + ": ") + "\'" + ch + "\'");
}

void Lexer::advance() {
    pos++;
    if (pos > input.length() - 1) currentChar = 0;
    else currentChar = input[pos];
    if (currentChar == '\n') line++;
}

void Lexer::skipWhiteSpace() {
    while (currentChar != 0 && (currentChar == ' ' || currentChar == '\n' || currentChar == '\t')) {
        advance();
    }
}

void Lexer::skipComment() {
    while (currentChar != '}') {
        advance();
    }
    advance();
}

Token* Lexer::number() {
    string res = "";
    while (currentChar != 0 && isdigit(currentChar)) {
        res += currentChar;
        advance();
    }
    if (currentChar == '.') {
        res += currentChar;
        advance();
        while (currentChar != 0 && isdigit(currentChar)) {
            res += currentChar;
            advance();
        }
        return new Token(ttype::real_const, utils::toDouble(res), line);
    }
    return new Token(ttype::int_const, utils::toDouble(res), line);
}

Token* Lexer::getNextToken() {
    while (currentChar != 0) {
        if (currentChar == ' ' || currentChar == '\n') {
            skipWhiteSpace();
            continue;
        }
        if (currentChar == '{') {
            advance();
            skipComment();
            continue;
        }
        if (isdigit(currentChar)) {
            return number();
        }
        if (isalpha(currentChar)) {
            return id();
        }
        if (currentChar == ':' && peek() == '=') {
            advance();
            advance();
            return new Token(ttype::assign, ":=", line);
        }
        if (currentChar == ':') {
            advance();
            return new Token(ttype::colon, ':', line);
        }
        if (currentChar == ';') {
            advance();
            return new Token(ttype::semi, ';', line);
        }
        if (currentChar == '.') {
            advance();
            return new Token(ttype::dot, '.', line);
        }
        if (currentChar == ',') {
            advance();
            return new Token(ttype::comma, ',', line);
        }
        if (currentChar == '+') {
            advance();
            return new Token(ttype::plus, '+', line);
        }
        if (currentChar == '-') {
            advance();
            return new Token(ttype::minus, '-', line);
        }
        if (currentChar == '*') {
            advance();
            return new Token(ttype::mul, '*', line);
        }
        if (currentChar == '/') {
            advance();
            return new Token(ttype::float_div, '/', line);
        }
        if (currentChar == '(') {
            advance();
            return new Token(ttype::lparen, '(', line);
        }
        if (currentChar == ')') {
            advance();
            return new Token(ttype::rparen, ')', line);
        }
        if (currentChar == '=') {
            advance();
            return new Token(ttype::equals, '=', line);
        }
        if (currentChar == '!') {
            if (peek() == '=') {
                advance();
                advance();
                return new Token(ttype::not_equals, "!=", line);
            }
            else {
                advance();
                return new Token(ttype::bang, '!', line);
            }
        }
        if (currentChar == '<') {
            if (peek() == '=') {
                advance();
                advance();
                return new Token(ttype::lt_or_equals, "<=", line);
            }
            else {
                advance();
                return new Token(ttype::less_than, '<', line);
            }
        }
        if (currentChar == '>') {
            if (peek() == '=') {
                advance();
                advance();
                return new Token(ttype::gt_or_equals, ">=", line);
            }
            else {
                advance();
                return new Token(ttype::greater_than, '>', line);
            }
        }
        error(currentChar);
    }
    return new Token(ttype::eof, '\0', line);
}

char Lexer::peek() {
    return (pos + 1) > input.length() - 1 ? '\0' : input[pos + 1];
}

Token* Lexer::id() {
    //handles identifiers and reserved keywords
    string result = "";
    while(currentChar != '\0' && isalnum(currentChar)) {
        result += toupper(currentChar);
        advance();
    }
    map<string, Token*>::const_iterator iter = RESERVED_KEYWORDS.find(result);
    if (iter != RESERVED_KEYWORDS.end()) {
        iter->second->line = line;
        return iter->second;
    }
    else {
        return new Token(ttype::id, result, line);
    }
}

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
    vector<AST*>* formalParameters();
    vector<AST*>* formalParameterList();
    vector<AST*>* variableDeclarations();
    AST* typeSpec();
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
    AST* ifStatement(bool isElseIf);
    AST* whileStatement();
private:
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
    Var* varNode = reinterpret_cast<Var*>(this->variable());
    string progName = varNode->value.strVal;
    this->eat(ttype::semi);
    int line = this->line();
    Block* blockNode = reinterpret_cast<Block*>(this->block());
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
                utils::combineArrs(*declarations, *varDecl);
                this->eat(ttype::semi);
            }
        }
        else if (currentToken->type == ttype::procedure) {
            this->eat(ttype::procedure);
            string procName = currentToken->value.strVal;
            this->eat(ttype::id);
            vector<AST*>* params;
            if (currentToken->type == ttype::lparen) {
                this->eat(ttype::lparen);
                params = this->formalParameterList();
                this->eat(ttype::rparen);
            }
            else {
                params = new vector<AST*>();
            }
            this->eat(ttype::semi);
            AST* blockNode = this->block();
            AST* procDecl = new ProcedureDecl(procName, params, blockNode);
            procDecl->line = this->line();
            declarations->push_back(procDecl);
            this->eat(ttype::semi);
        }
        else break;
    }
    return declarations;
}

vector<AST*>* Parser::formalParameters() {
    vector<AST*>* paramNodes = new vector<AST*>();
    vector<Token*> paramTokens = { currentToken };
    this->eat(ttype::id);
    while (currentToken->type == ttype::comma) {
        this->eat(ttype::comma);
        paramTokens.push_back(currentToken);
        this->eat(ttype::id);
    }
    this->eat(ttype::colon);
    AST* typeNode = this->typeSpec();
    for (Token* paramToken : paramTokens) {
        paramNodes->push_back(new Param(new Var(paramToken), typeNode));
    }
    return paramNodes;
}

vector<AST*>* Parser::formalParameterList() {
    if (currentToken->type != ttype::id) {
        return new vector<AST*>();
    }
    vector<AST*>* paramNodes = this->formalParameters();
    vector<AST*>* formalParams;
    while (currentToken->type == ttype::semi) {
        this->eat(ttype::semi);
        formalParams = this->formalParameters();
        utils::combineArrs(*paramNodes, *formalParams);
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
    AST* typeNode = this->typeSpec();
    vector<AST*>* varDeclarations = new vector<AST*>();
    VarDecl* varDecl;
    for (AST* varNode : varNodes) {
        varDecl = new VarDecl(varNode, typeNode);
        varDecl->line = this->line();
        varDeclarations->push_back(varDecl);
    }
    return varDeclarations;
}

AST* Parser::typeSpec() {
    Token* token = currentToken;
    if (currentToken->type == ttype::integer) {
        this->eat(ttype::integer);
    }
    else {
        this->eat(ttype::real);
    }
    Type* type = new Type(token);
    type->line = this->line();
    return type;
}

AST* Parser::ifStatement(bool isElseIf = false) {
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
    void visit(AST* node);
    SemanticAnalyzer();
private:
    ScopedSymbolTable* currentScope;
    map<ProcedureSymbol*, AST*> procedureTable;
};

SemanticAnalyzer::SemanticAnalyzer() : currentScope(nullptr) {
}

void SemanticAnalyzer::visit(AST* node) {
    if (node == nullptr) utils::fatalError(string("Parse tree is null"));
    switch(node->type()) {
        case NodeType::block: {
            Block* blockNode = reinterpret_cast<Block*>(node);
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
            Program* progNode = reinterpret_cast<Program*>(node);
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
            Compound* compNode = reinterpret_cast<Compound*>(node);
            for (AST* child : compNode->children) {
                this->visit(child);
            }
            break;
        }
        case NodeType::none: {
            break;
        }
        case NodeType::binOp: {
            BinOp* binOpNode = reinterpret_cast<BinOp*>(node);
            this->visit(binOpNode->left);
            this->visit(binOpNode->right);
            break;
        }
        case NodeType::varDecl: {
            VarDecl* varDeclNode = reinterpret_cast<VarDecl*>(node);
            Var* varNode = reinterpret_cast<Var*>(varDeclNode->varNode);
            Type* typeNode = reinterpret_cast<Type*>(varDeclNode->typeNode);
            string typeName = typeNode->value.strVal;
            Symbol* typeSymbol;
            if (!(typeSymbol = currentScope->lookup(typeName))) {
                utils::fatalError("Semantic error: no type symbol found for type name " + typeName + " on line " + to_string(varNode->token->line));
            }
            string varName = varNode->value.strVal;
            if (currentScope->lookup(varName)) {
                utils::fatalError("Semantic error: duplicate identifier " + varName + " found on line " + to_string(varNode->token->line));
            }
            currentScope->define(new VarSymbol(varName, typeSymbol));
            break;
        }
        case NodeType::assign: {
            Assign* assignNode = reinterpret_cast<Assign*>(node);
            this->visit(assignNode->left);
            this->visit(assignNode->right);
            break;
        }
        case NodeType::var: {
            Var* varNode = reinterpret_cast<Var*>(node);
            if (!currentScope->lookup(varNode->value.strVal)) {
                utils::fatalError("Semantic error: symbol not found for variable " + varNode->value.strVal + " on line " + to_string(varNode->token->line));
            }
            break;
        }
        case NodeType::procedureDecl: {
            ProcedureDecl* procDecNode = reinterpret_cast<ProcedureDecl*>(node);
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
                Param* paramNode = reinterpret_cast<Param*>(param);
                Type* paramType = reinterpret_cast<Type*>(paramNode->typeNode);
                Symbol* paramTypeSymbol = currentScope->lookup(paramType->value.strVal);
                Var* paramVarNode = reinterpret_cast<Var*>(paramNode->varNode);
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
            ProcedureCall* procCallNode = reinterpret_cast<ProcedureCall*>(node);
            string procName = procCallNode->procName;
            Symbol* result;
            if (!(result = currentScope->lookup(procName))) {
                utils::fatalError("Semantic error: no procedure found with name " + procName);
            }
            ProcedureSymbol* procSymbol = reinterpret_cast<ProcedureSymbol*>(result);
            map<ProcedureSymbol*, AST*>::iterator iter;
            if ((iter = procedureTable.find(procSymbol)) == procedureTable.end()) {
                utils::fatalError("Semantic error: procedure declaration node could not be found in program tree");
            }
            procCallNode->procDeclNode = iter->second;
            break;
        }
        case NodeType::ifStatement: {
            IfStatement* ifStatementNode = reinterpret_cast<IfStatement*>(node);
            this->visit(ifStatementNode->conditionNode);
            this->visit(ifStatementNode->blockNode);
            if (ifStatementNode->elseBranch) {
                this->visit(ifStatementNode->elseBranch);
            }
            break;
        }
        case NodeType::whileStatement: {
            WhileStatement* whileStatementNode = reinterpret_cast<WhileStatement*>(node);
            this->visit(whileStatementNode->conditionNode);
            this->visit(whileStatementNode->blockNode);
            break;
        }
        default:
            break;
    }
}

/****************************************
 Call Stack
 ***************************************/

class CallStack {
public:
    CallStack();
    void pushFrame(ScopedSymbolTable* symbolTable);
    void pushFrame(ScopedSymbolTable* symbolTable, string* formalParams, string* actualParams, ssize_t numParams);
    void popFrame();
    void assign(string key, string value, int line);
    string lookup(string key, int line);
    void printCurrentFrame();
private:
    struct StackFrame {
        unordered_map<string, string> valTable;
        StackFrame* parent;
        ScopedSymbolTable* symbolTable;
        StackFrame(ScopedSymbolTable* symbolTable, StackFrame* parent = nullptr) : parent(parent), symbolTable(symbolTable) {}
        void dump() {
            cout << "______________________________" << endl;
            cout << "Frame: " << symbolTable->name() << endl;
            utils::dumpMap(valTable);
            cout << "______________________________" << endl;
        }
    };
    string foundVal;
    StackFrame* findFrame(string key);
    StackFrame* currentFrame;
};

void CallStack::printCurrentFrame() {
    currentFrame->dump();
}

CallStack::CallStack() {
    currentFrame = nullptr;
}

CallStack::StackFrame* CallStack::findFrame(string key) {
    utils::toUpper(key);
    StackFrame* frame = currentFrame;
    unordered_map<string, string>::iterator it;
    if (options::dumpVars) {
        cout << "******************************" << endl;
    }
    while(frame) {
        if (options::dumpVars) {
            frame->dump();
        }
        it = frame->valTable.find(key);
        if (it == frame->valTable.end()) {
            frame = frame->parent;
        }
        else {
            if (options::dumpVars) {
                cout << "******************************" << endl;
            }
            foundVal = it->second;
            return frame;
        }
    }
    return nullptr;
}

string CallStack::lookup(string key, int line = -1) {
    if (findFrame(key)) {
        return foundVal;
    }
    utils::fatalError("Could not find value for variable reference '" + key + "' on line " + to_string(line));
    return "";
}

void CallStack::assign(string key, string value, int line = -1) {
    utils::toUpper(key);
    StackFrame* frame;
    if (!currentFrame) {
        utils::fatalError("Stack error: no initial frame pushed to call stack");
    }
    if (!currentFrame->symbolTable->lookup(key)) {
        utils::fatalError("Failed assignment to undeclared variable " + value + " on line " + to_string(line));
    }
    if (!(frame = this->findFrame(key))) {
        frame = currentFrame;
    }
    frame->valTable.insert(make_pair(key, value));
}

void CallStack::pushFrame(ScopedSymbolTable* symbolTable) {
    StackFrame* newFrame = new StackFrame(symbolTable, currentFrame);
    currentFrame = newFrame;
}

void CallStack::pushFrame(ScopedSymbolTable *symbolTable, string *formalParams, string *actualParams, ssize_t numParams) {
    // Since this is for procedure calls, we can always assign to the new frame.
    this->pushFrame(symbolTable);
    // Populate stack value table.
    for (unsigned int i = 0;i<numParams;i++) {
        currentFrame->valTable.insert(make_pair(formalParams[i], actualParams[i]));
    }
    currentFrame->dump();
}

void CallStack::popFrame() {
    cout << "popping frame" << endl;
    StackFrame* oldFrame = currentFrame;
    currentFrame = currentFrame->parent;
    // Can't delete the symbol table, because we might need it for other
    // frames.
    delete oldFrame;
    if (!currentFrame && options::dumpVars) {
        cout << "Stack base frame popped" << endl;
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
    double visit(AST* node);
    double interpret();
private:
    CallStack stack;
};

Interpreter::Interpreter(Parser* parser) {
    this->parser = parser;
}

double Interpreter::visit(AST* node) {
    if (node == nullptr) utils::fatalError(string("Parse tree is null"));
    switch(node->type()) {
        case NodeType::binOp: {
            BinOp binNode = reinterpret_cast<BinOp&>(*node);
            string opType = binNode.op->type;
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
            else if (opType == ttype::equals) {
                double left = visit(binNode.left);
                double right = visit(binNode.right);
                cout << "left: " << left << " right: " << right << endl;
                return left == right;
            }
            else if (opType == ttype::not_equals) {
                double left = visit(binNode.left);
                double right = visit(binNode.right);
                cout << "left: " << left << " right: " << right << endl;
                return left != right;
            }
            else {
                utils::fatalError(opType + " on line " + to_string(binNode.line) + " is not a known binary operation");
            }
            break;
        }
        case NodeType::num: {
            Num num = reinterpret_cast<Num&>(*node);
            return num.value;
            break;
        }
        case NodeType::unaryOp: {
            UnaryOp unaryNode = reinterpret_cast<UnaryOp&>(*node);
            string opType = unaryNode.op->type;
            if (opType == ttype::plus) {
                return visit(unaryNode.expr);
            }
            else if (opType == ttype::minus) {
                return -1 * visit(unaryNode.expr);
            }
            else {
                utils::fatalError(opType + " on line " + to_string(unaryNode.line) + " is not a known unary operation");
            }
            break;
        }
        case NodeType::compound: {
            Compound compoundNode = reinterpret_cast<Compound&>(*node);
            for (AST* child : compoundNode.children) {
                visit(child);
            }
            break;
        }
        case NodeType::none: {
            break;
        }
        case NodeType::assign: {
            Assign assignNode = reinterpret_cast<Assign&>(*node);
            Var varNode = reinterpret_cast<Var&>(*assignNode.left);
            string varName = varNode.value.strVal;
            stack.assign(varName, to_string(visit(assignNode.right)), assignNode.line);
            break;
        }
        case NodeType::var: {
            Var varNode = reinterpret_cast<Var&>(*node);
            string varValue = stack.lookup(varNode.value.strVal, varNode.line);
            return utils::toDouble(varValue);
            break;
        }
        case NodeType::program: {
            Program progNode = reinterpret_cast<Program&>(*node);
            stack.pushFrame(progNode.table);
            visit(progNode.block);
            break;
        }
        case NodeType::block: {
            Block block = reinterpret_cast<Block&>(*node);
            for (AST* decl : block.declarations) {
                visit(decl);
            }
            visit(block.compoundStatement);
            break;
        }
        case NodeType::varDecl: {
            break;
        }
        case NodeType::varType: {
            break;
        }
        case NodeType::procedureDecl: {
            break;
        }
        case NodeType::procedureCall: {
            ProcedureCall* procCallNode = reinterpret_cast<ProcedureCall*>(node);
            ProcedureDecl* procDeclNode = reinterpret_cast<ProcedureDecl*>(procCallNode->procDeclNode);
            // Check for matching number of formal and actual params.
            if (procCallNode->paramVals->size() != procDeclNode->params->size()) {
                utils::fatalError("Wrong number of parameters in call to " + procDeclNode->procName + " on line " + to_string(procCallNode->line));
            }
            ssize_t numParams = procCallNode->paramVals->size();
            Param* paramNode;
            Var* varNode;
            // Make an array for each of the formal and actual params.
            string finalParamVals[numParams];
            string paramNames[numParams];
            for (unsigned int i = 0;i<numParams;i++) {
                paramNode = reinterpret_cast<Param*>(procDeclNode->params->at(i));
                varNode = reinterpret_cast<Var*>(paramNode->varNode);
                finalParamVals[i] = to_string(visit(procCallNode->paramVals->at(i)));
                cout << "param val: " << finalParamVals[i] << endl;
                paramNames[i] = varNode->value.strVal;
            }
            // Push a new stack frame and assign params.
            stack.pushFrame(procDeclNode->table, paramNames, finalParamVals, numParams);
            // Run procedure body.
            visit(procDeclNode->blockNode);
            // Pop stack framee.
            stack.popFrame();
            break;
        }
        case NodeType::ifStatement: {
            IfStatement* ifStatementNode = reinterpret_cast<IfStatement*>(node);
            // Well isn't this code convenient...
            double condition = visit(ifStatementNode->conditionNode);
            cout << "If Condition result: " << condition << endl;
            if (condition) {
                visit(ifStatementNode->blockNode);
            }
            else if (ifStatementNode->elseBranch) {
                visit(ifStatementNode->elseBranch);
            }
            break;
        }
        case NodeType::whileStatement: {
            WhileStatement* whileStatementNode = reinterpret_cast<WhileStatement*>(node);
            double condition;
            while ((condition = visit(whileStatementNode->conditionNode))) {
                cout << "While condition result: " << condition << endl;
                visit(whileStatementNode->blockNode);
            }
            break;
        }
        default:
            utils::fatalError(string("Syntax tree node of type-index ") + to_string(node->type()) + string(" cannot be visited"));
    }
    return NAN;
}

double Interpreter::interpret() {
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
        std::vector<std::string>::const_iterator itr;
        itr =  std::find(this->tokens.begin(), this->tokens.end(), option);
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
    
    if (!fileName.empty()) {
        cout << "File: " << fileName << endl;
        ifstream file(fileName);
        stringstream buffer;
        buffer << file.rdbuf();
        string str = buffer.str();
        Lexer lexer = Lexer(str);
        Parser parser = Parser(&lexer);
        Interpreter interpreter = Interpreter(&parser);
        cout << interpreter.interpret() << endl;
    }
    return 0;
}
