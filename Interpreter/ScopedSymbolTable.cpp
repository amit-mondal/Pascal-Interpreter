#include "ScopedSymbolTable.h"
#include "options.h"

using namespace std;

// Declare static members.
const int ScopedSymbolTable::NUM_BUILTINS;
BuiltInTypeSymbol* ScopedSymbolTable::builtInsMap[NUM_BUILTINS];

string ScopedSymbolTable::name() {
    return scopeName;
}

ScopedSymbolTable::ScopedSymbolTable(string scopeName, int scopeLevel, ScopedSymbolTable* enclosingScope = nullptr) : scopeLevel(scopeLevel), scopeName(scopeName) {
    this->enclosingScope = enclosingScope;
    this->initBuiltIns();
    
    define(builtInsMap[builtInSymbols::INT]);
    define(builtInsMap[builtInSymbols::REAL]);
    define(builtInsMap[builtInSymbols::STRING]);
    // If we only want static type checking, we don't define the "any" type.
    if (!options::staticTypeChecking) {
	define(builtInsMap[builtInSymbols::ANY]);
    }
}

void ScopedSymbolTable::define(Symbol* symbol) {
    if (options::showST) cout << "Define: " + symbol->toString() << endl;
    symbols.add(symbol->name, symbol);
}

Symbol* ScopedSymbolTable::lookup(string name, bool currScope) {
    if (options::showST) cout << "Lookup: " + name << endl;
    Symbol* res = symbols.get(name);
    if (res) return res;
    if (currScope) return nullptr;
    else if (enclosingScope) return enclosingScope->lookup(name);
    return nullptr;
}

bool ScopedSymbolTable::initBuiltIns() {
    builtInsMap[builtInSymbols::INT] = new BuiltInTypeSymbol("INTEGER");
    builtInsMap[builtInSymbols::REAL] = new BuiltInTypeSymbol("REAL");
    builtInsMap[builtInSymbols::STRING] = new BuiltInTypeSymbol("STRING");    
    builtInsMap[builtInSymbols::ANY] = new BuiltInTypeSymbol("ANY");
    return false;
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
