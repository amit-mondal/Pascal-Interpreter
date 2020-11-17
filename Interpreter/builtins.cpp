#include <iostream>
#include <chrono>
#include <thread>
#include "builtins.h"
#include "DataVal.h"

using namespace std;

void builtin::error(const std::string& err, const std::string& name) {
    utils::fatalError("Error in built-in function " + name + ": " + err);
}

BUILTIN(DUMP) {    
    cout << args[0].toString();
    return DataVal();
}

BUILTIN(PRINT) {
    cout << args[0].toString();
    return DataVal();
}

BUILTIN(PRINTLN) {
    cout << args[0].toString() << endl;
    return DataVal();
}

BUILTIN(SLEEP) {
    this_thread::sleep_for(chrono::milliseconds((int) DATAVAL_GET_VAL(double, args[0].data)));
    return DataVal();
}

BUILTIN(STRMODIFY) {
    string* str = DATAVAL_GET_PTR(string, args[0]);
    string other_str = DATAVAL_GET_VAL(string, args[1].data);
    int index = (int) DATAVAL_GET_VAL(double, args[2].data);

    cout << *str << " " << other_str << " " << index << endl;
    
    if (other_str.size() != 1) {
	builtin::error("expected second string \"" + other_str + "\" to be of length 1", "STRMODIFY");
    }
    if ((unsigned long) index > str->size() - 1) {
	builtin::error("index must be within the bounds of the first string", "STRMODIFY");
    }
    (*str)[index] = other_str[0];
    return DataVal();
}

BUILTIN(PANIC) {
    cout << "Panicking! Stack trace:" << endl;
    while (!stack->empty()) {
	stack->printCurrentFrame();
	stack->popFrame();
    }
    exit(1);
    return DataVal();
}

BUILTIN(BIND) {
    string& varname = DATAVAL_GET_VAL(string, args[0].data);
    stack->assign(varname, args[1], -1);
    stack->lookup(varname, -1);
    return DataVal();
}

BUILTIN(PARSEINT) {
    const string& intstr = DATAVAL_GET_VAL(string, args[0].data);
    int res = std::stoi(intstr);
    return DataVal::allocator.allocate(res);
}

BUILTIN(INPUT) {
    string res;
    getline(cin, res);
    return DataVal::allocator.allocate(res);
}

BUILTIN(INT_TO_REAL) {
    int val = DATAVAL_GET_VAL(int, args[0].data);
    return DataVal::allocator.allocate((double) val);
}

BUILTIN(REAL_TO_INT) {
    double val = DATAVAL_GET_VAL(double, args[0].data);
    return DataVal::allocator.allocate((int) val);
}
