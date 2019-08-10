
#include <iostream>
#include "builtins.h"
#include "DataVal.h"

using namespace std;

void builtin::error(const std::string& err, const std::string& name) {
    utils::fatalError("Error in built-in function " + name + ": " + err);
}

BUILTIN(PRINT) {
    cout << args[0].toString();
    return DataVal();
}

BUILTIN(PRINTLN) {
    cout << args[0].toString() << endl;
    return DataVal();
}

BUILTIN(STOI) {
    return DataVal(stoi(args[0].toString()));
}
