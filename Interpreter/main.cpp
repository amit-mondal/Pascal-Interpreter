// Example program
#include <iostream>
#include <string>
#include <cctype>
#include <cmath>
#include <vector>
#include <map>
#include <algorithm>
#include <fstream>
#include <sstream>

using namespace std;

/*
 * Runtime options
 */

namespace options {
    bool printTokens = false;
    bool dumpVars = false;
    bool showST = false;
}

/*
 * Utility functions grouped into utils namespace
 */

class AST;

namespace utils {
    int toInt(const char c) {
        if (!isdigit(c)) throw std::invalid_argument("Non-digit character found");
        return c - '0';
    }
    
    double toDouble(const string s) {
        double result = stod(s);
        return result;
    }
    
    bool inArray(const std::string &value, const std::vector<string> &array) {
        return std::find(array.begin(), array.end(), value) != array.end();
    }
    
    void dumpMap(const map<string, string> &m) {
        for(auto elem : m) {
            std::cout << elem.first  << " : " << elem.second << std::endl;
        }
    }
    
    void combineArrs(vector<AST*> a, vector<AST*> b) {
        move(b.begin(), b.end(), back_inserter(a));
    }
    
    template <typename K, typename V> class OrderedMap {
    public:
        map<K, V> m_map;
        vector<K> order;
        V get(K key) const {
            if (m_map.find(key) == m_map.end()) throw std::invalid_argument("Key not found in ordered map");
            return m_map.find(key)->second;
        }
        V add(K key, V value) {
            m_map[key] = value;
            order.insert(order.begin(), key);
            return value;
        }
    };
}

/*
 * Token types grouped into type namespace
 */
namespace type {
    const string eof = "EOF";
    const string integer = "INTEGER";
    const string plus = "PLUS";
    const string minus = "MINUS";
    const string mul = "MUL";
    const string float_div = "FLOAT_DIV";
    const string lparen = "(";
    const string rparen = ")";
    const string begin = "BEGIN";
    const string end = "END";
    const string assign = "ASSIGN";
    const string semi = "SEMI";
    const string dot = "DOT";
    const string id = "ID";
    const string program = "PROGRAM";
    const string var = "VAR";
    const string int_div = "INTEGER_DIV";
    const string real = "REAL";
    const string int_const = "INTEGER_CONST";
    const string real_const = "REAL_CONST";
    const string colon = "COLON";
    const string comma = "COMMA";
    const string procedure = "PROCEDURE";
    
}

/*
 * Enum to determine the type of data in TokenVal
 */
enum TokenValType {
    Char,
    Double,
    String
};

/*
 * Struct to hold the value of a token
 */

struct TokenVal {
    double numVal;
    char charVal;
    string strVal;
    TokenValType type;
    TokenVal(TokenValType type) {
        this->type = type;
    }
    ~TokenVal() {
        strVal.clear();
    }
    string toString() const {
        switch(type) {
            case TokenValType::String: {
                return strVal;
                break;
            }
            case TokenValType::Double: {
                return to_string(numVal);
                break;
            }
            case TokenValType::Char: {
                string res = " ";
                res[0] = charVal;
                return res;
                break;
            }
        }
    }
};

/*
 * Token class definition
 */
class Token {
public:
    string type;
    int line;
    TokenVal value;
    Token(string type, char value, int line);
    Token(string type, double value, int line);
    Token(string type, string value, int line);
    string toString() const;
};

Token::Token(string type, char value, int line) : value(TokenVal(TokenValType::Char)){
    this->type = type;
    this->value.charVal = value;
    this->line = line;
}

Token::Token(string type, double value, int line) : value(TokenVal(TokenValType::Double)){
    this->type = type;
    this->value.numVal = value;
    this->line = line;
}

Token::Token(string type, string value, int line) : value(TokenVal(TokenValType::String)){
    this->type = type;
    this->value.strVal = value;
    this->line = line;
}

string Token::toString() const {
    return "Line " + to_string(line) + " " + "Token({" + type + "}, {" + value.toString() + "})";
};

std::ostream &operator<< (std::ostream &os, Token const &token) {
    os << token.toString();
    return os;
}

