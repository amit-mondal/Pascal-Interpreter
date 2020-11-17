#include "utils.h"
#include "CallStack.h"
#include "DataVal.h"

using namespace std;

void CallStack::printCurrentFrame() const {
    currentFrame->dump();
}

bool CallStack::empty() const {
    return callStackDepth == 0;
}

CallStack::CallStack() : currentFrame(nullptr), callStackDepth(0) {
}

CallStack::StackFrame* CallStack::findFrame(string key) {
    StackFrame* frame = currentFrame;
    if (options::dumpVars) {
        cout << "******************************" << endl;
    }
    while(frame) {
        if (options::dumpVars) {
            frame->dump();
        }
        auto it = frame->valTable.find(key);
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

DataVal CallStack::lookup(string key, int line = -1) {
    if (findFrame(key)) {
        return foundVal;
    }
    utils::fatalError("Could not find value for variable reference '" + key + "' on line " + to_string(line));
    return DataVal();
}

void CallStack::assign(string key, DataVal value, int line = -1) {
    StackFrame* frame;
    if (!currentFrame) {
        utils::fatalError("Stack error: no initial frame pushed to call stack");
    }
    if (!currentFrame->symbolTable->lookup(key)) {
        utils::fatalError("Failed assignment to undeclared variable \"" + key + "\" on line " + to_string(line));
    }
    if (!(frame = this->findFrame(key))) {
        frame = currentFrame;
    }
    frame->valTable[key] = value;
    DataVal::allocator.incRefCount(value);
}

void CallStack::pushFrame(ScopedSymbolTable* symbolTable) {
    if (callStackDepth > CALL_STACK_MAX_DEPTH) {
        utils::fatalError("Stack error: call stack max depth exceeded; stack overflow");
    }
    StackFrame* newFrame = new StackFrame(symbolTable, currentFrame);
    currentFrame = newFrame;
    callStackDepth++;
}

void CallStack::pushFrame(ScopedSymbolTable *symbolTable, string *formalParams, DataVal *actualParams, ssize_t numParams) {
    // Since this is for procedure calls, we can always assign to the new frame.
    this->pushFrame(symbolTable);
    // Populate stack value table.
    for (unsigned int i = 0;i<numParams;i++) {
	currentFrame->paramNames.insert(formalParams[i]);
	currentFrame->valTable[formalParams[i]] = actualParams[i];
	DataVal::allocator.incRefCount(actualParams[i]);
    }
}

void CallStack::popFrame(void* retValPtr) {
    if (options::dumpVars) {	
	cout << "popping frame" << endl;
    }
    StackFrame* oldFrame = currentFrame;
    currentFrame = currentFrame->parent;
    // Can't delete the symbol table, because we might need it for other
    // frames.

    for (auto oldBinding : oldFrame->valTable) {
	if ((oldBinding.second.data != retValPtr) &&
	    oldFrame->paramNames.find(oldBinding.first) == oldFrame->paramNames.end()) {
	    DataVal::allocator.free(oldBinding.second);
	} else {
	    DataVal::allocator.decRefCount(oldBinding.second);
	}
    }
    
    delete oldFrame;
    if (!currentFrame && options::dumpVars) {
        cout << "Stack base frame popped" << endl;
    }

    callStackDepth--;
}
