#pragma once

#include<iostream>
#include<windows.h>
#include<algorithm>
#include<stack>

using namespace std;

#define TOKEN_MAGIC "KEY"
#define TOKEN_MAGIC_SIZE 3

#define REGEX_MAGIC "REGEX"
#define REGEX_MAGIC_SIZE 5

#define NUMBER_OF_SYMBOLS 93

typedef class NODE {
    public:
        union {
            char* symbol;
            char* type;
        } token;
        int symbol_index[NUMBER_OF_SYMBOLS] = { -1 };

        NODE** next = NULL;
        unsigned short next_count = 0;

        bool is_final_state = FALSE;

        bool build_transition(char* value, unsigned int i, unsigned int size);
        bool build_thompson(char* regex, unsigned int i, unsigned int size, char* type);
        NODE() {
            for(unsigned int i = 0; i < NUMBER_OF_SYMBOLS; i++) symbol_index[i] = -1;
        }
} NODE;

typedef struct DFA {
    NODE head;
} DFA;


static const char symbols[] = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ1234567890()!@#$%^&|*-=_+`~,/<>?;':\"[]{}.";

class LANGUAGE {
    private:
        char* spec_buffer;
        unsigned int spec_size;

    public:
        LANGUAGE(char* buffer = NULL, unsigned int size = 0) { spec_buffer = buffer; spec_size = size; }

        DFA* construct_dfa();
        NODE construct_thompson(char* regex);
        NODE construct_dfa(char* value);
};

