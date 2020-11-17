
#include <string>
#include <unordered_set>
#include <map>
#include <unordered_map>
#include "constants.h"
#include "Token.h"
#include "Symbol.h"
#include "ASTNodes.h"
#include "SemanticAnalyzer.h"
#include "Interpreter.h"
#include "options.h"
#include "builtins.h"

using namespace std;

Interpreter::Interpreter(Parser* parser) : parser(parser) {}

DataVal Interpreter::visit(AST* node) {
    if (node == nullptr) utils::fatalError(string("Parse tree is null"));
    switch(node->type()) {
    case NodeType::binOp: {
	BinOp binNode = dynamic_cast<BinOp&>(*node);
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
	/*
	else if (opType == ttype::int_div) {
	    return (int(visit(binNode.left)) / int(visit(binNode.right)));
	}
	*/
	else if (opType == ttype::float_div) {
	    return visit(binNode.left) / visit(binNode.right);
	}
	if (opType == ttype::equals) {
	    DataVal left = visit(binNode.left);
	    DataVal right = visit(binNode.right);
	    if (options::showConditions) {
		cout << "left: " << left.toString() << " right: " << right.toString() << endl;
	    }
	    return DataVal::allocator.allocate(left == right);
	}
	else if (opType == ttype::not_equals) {
	    DataVal left = visit(binNode.left);
	    DataVal right = visit(binNode.right);
	    if (options::showConditions) {
		cout << "left: " << left.toString() << " right: " << right.toString() << endl;
	    }
	    return DataVal::allocator.allocate(left != right);
	}
	else if (opType == ttype::less_than) {
	    DataVal left = visit(binNode.left);
	    DataVal right = visit(binNode.right);
	    if (options::showConditions) {
		cout << "left: " << left.toString() << " right: " << right.toString() << endl;
	    }
	    return DataVal::allocator.allocate(left < right);
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
		    return DataVal::allocator.allocate(-1 * DATAVAL_GET_VAL(int, exprResult.data));
		}
		case DataVal::D_REAL:
		    return DataVal::allocator.allocate(-1 * DATAVAL_GET_VAL(double, exprResult.data));
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
	
    case NodeType::varDecl:
    case NodeType::recordDecl:
    case NodeType::varType:
    case NodeType::procedureDecl: break;
	
    case NodeType::procedureCall: {
	ProcedureCall* procCallNode = dynamic_cast<ProcedureCall*>(node);
	ProcedureDecl* procDeclNode = dynamic_cast<ProcedureDecl*>(procCallNode->procDeclNode);
	size_t numParams = procCallNode->paramVals->size();
	Param* paramNode;
	Var* varNode;
	DataVal finalParamVals[numParams];

	// Run built-in functions by calling the built-in handler.
	auto itr = builtin::FUNCTIONS.find(procCallNode->procName);
	if (itr != builtin::FUNCTIONS.end()) {

	    for (unsigned int i = 0; i < numParams; i++) {
		finalParamVals[i] = visit(procCallNode->paramVals->at(i));
	    }	    
	    return itr->second.fn(&this->stack, vector<DataVal>(finalParamVals, finalParamVals + numParams));
	}
	
	// Make an array for each of the formal and actual params.
	string paramNames[numParams];
	for (unsigned int i = 0;i<numParams;i++) {
	    paramNode = dynamic_cast<Param*>(procDeclNode->params->at(i));
	    varNode = paramNode->varNode;
	    finalParamVals[i] = visit(procCallNode->paramVals->at(i));
	    paramNames[i] = varNode->value.strVal;
	}
	
	// Push a new stack frame and assign params.
	stack.pushFrame(procDeclNode->table, paramNames, finalParamVals, numParams);
	try {
	    // Run procedure body.
	    visit(procDeclNode->blockNode);
	} catch (DataVal returnVal) {
	    // Make sure not to free the value we just returned by passing it to the call stack.
	    stack.popFrame(returnVal.data);
	    return returnVal;
	}
	// Pop stack frame.
	stack.popFrame();

	if (procDeclNode->returnTypeNode != nullptr) {
	    utils::fatalError("Reached end of non-void procedure " + procDeclNode->procName + " without returning a value");
	}
	break;
    }

    case NodeType::returnStatement: {
	ReturnStatement* retStatementNode = dynamic_cast<ReturnStatement*>(node);
	throw this->visit(retStatementNode->expr);
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
