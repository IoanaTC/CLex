#include "language.h"
static const char symbols[] = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ1234567890!@#$%^&-=_+`~/,/<>?;':\"[]{}.";

DFA* LANGUAGE::construct_dfa() {

    // construct dfa from all the nfas coresponding to regular expresisons of given types
    DFA* dfa = new DFA();
    if(!dfa) {
        printf("[-] Error allocating dfa\n");
        return NULL;
    }
    
    // create the start node, that connects the dfas recognizing all the token types
    dfa->start = NODE();
    NODE* start = &dfa->start;
    // initializing starting node
    start->regex = start->token = start->id = 0;
    start->transitions_count = 0;
    start->is_final = FALSE;
    start->transitions = NULL;
    unsigned int* transitions_count = &dfa->start.transitions_count; 

    // create an initial node for each token type
    NODE* id_start = NULL;

    // for each token type in the spec file construct a coreponding dfa that recognize it

    char* buff_line = strtok(spec_buffer, "\n");
    unsigned int buff_line_size = 0;
    
    while(buff_line) {
        buff_line_size = sizeof(buff_line);
        // check if current line is either a token name, or a token value
        if(buff_line_size > TOKEN_MAGIC_SIZE && !strncmp(TOKEN_MAGIC, buff_line, TOKEN_MAGIC_SIZE)) {
            // keyword found
            if(*transitions_count) (*transitions_count)++;

            start->transitions[*transitions_count] = new NODE();
            id_start = start->transitions[*transitions_count];

            if(!id_start) {
                printf("[-] Error: allocating new token type initial node\n");
                delete dfa;
                return NULL;
            }
            id_start->regex = id_start->token = 0;

            for(unsigned int i = 0; i < sizeof())
            id_start->transitions = NULL;
            id_start->transitions_count = 0;
            id_start->is_final = FALSE;
            id_start->id = (char*) malloc((buff_line_size - TOKEN_MAGIC_SIZE) * sizeof(char));

            if(!id_start->id) {
                printf("[-] Error alloc token identifier\n");
                delete id_start;
                delete dfa;
                return NULL;
            }
            strcpy(id_start->id, buff_line + TOKEN_MAGIC_SIZE);
        }
        else {
            // token value found
            // check if current value is regular expresion
            if(buff_line_size > REGEX_MAGIC_SIZE && !strncmp(REGEX_MAGIC, buff_line, REGEX_MAGIC_SIZE)) {
            /*
                // regex
                TOKEN token;
                token.regex = (char*) malloc(buff_line_size - REGEX_MAGIC_SIZE);

                if(!token.regex) {
                    printf("[-] Error alloc token regex\n");

                    free(current_identifier);
                    current_identifier = NULL;
                    delete dfa;
                    return NULL;
                }
                token.identifier = current_identifier;
                //token.dfa = CreateDFA(); //TODO
                */
            }
            else {
                // value
                unsigned int token_count = start->transitions[*transitions_count]->transitions_count;
                NODE* token_list = start->transitions[*transitions_count];

                token_family->transitions[token_family->transitions_count++] = construct_token(buff_line, token_family->id);
            }
        }
        buff_line = strtok(NULL, "\n");
    }
    return dfa;
}
NODE* LANGUAGE::construct_token(char* token, char* identifier) {
    NODE* start = new NODE();
    if(!start) {
        return NULL;
    }
    start->regex = 0;
    start->transitions_count = 0;


    return start;
}