/*
 * Symbol class definition
 */

class Symbol {
public:
    string name;
    Symbol* type;
    Symbol(string name);
    Symbol(string name, Symbol* type);
    virtual string toString() const = 0;
};

Symbol::Symbol(string name) {
    this->name = name;
    type = nullptr;
}

Symbol::Symbol(string name, Symbol* type) {
    this->name = name;
    this->type = type;
}

std::ostream &operator<< (std::ostream &os, Symbol const &symbol) {
    os << symbol.toString();
    return os;
}

/*
 * Built-in symbol subclass
 */

class BuiltInTypeSymbol: public Symbol {
public:
    BuiltInTypeSymbol(string name);
    string toString() const;
};

BuiltInTypeSymbol::BuiltInTypeSymbol(string name): Symbol(name) {
    
}

string BuiltInTypeSymbol::toString() const {
    return this->name;
}

/*
 * Variable symbol subclass
 */

class VarSymbol: public Symbol {
public:
    VarSymbol(string name, Symbol* type);
    string toString() const;
};

VarSymbol::VarSymbol(string name, Symbol* type) : Symbol(name, type) {
    
}

string VarSymbol::toString() const {
    return "<{" + name + "}:{" + type->toString() + "}>";
}

/*
 * Symbol table class definition
 */

class SymbolTable {
public:
    utils::OrderedMap<string, Symbol*> symbols;
    SymbolTable();
    void define(Symbol* symbol);
    Symbol* lookup(string name);
    void initBuiltIns();
    string toString() const;
};

SymbolTable::SymbolTable() {
    
}

void SymbolTable::define(Symbol* symbol) {
    if (options::showST) cout << "Define: " + symbol->toString();
    symbols.add(symbol->name, symbol);
    initBuiltIns();
}

Symbol* SymbolTable::lookup(string name) {
    if (options::showST) cout << "Lookup: " + name;
    return symbols.get(name);
}

void SymbolTable::initBuiltIns() {
    define(new BuiltInTypeSymbol("INTEGER"));
    define(new BuiltInTypeSymbol("REAL"));
}

string SymbolTable::toString() const {
    string result = "Symbols: ";
    for (string key : symbols.order) {
        result += this->symbols.get(key)->toString() + ", ";
    }
    return result;
}

/*
 * Lexer class definition
 */

class Lexer {
public:
    string input;
    int pos;
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
        { "BEGIN" , new Token(type::begin, "BEGIN", -1) },
        { "END", new Token(type::end, "END", -1) },
        { "PROGRAM", new Token(type::program, "PROGRAM", -1) },
        { "VAR", new Token(type::var, "VAR", -1) },
        { "DIV", new Token(type::int_div, "DIV", -1) },
        { "INTEGER", new Token(type::integer, "INTEGER", -1) },
        { "REAL", new Token(type::real, "REAL", -1) },
        { "PROCEDURE", new Token(type::procedure, "PROCEDURE", -1) }
    };
};

Lexer::Lexer(string input) {
    this->input = input;
    pos = 0;
    line = 1;
    currentChar = input[pos];
}

void Lexer::error(char ch) {
    throw std::invalid_argument(string("Invalid character at line " + to_string(line) + ": ") + ch);
}

void Lexer::advance() {
    pos++;
    if (pos > input.length() - 1) currentChar = '\0';
    else currentChar = input[pos];
    if (currentChar == '\n') line++;
}

