#ifndef TOKEN_H
#define TOKEN_H

#include <string>
#include <iostream>

/*
 * Token types grouped into type namespace
 */
namespace ttype {
    const std::string eof = "EOF";
    const std::string integer = "INTEGER";
    const std::string plus = "PLUS";
    const std::string minus = "MINUS";
    const std::string mul = "MUL";
    const std::string float_div = "FLOAT_DIV";
    const std::string lparen = "(";
    const std::string rparen = ")";
    const std::string begin = "BEGIN";
    const std::string end = "END";
    const std::string assign = "ASSIGN";
    const std::string semi = "SEMI";
    const std::string dot = "DOT";
    const std::string id = "ID";
    const std::string program = "PROGRAM";
    const std::string var = "VAR";
    const std::string int_div = "INTEGER_DIV";
    const std::string real = "REAL";
    const std::string int_const = "INTEGER_CONST";
    const std::string real_const = "REAL_CONST";
    const std::string colon = "COLON";
    const std::string comma = "COMMA";
    const std::string procedure = "PROCEDURE";
    const std::string twhile = "WHILE";
    const std::string tdo = "DO";
    const std::string tif = "IF";
    const std::string then = "THEN";
    const std::string telse = "ELSE";
    const std::string equals = "EQUALS";
    const std::string not_equals = "NOT_EQUALS";
    const std::string less_than = "LESS_THAN";
    const std::string greater_than = "GREATER_THAN";
    const std::string lt_or_equals = "LT_OR_EQUALS";
    const std::string gt_or_equals = "GT_OR_EQUALS";
    const std::string bang = "BANG";
    const std::string string_literal = "STRING_LITERAL";
    const std::string string = "STRING";
    const std::string any = "ANY";
    const std::string record = "RECORD";
    const std::string type = "TYPE";
}

/*
 * Enum to determine the type of data in TokenVal
 */
enum TokenValType {
    Char,
    Double,
    String,
};

/*
 * Struct to hold the value of a token
 */

struct TokenVal {
    double numVal;
    char charVal;
    std::string strVal;
    int intVal;
    TokenValType type;
    TokenVal(TokenValType type) {
        this->type = type;
    }
    ~TokenVal() {
        strVal.clear();
    }
    std::string toString() const {
        switch(type) {
	case TokenValType::String: {
	    return strVal;
	}
	case TokenValType::Double: {
	    return std::to_string(numVal);
	}
	case TokenValType::Char: {
	    std::string res = " ";
	    res[0] = charVal;
	    return res;
	}
        }
    }
};

/*
 * Token class definition
 */
class Token {
public:
    std::string type;
    int line;
    TokenVal value;
    Token(std::string type, char value, int line);
    Token(std::string type, double value, int line);
    Token(std::string type, std::string value, int line);
    std::string toString() const;
    friend std::ostream &operator<< (std::ostream& os, Token const& token);
};

#endif
