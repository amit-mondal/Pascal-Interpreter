#include "utils.h"
#include "Lexer.h"

using namespace std;

Lexer::Lexer(string input) {
    this->input = input;
    pos = 0;
    line = 1;
    currentChar = input[pos];
}

void Lexer::error(char ch) {
    utils::fatalError(string("Invalid character at line " + to_string(line) + ": ") + "\'" + ch + "\'");
}

void Lexer::advance() {
    pos++;
    if (pos > input.length() - 1) currentChar = 0;
    else currentChar = input[pos];
    if (currentChar == '\n' || currentChar == '\r') line++;
}

void Lexer::skipWhiteSpace() {
    while (currentChar != 0 &&
	   (currentChar == ' '
	    || currentChar == '\n'
	    || currentChar == '\t'
	    || currentChar == '\r')) {
        advance();
    }
}

void Lexer::skipComment() {
    while (currentChar != '}') {
        advance();
    }
    advance();
}

Token* Lexer::number() {
    string res = "";
    while (currentChar != 0 && isdigit(currentChar)) {
        res += currentChar;
        advance();
    }
    if (currentChar == '.') {
        res += currentChar;
        advance();
        while (currentChar != 0 && isdigit(currentChar)) {
            res += currentChar;
            advance();
        }
        return new Token(ttype::real_const, utils::toDouble(res), line);
    }
    return new Token(ttype::int_const, utils::toDouble(res), line);
}

Token* Lexer::getNextToken() {
    while (currentChar != 0) {
        if (currentChar == ' ' || currentChar == '\n') {
            skipWhiteSpace();
            continue;
        }
        if (currentChar == '{') {
            advance();
            skipComment();
            continue;
        }
        if (currentChar == '"') {
            return stringLiteral();
        }
        if (isdigit(currentChar)) {
            return number();
        }
        if (isalpha(currentChar)) {
            return id();
        }
        if (currentChar == ':' && peek() == '=') {
            advance();
            advance();
            return new Token(ttype::assign, ":=", line);
        }
        if (currentChar == ':') {
            advance();
            return new Token(ttype::colon, ':', line);
        }
        if (currentChar == ';') {
            advance();
            return new Token(ttype::semi, ';', line);
        }
        if (currentChar == '.') {
	    char next = peek();
	    if (next != '\0' && isdigit(next)) {
		return number();
	    }
	    advance();
            return new Token(ttype::dot, '.', line);
        }
        if (currentChar == ',') {
            advance();
            return new Token(ttype::comma, ',', line);
        }
        if (currentChar == '+') {
            advance();
            return new Token(ttype::plus, '+', line);
        }
        if (currentChar == '-') {
            advance();
            return new Token(ttype::minus, '-', line);
        }
        if (currentChar == '*') {
            advance();
            return new Token(ttype::mul, '*', line);
        }
        if (currentChar == '/') {
            advance();
            return new Token(ttype::float_div, '/', line);
        }
        if (currentChar == '(') {
            advance();
            return new Token(ttype::lparen, '(', line);
        }
        if (currentChar == ')') {
            advance();
            return new Token(ttype::rparen, ')', line);
        }
        if (currentChar == '=') {
            advance();
            return new Token(ttype::equals, '=', line);
        }
        if (currentChar == '!') {
            if (peek() == '=') {
                advance();
                advance();
                return new Token(ttype::not_equals, "!=", line);
            }
            else {
                advance();
                return new Token(ttype::bang, '!', line);
            }
        }
        if (currentChar == '<') {
            if (peek() == '=') {
                advance();
                advance();
                return new Token(ttype::lt_or_equals, "<=", line);
            }
            else {
                advance();
                return new Token(ttype::less_than, '<', line);
            }
        }
        if (currentChar == '>') {
            if (peek() == '=') {
                advance();
                advance();
                return new Token(ttype::gt_or_equals, ">=", line);
            }
            else {
                advance();
                return new Token(ttype::greater_than, '>', line);
            }
        }
        error(currentChar);
    }
    return new Token(ttype::eof, '\0', line);
}

char Lexer::peek() {
    return (pos + 1) > input.length() - 1 ? '\0' : input[pos + 1];
}

Token* Lexer::id() {
    //handles identifiers and reserved keywords
    string result = "";
    while(currentChar != '\0' && isalnum(currentChar)) {
        result += toupper(currentChar);
        advance();
    }
    unordered_map<string, Token*>::const_iterator iter = RESERVED_KEYWORDS.find(result);
    if (iter != RESERVED_KEYWORDS.end()) {
        iter->second->line = line;
        return iter->second;
    }
    else {
        return new Token(ttype::id, result, line);
    }
}

Token* Lexer::stringLiteral() {
    //handles identifiers and reserved keywords
    advance();
    string result = "";
    while(currentChar != '\0') {
        if (currentChar == '"') {
            advance();
            break;
        }
        result += currentChar;
        advance();
    }
    return new Token(ttype::string_literal, result, line);
}
