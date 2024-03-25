#pragma once

#include<iostream>
#include<windows.h>
#include "language.h"

using namespace std;

class LEXER {
    private:
        char* buffer;
        HANDLE hOutputFile;
        DFA* dfa;

    public:
        LEXER(char* buffer, HANDLE hOuputFile, DFA* dfa): buffer(NULL), hOutputFile(0) {}
        
        //char* getBuffer();
        //HANDLE gethOutputFile();

        bool compute_flow();
};
