#pragma once

#include<iostream>
#include<windows.h>
#include "language.h"
#include <cctype>

using namespace std;

class LEXER {
    private:
        char* buffer;
        unsigned int buffer_size;
        HANDLE hOutputFile;
        DFA* dfa;

    public:
        LEXER(char* buffer, HANDLE hOuputFile, DFA* dfa){ this->buffer = buffer; this->dfa = dfa; }
        
        bool compute_flow(HANDLE hOutputFile);
        bool run_types(NODE* head, char* type, unsigned int& index);
};