void Lexer::skipWhiteSpace() {
    while (currentChar != '\0' && (currentChar == ' ' || currentChar == '\n')) {
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
    while (currentChar != '\0' && isdigit(currentChar)) {
        res += currentChar;
        advance();
    }
    if (currentChar == '.') {
        res += currentChar;
        advance();
        while (currentChar != '\0' && isdigit(currentChar)) {
            res += currentChar;
            advance();
        }
        return new Token(type::real_const, utils::toDouble(res), line);
    }
    return new Token(type::int_const, utils::toDouble(res), line);
}

Token* Lexer::getNextToken() {
    while (currentChar != '\0') {
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
            return new Token(type::assign, ":=", line);
        }
        if (currentChar == ':') {
            advance();
            return new Token(type::colon, ':', line);
        }
        if (currentChar == ';') {
            advance();
            return new Token(type::semi, ';', line);
        }
        if (currentChar == '.') {
            advance();
            return new Token(type::dot, '.', line);
        }
        if (currentChar == ',') {
            advance();
            return new Token(type::comma, ',', line);
        }
        if (currentChar == '+') {
            advance();
            return new Token(type::plus, '+', line);
        }
        if (currentChar == '-') {
            advance();
            return new Token(type::minus, '-', line);
        }
        if (currentChar == '*') {
            advance();
            return new Token(type::mul, '*', line);
        }
        if (currentChar == '/') {
            advance();
            return new Token(type::float_div, '/', line);
        }
        if (currentChar == '(') {
            advance();
            return new Token(type::lparen, '(', line);
        }
        if (currentChar == ')') {
            advance();
            return new Token(type::rparen, ')', line);
        }
        error(currentChar);
    }
    return new Token(type::eof, '\0', line);
}

char Lexer::peek() {
    return (pos + 1) > input.length() - 1 ? '\0' : input[pos + 1];
}

Token* Lexer::id() {
    //handles identifiers and reserved keywords
    string result = "";
    while(currentChar != '\0' && isalnum(currentChar)) {
        result += currentChar;
        advance();
    }
    map<string, Token*>::const_iterator iter = RESERVED_KEYWORDS.find(result);
    if (iter != RESERVED_KEYWORDS.end()) {
        iter->second->line = line;
        return iter->second;
    }
    else {
        return new Token(type::id, result, line);
    }
}

/****************************************
 Parser
 ***************************************/

/*
 * Define node types
 */

enum NodeType {
    none,
    binOp,
    num,
    unaryOp,
    assign,
    var,
    compound,
    program,
    block,
    varDecl,
    varType,
    procedureDecl
};

/*
 * Abstract Syntax Tree class definisions
 */

