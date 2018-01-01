#include "Token.h"
using namespace std;

Token::Token(string type, char value, int line) : value(TokenVal(TokenValType::Char)){
    this->type = type;
    this->value.charVal = value;
    this->line = line;
}

Token::Token(string type, double value, int line) : value(TokenVal(TokenValType::Double)){
    this->type = type;
    this->value.numVal = value;
    this->line = line;
}

Token::Token(string type, string value, int line) : value(TokenVal(TokenValType::String)){
    this->type = type;
    this->value.strVal = value;
    this->line = line;
}

string Token::toString() const {
    return "Line " + to_string(line) + " " + "Token({" + type + "}, {" + value.toString() + "})";
};

std::ostream &operator<< (std::ostream &os, Token const &token) {
    os << token.toString();
    return os;
}
