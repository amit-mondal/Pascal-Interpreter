CXX = g++
CXXFLAGS = -g -Wall -Wextra -Wno-unused-parameter -std=c++11

headers = utils.h Token.h Symbol.h ASTNodes.h constants.h
sources = main.cpp Token.cpp Symbol.cpp ASTNodes.cpp
objectfiles = main.o Token.o Symbol.o ASTNodes.o


all: pas

pas: $(headers) $(sources) $(objectfiles)
	$(CXX) $(CXXFLAGS) $(objectfiles) -o pas


main.o: $(headers)
Token.o: Token.h
Symbol.o: Symbol.h utils.h
ASTNodes.o: $(headers)

clean:
	rm -f *.o *~ pas
