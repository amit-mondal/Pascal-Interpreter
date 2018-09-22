
#include "Parser.h"

using namespace std;

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
    int line = this->line();
    AST* left = this->variable();
    Token* token = currentToken;
    this->eat(ttype::assign);
    AST* right = this->expr();
    Assign* assignmentStatement = new Assign(left, token, right);
    assignmentStatement->line = line;
    return assignmentStatement;
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
