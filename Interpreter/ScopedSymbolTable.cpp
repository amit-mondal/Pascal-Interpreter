#include "ScopedSymbolTable.h"

using namespace std;

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

Symbol* ScopedSymbolTable::lookup(string name, bool currScope) {
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
    define(new BuiltInTypeSymbol("STRING"));
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
