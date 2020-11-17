/****************************************
 Call Stack
***************************************/
#include <unordered_map>
#include <unordered_set>
#include <vector>
#include "ScopedSymbolTable.h"
#include "DataVal.h"
#include "constants.h"

#ifndef CALLSTACK_H
#define CALLSTACK_H

class CallStack {
public:
    CallStack();
    void pushFrame(ScopedSymbolTable* symbolTable);
    void pushFrame(ScopedSymbolTable* symbolTable, std::string* formalParams, DataVal* actualParams, ssize_t numParams);
    void popFrame(void* retValPtr = nullptr);
    void assign(std::string key, DataVal value, int line);
    DataVal lookup(std::string key, int line);
    void printCurrentFrame() const;
    bool empty() const;
private:
    struct StackFrame {
	std::unordered_map<std::string, DataVal> valTable;
	std::unordered_set<std::string> paramNames;
        StackFrame* parent;
        ScopedSymbolTable* symbolTable;
        StackFrame(ScopedSymbolTable* symbolTable, StackFrame* parent = nullptr) : parent(parent), symbolTable(symbolTable) {}
	void dump() const {
	    std::cout << "______________________________" << std::endl;
	    std::cout << "Frame: " << symbolTable->name() << std::endl;
	    for(auto elem : valTable) {
		std::cout << elem.first  << " : " << elem.second << std::endl;
	    }
	    std::cout << "______________________________" << std::endl;
	}
	
    };
    DataVal foundVal;
    StackFrame* findFrame(std::string key);
    StackFrame* currentFrame;
    int callStackDepth;
};

#endif
