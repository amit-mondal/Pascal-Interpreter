
#ifndef INTERPRETER_H
#define INTERPRETER_H

#include "DataVal.h"
#include "CallStack.h"
#include "Parser.h"


class Interpreter {
public:
    Parser* parser;
    Interpreter(Parser* parser);
    DataVal visit(AST* node);
    DataVal interpret();
private:
    void error(const std::string& msg, int line=-1);
    CallStack stack;
};


#endif
