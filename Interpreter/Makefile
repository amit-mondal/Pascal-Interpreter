CXX = g++
CXXFLAGS = -g3 -Wall -Wextra -Wno-unused-parameter -std=c++11

headers = utils.h builtins.h Token.h Symbol.h ASTNodes.h Allocator.h DataVal.h constants.h CallStack.h ScopedSymbolTable.h options.h Lexer.h Parser.h SemanticAnalyzer.h
sources = main.cpp builtins.cpp Token.cpp Symbol.cpp ASTNodes.cpp Allocator.cpp DataVal.cpp CallStack.cpp ScopedSymbolTable.cpp options.cpp Lexer.cpp Parser.cpp SemanticAnalyzer.cpp
objectfiles = main.o builtins.o Token.o Symbol.o ASTNodes.o Allocator.o DataVal.o CallStack.o ScopedSymbolTable.o options.o Lexer.o Parser.o SemanticAnalyzer.o


all: pas

pas: $(headers) $(sources) $(objectfiles)
	$(CXX) $(CXXFLAGS) $(objectfiles) -o pas

clean:
	rm -f *.o *~ pas
