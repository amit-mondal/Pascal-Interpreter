#include "Symbol.h"
using namespace std;

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




BuiltInTypeSymbol::BuiltInTypeSymbol(string name): Symbol(name) {
    
}

string BuiltInTypeSymbol::toString() const {
    return "BuiltInTypeSymbol<name=" + this->name + ">";
}


UserDefinedTypeSymbol::UserDefinedTypeSymbol(string name): Symbol(name) {
    
}

string UserDefinedTypeSymbol::toString() const {
    return "UserDefinedTypeSymbol<name=" + this->name + ">";
}



VarSymbol::VarSymbol(string name, Symbol* type) : Symbol(name, type) {
    
}

string VarSymbol::toString() const {
    return "VarSymbol<name={" + name + "}:{type=" + type->toString() + "}>";
}




ProcedureSymbol::ProcedureSymbol(string name, vector<Symbol*>* params) : Symbol(name), params(params) {
}

ProcedureSymbol::ProcedureSymbol(string name) : Symbol(name) {
    params = new vector<Symbol*>();
}

string ProcedureSymbol::toString() const {
    string paramStr = "[";
    for (Symbol* symbol : *params) {
        paramStr += symbol->toString() + ", ";
    }
    return "ProcedureSymbol<{name=" + name + "}:{params=" + paramStr + "}>";
}
