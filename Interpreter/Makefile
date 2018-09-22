CXX = g++
CXXFLAGS = -g3 -Wall -Wextra -Wno-unused-parameter -std=c++11

headers = utils.h Token.h Symbol.h ASTNodes.h DataVal.h constants.h CallStack.h ScopedSymbolTable.h options.h Lexer.h Parser.h SemanticAnalyzer.h
sources = main.cpp Token.cpp Symbol.cpp ASTNodes.cpp DataVal.cpp CallStack.cpp ScopedSymbolTable.cpp options.cpp Lexer.cpp Parser.cpp SemanticAnalyzer.cpp
objectfiles = main.o Token.o Symbol.o ASTNodes.o DataVal.o CallStack.o ScopedSymbolTable.o options.o Lexer.o Parser.o SemanticAnalyzer.o


all: pas

pas: $(headers) $(sources) $(objectfiles)
	$(CXX) $(CXXFLAGS) $(objectfiles) -o pas

clean:
	rm -f *.o *~ pas
