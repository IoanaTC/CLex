#pragma once

#include<iostream>
#include<windows.h>

using namespace std;

#define TOKEN_MAGIC "KEY"
#define TOKEN_MAGIC_SIZE 3

#define REGEX_MAGIC "REGEX"
#define REGEX_MAGIC_SIZE 5
            
typedef struct NODE {
    
    char* regex;
    
    char* token;
    char* id;

    NODE** transitions;
    unsigned int transitions_count;

    bool is_final;
} NODE;

typedef struct DFA {
    NODE start;
    NODE* finalStates;
} DFA;


class LANGUAGE {
    private:
        char* spec_buffer;
        unsigned int spec_size;

    public:
        LANGUAGE(char* buffer = NULL, unsigned int size = 0) { spec_buffer = buffer; spec_size = size; }

        DFA* construct_dfa();
        NODE* construct_token(char* token, char* identifier);
};

