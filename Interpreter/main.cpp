
#include <iostream>
#include <fstream>
#include <sstream>
#include "Interpreter.h"
#include "Parser.h"
#include "options.h"

using namespace std;


/*********************************
 Command Line Args Parser
********************************/

class InputParser{
public:
    InputParser (int &argc, char **argv){
        for (int i=1; i < argc; ++i)
            this->tokens.push_back(std::string(argv[i]));
    }
    const std::string& getCmdOption(const std::string &option) const{
        auto itr =  std::find(this->tokens.begin(), this->tokens.end(), option);
        if (itr != this->tokens.end() && ++itr != this->tokens.end()){
            return *itr;
        }
        return empty;
    }
    bool cmdOptionExists(const std::string &option) const{
        return std::find(this->tokens.begin(), this->tokens.end(), option)
	    != this->tokens.end();
    }
private:
    const std::string empty;
    std::vector <std::string> tokens;
};

int main(int argc, char *argv[]) {
    InputParser input(argc, argv);
    const string fileName = input.getCmdOption("-f");

    DEFINE_CMD_LINE_OPT(input, printTokens, "-pt", "--print-tokens");
    DEFINE_CMD_LINE_OPT(input, dumpVars, "-dv", "--dump-vars");
    DEFINE_CMD_LINE_OPT(input, showST, "-sst", "--show-symbol-table");
    DEFINE_CMD_LINE_OPT(input, showConditions, "-sc", "--show-conditions");
    DEFINE_CMD_LINE_OPT(input, staticTypeChecking, "-stc", "--static-type-checking");
    DEFINE_CMD_LINE_OPT(input, showAllocations, "-sa", "--show-allocations");
    
    if (!fileName.empty()) {
        ifstream file(fileName);
        stringstream buffer;
        buffer << file.rdbuf();
        string str = buffer.str();
        Lexer lexer = Lexer(str);
        Parser parser = Parser(&lexer);
        Interpreter interpreter = Interpreter(&parser);
	interpreter.interpret();
    }
    return 0;
}
