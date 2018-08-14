#include <unordered_map>
#include "Token.h"

/*
 * Lexer class definition
 */

class Lexer {
public:
    std::string input;
    unsigned int pos;
    int line;
    char currentChar;
    Lexer(std::string input);
    void error(char ch);
    void advance();
    void skipWhiteSpace();
    void skipComment();
    Token* number();
    Token* getNextToken();
    char peek();
    Token* id();
    Token* stringLiteral();
    
private:
    const std::unordered_map<std::string, Token*> RESERVED_KEYWORDS = {
        { "BEGIN" , new Token(ttype::begin, "BEGIN", -1) },
        { "END", new Token(ttype::end, "END", -1) },
        { "PROGRAM", new Token(ttype::program, "PROGRAM", -1) },
        { "VAR", new Token(ttype::var, "VAR", -1) },
        { "DIV", new Token(ttype::int_div, "DIV", -1) },
        { "INTEGER", new Token(ttype::integer, "INTEGER", -1) },
        { "REAL", new Token(ttype::real, "REAL", -1) },
        { "PROCEDURE", new Token(ttype::procedure, "PROCEDURE", -1) },
        { "WHILE", new Token(ttype::twhile, "WHILE", -1)},
        { "IF", new Token(ttype::tif, "IF", -1)},
        { "THEN", new Token(ttype::then, "THEN", -1)},
        { "ELSE", new Token(ttype::telse, "ELSE", -1)},
        { "DO", new Token(ttype::tdo, "DO", -1)},
        { "STRING", new Token(ttype::string, "STRING", -1)},
	{ "RECORD", new Token(ttype::record, "RECORD", -1)},
	{ "TYPE", new Token(ttype::type, "TYPE", -1)}
    };
};