//Pure abstract class AST
class AST {
public:
    virtual NodeType type() const = 0;
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

BinOp::BinOp(AST* left, Token* op, AST* right) {
    this->left = left;
    this->op = op;
    this->right = right;
}

NodeType BinOp::type() const {
    return NodeType::binOp;
}

//Number leaf node
class Num: public AST {
public:
    double value;
    Token* token;
    Num(Token* token);
    virtual NodeType type() const;
};

Num::Num(Token* token) {
    this->token = token;
    this->value = token->value.numVal;
}

NodeType Num::type() const {
    return NodeType::num;
}

//Unary operation node
class UnaryOp: public AST {
public:
    Token* op;
    Token* token;
    AST* expr;
    UnaryOp(Token* op, AST* expr);
    virtual NodeType type() const;
};

UnaryOp::UnaryOp(Token* op, AST* expr) {
    this->token = op;
    this->op = op;
    this->expr = expr;
}

NodeType UnaryOp::type() const {
    return NodeType::unaryOp;
}

//Compound statement node
class Compound: public AST {
public:
    vector<AST*> children;
    Compound();
    virtual NodeType type() const;
};

Compound::Compound() {
    children = {};
}

NodeType Compound::type() const {
    return NodeType::compound;
}

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

Assign::Assign(AST* left, Token* op, AST* right) {
    this->left = left;
    this->right = right;
    this->op = op;
    this->token = op;
}

NodeType Assign::type() const {
    return NodeType::assign;
}

//Variable node
class Var: public AST {
public:
    Token* token;
    TokenVal value;
    Var(Token* token);
    virtual NodeType type() const;
};

Var::Var(Token* token) : token(token), value(token->value) {
    
}

NodeType Var::type() const {
    return NodeType::var;
}

//Empty statement node
class NoOp: public AST {
public:
    NodeType type() const;
};

NodeType NoOp::type() const {
    return NodeType::none;
}

//Program node
class Program: public AST {
public:
    string name;
    AST* block;
    Program(string name, AST* block);
    virtual NodeType type() const;
};

Program::Program(string name, AST* block) {
    this->name = name;
    this->block = block;
}

NodeType Program::type() const {
    return NodeType::program;
}

//Block node
class Block: public AST {
public:
    vector<AST*> declarations;
    AST* compoundStatement;
    Block(vector<AST*> declarations, AST* compoundStatement);
    virtual NodeType type() const;
};

Block::Block(vector<AST*> declarations, AST* compoundStatement) {
    this->declarations = declarations;
    this->compoundStatement = compoundStatement;
}

NodeType Block::type() const {
    return NodeType::block;
}

//Var declaration node
class VarDecl: public AST {
public:
    AST* varNode;
    AST* typeNode;
    VarDecl(AST* varNode, AST* typeNode);
    virtual NodeType type() const;
};

VarDecl::VarDecl(AST* varNode, AST* typeNode) {
    this->varNode = varNode;
    this->typeNode = typeNode;
}

NodeType VarDecl::type() const {
    return NodeType::varDecl;
}

//Procedure declaration noe
class ProcedureDecl: public AST {
public:
    string name;
    AST* blockNode;
    ProcedureDecl(string name, AST* blockNode);
    virtual NodeType type() const;
};

ProcedureDecl::ProcedureDecl(string name, AST* blockNode) {
    this->name = name;
    this->blockNode = blockNode;
}

NodeType ProcedureDecl::type() const {
    return NodeType::procedureDecl;
}

//Variable type
class VarType: public AST {
public:
    Token* token;
    TokenVal value;
    VarType(Token* token);
    NodeType type() const;
};

VarType::VarType(Token* token) : token(token), value(token->value) {
    
}

NodeType VarType::type() const {
    return NodeType::varType;
}

class Parser {
public:
    Lexer* lexer;
    Token* currentToken;
    Parser(Lexer* lexer);
    void error(string details);
    void eat(string tokenType);
    AST* factor();
    AST* term();
    AST* expr();
    AST* parse();
    AST* program();
    AST* compoundStatement();
    vector<AST*> statementList();
    AST* statement();
    AST* assignmentStatement();
    AST* variable();
    AST* empty();
    AST* block();
    vector<AST*> declarations();
    vector<AST*> variableDeclarations();
    AST* typeSpec();
};

Parser::Parser(Lexer* lexer) {
    this->lexer = lexer;
    currentToken = lexer->getNextToken();
    if (options::printTokens) cout << *currentToken << endl;
}

void Parser::error(string details) {
    throw std::invalid_argument("Invalid syntax: " + details);
}

void Parser::eat(string tokenType) {
    if (currentToken->type == tokenType) {
        currentToken = lexer->getNextToken();
        if (currentToken == nullptr) throw std::logic_error("Current token is null");
        if (options::printTokens) {
            cout << *currentToken << endl;
        }
    }
    else this->error("line " + to_string(currentToken->line) + ": " + tokenType + " expected, token of type " + currentToken->type + " with value " + currentToken->value.toString() + " found");
}

AST* Parser::factor() {
    /*
     factor : PLUS factor
     | MINUS factor
     | INTEGER_CONST
     | REAL_CONST
     | LPAREN expr RPAREN
     | variable
     */
    AST* result = nullptr;
    Token* token = currentToken;
    if (token->type == type::plus) {
        eat(type::plus);
        result = new UnaryOp(token, factor());
    }
    else if (token->type == type::minus) {
        eat(type::minus);
        result = new UnaryOp(token, factor());
    }
    else if (token->type == type::int_const) {
        eat(type::int_const);
        result = new Num(token);
    }
    else if (token->type == type::real_const) {
        eat(type::real_const);
        result = new Num(token);
    }
    else if (token->type == type::lparen) {
        eat(type::lparen);
        result = expr();
        eat(type::rparen);
    }
    else {
        result = variable();
    }
    return result;
}

AST* Parser::term() {
    //term : factor ((MUL | INTEGER_DIV | FLOAT_DIV) factor)*
    AST* result = factor();
    vector<string> ops = {type::mul, type::float_div, type::int_div};
    Token* token;
    while(utils::inArray(currentToken->type, ops)) {
        token = currentToken;
        if (token->type == type::mul) {
            eat(type::mul);
        }
        else if (token->type == type::int_div) {
            eat(type::int_div);
        }
        else if (token->type == type::float_div) {
            eat(type::float_div);
        }
        result = new BinOp(result, token, factor());
    }
    return result;
}

AST* Parser::expr() {
    //expr : term ((PLUS | MINUS) term)*
    AST* result = term();
    vector<string> ops = {type::plus, type::minus};
    Token* token;
    while(utils::inArray(currentToken->type, ops)) {
        token = currentToken;
        if (token->type == type::plus) {
            eat(type::plus);
        }
        else if (token->type == type::minus) {
            eat(type::minus);
        }
        result = new BinOp(result, token, term());
    }
    return result;
}

AST* Parser::parse() {
    AST* node = program();
    if (currentToken->type != type::eof) {
        error("EOF expected, " + currentToken->type + " found");
    }
    return node;
}

AST* Parser::program() {
    //program : PROGRAM variable SEMI block DOT
    eat(type::program);
    AST* node = variable();
    if (node->type() != NodeType::var) error("Variable expected before block");
    Var varNode = dynamic_cast<Var&>(*node);
    if (varNode.value.type != TokenValType::String) error("Program token did not return string for program name");
    string programName = varNode.value.strVal;
    eat(type::semi);
    AST* blockNode = block();
    AST* programNode = new Program(programName, blockNode);
    eat(type::dot);
    return programNode;
}

AST* Parser::compoundStatement() {
    //compound_statement: BEGIN statement_list END
    eat(type::begin);
    vector<AST*> nodes = statementList();
    eat(type::end);
    Compound* root = new Compound();
    for (AST* node : nodes) {
        root->children.push_back(node);
    }
    return root;
}

vector<AST*> Parser::statementList() {
    /*
     statement_list : statement
     | statement SEMI statement_list
     */
    AST* node = statement();
    vector<AST*> results = {node};
    while(currentToken->type == type::semi) {
        eat(type::semi);
        results.push_back(statement());
    }
    if (currentToken->type == type::id) {
        error("Unexpected ID found");
    }
    return results;
}

AST* Parser::statement() {
    /*
     statement : compound_statement
     | assignment_statement
     | empty
     */
    AST* node = nullptr;
    if (currentToken->type == type::begin) {
        node = compoundStatement();
    }
    else if (currentToken->type == type::id) {
        node = assignmentStatement();
    }
    else {
        node = empty();
    }
    return node;
}

AST* Parser::assignmentStatement() {
    //assignment_statement : variable ASSIGN expr
    AST* left = variable();
    Token* token = currentToken;
    eat(type::assign);
    AST* right = expr();
    return new Assign(left, token, right);
}

AST* Parser::variable() {
    //variable : ID
    AST* node = new Var(currentToken);
    eat(type::id);
    return node;
}

AST* Parser::empty() {
    return new NoOp();
}

AST* Parser::block() {
    //block : declarations compound_statement
    vector<AST*> declarationNodes = declarations();
    AST* compoundStatement = this->compoundStatement();
    return new Block(declarationNodes, compoundStatement);
}

vector<AST*> Parser::declarations() {
    /*
     declarations : VAR (variable_declaration SEMI)+
     | empty
     */
    vector<AST*> declarations = {};
    if (currentToken->type == type::var) {
        eat(type::var);
        while(currentToken->type == type::id) {
            vector<AST*> varDecl = variableDeclarations();
            utils::combineArrs(declarations, varDecl);
            eat(type::semi);
        }
    }
    while (currentToken->type == type::procedure) {
        eat(type::procedure);
        string procName = currentToken->value.strVal;
        eat(type::id);
        eat(type::semi);
        AST* blockNode = block();
        AST* procedureDecl = new ProcedureDecl(procName, blockNode);
        declarations.push_back(procedureDecl);
        eat(type::semi);
    }
    return declarations;
}

vector<AST*> Parser::variableDeclarations() {
    //variable_declaration : ID (COMMA ID)* COLON type_spec
    vector<AST*> varNodes = {new Var(currentToken)};
    eat(type::id);
    while(currentToken->type == type::comma) {
        eat(type::comma);
        varNodes.push_back(new Var(currentToken));
        eat(type::id);
    }
    eat(type::colon);
    AST* typeNode = typeSpec();
    vector<AST*> varDecls = {};
    for (AST* varNode : varNodes) {
        varDecls.push_back(new VarDecl(varNode, typeNode));
    }
    return varDecls;
}

AST* Parser::typeSpec() {
    /*
     type_spec : INTEGER
     | REAL
     */
    Token* token = currentToken;
    if (token->type == type::integer) {
        eat(type::integer);
    }
    else {
        eat(type::real);
    }
    return new VarType(token);
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
    map<string, string> GLOBAL_SCOPE;
};

Interpreter::Interpreter(Parser* parser) {
    this->parser = parser;
    Interpreter::GLOBAL_SCOPE = {};
}

double Interpreter::visit(AST* node) {
    if (node == nullptr) throw std::logic_error(string("Parse tree is null"));
    switch(node->type()) {
        case NodeType::binOp: {
            BinOp binNode = dynamic_cast<BinOp&>(*node);
            string opType = binNode.op->type;
            if (opType == type::plus) {
                return visit(binNode.left) + visit(binNode.right);
            }
            else if (opType == type::minus) {
                return visit(binNode.left) - visit(binNode.right);
            }
            else if (opType == type::mul) {
                return visit(binNode.left) * visit(binNode.right);
            }
            else if (opType == type::int_div) {
                return (int(visit(binNode.left)) / int(visit(binNode.right)));
            }
            else if (opType == type::float_div) {
                return visit(binNode.left) / visit(binNode.right);
            }
            break;
        }
        case NodeType::num: {
            Num num = dynamic_cast<Num&>(*node);
            return num.value;
            break;
        }
        case NodeType::unaryOp: {
            UnaryOp unaryNode = dynamic_cast<UnaryOp&>(*node);
            string opType = unaryNode.op->type;
            if (opType == type::plus) {
                return visit(unaryNode.expr);
            }
            else if (opType == type::minus) {
                return -1 * visit(unaryNode.expr);
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
            Assign assignNode = dynamic_cast<Assign&>(*node);
            Var varNode = dynamic_cast<Var&>(*assignNode.left);
            string varName = varNode.value.strVal;
            GLOBAL_SCOPE[varName] = to_string(visit(assignNode.right));
            if (options::dumpVars) {
                cout << "Dumping global variables..." << endl;
                utils::dumpMap(GLOBAL_SCOPE);
            }
            break;
        }
        case NodeType::var: {
            Var varNode = dynamic_cast<Var&>(*node);
            map<string, string>::iterator iter = GLOBAL_SCOPE.find(varNode.value.strVal);
            string varValue = iter->second;
            if (iter == GLOBAL_SCOPE.end()) {
                throw std::invalid_argument(string("Variable ") + varNode.value.strVal + string(" does not exist"));
            }
            else {
                return utils::toDouble(varValue);
            }
            break;
        }
        case NodeType::program: {
            Program progNode = dynamic_cast<Program&>(*node);
            visit(progNode.block);
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
        case NodeType::varType: {
            break;
        }
        case NodeType::procedureDecl: {
            break;
        }
        default:
            throw std::logic_error(string("Syntax tree node of type-index ") + to_string(node->type()) + string(" cannot be visited"));
    }
    return NAN;
}

double Interpreter::interpret() {
    AST* tree = parser->parse();
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
        cout << "file name: " << fileName << endl;
        ifstream file(fileName);
        stringstream buffer;
        buffer << file.rdbuf();
        string str = buffer.str();
        Lexer lexer = Lexer(str);
        Parser parser = Parser(&lexer);
        Interpreter interpreter = Interpreter(&parser);
        cout << interpreter.interpret() << endl;
    }
    /*
     string input;
     Lexer* lexer;
     Parser* parser;
     Interpreter* interpreter;
     while(true) {
     cout << ">> ";
     getline(cin, input);
     if (input.empty()) continue;
     lexer = new Lexer(input);
     parser = new Parser(lexer);
     interpreter = new Interpreter(parser);
     cout << interpreter->interpret() << endl;
     
     delete lexer;
     delete parser;
     delete interpreter;
     }
     */
    return 0;
}
