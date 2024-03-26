#include "language.h"

DFA* LANGUAGE::construct_dfa() {

    // create start nodes for the token types dfas
    // for each token type in the spec file construct a coreponding dfa that recognizes it
    //
    char* buff_line = strtok(spec_buffer, "\n");   
    unsigned int buff_line_size = strlen(buff_line);

    if(buff_line_size != 2){
        printf("[-] Error: number of token types has not been provided\n");
        return NULL;
    } 

    DFA* dfa = (atoi(buff_line) != 0 ) ? (DFA*) malloc(sizeof(DFA) + atoi(buff_line) * sizeof(NODE*)) : NULL;
    if(!dfa) {                               
        printf("[-] Error allocating dfa\n");
        return NULL;                         
    }

    // create head node
    dfa->head.token.symbol = 0;
    dfa->head.next = (NODE**) dfa + 1;
    dfa->head.next_count = 0;

    buff_line = strtok(NULL, "\n");

    NODE* head = NULL;

    while(buff_line) {                                                                              
        buff_line_size = strlen(buff_line);

        // check if current line is either a token type, or a token value
        if(buff_line_size > TOKEN_MAGIC_SIZE && !strncmp(TOKEN_MAGIC, buff_line, TOKEN_MAGIC_SIZE)) {                        
            // line holds token type 
            //
            // create a head node for current dfa, and current token type
            unsigned int type_size = buff_line_size - TOKEN_MAGIC_SIZE + 1;

            head = (NODE*) malloc(sizeof(NODE) + type_size * sizeof(char));
            if(!head) {
                printf("[-] Error: token type head for dfa allocation failed\n");
                return NULL;
            }
            for(unsigned int i = 0; i < NUMBER_OF_SYMBOLS; i++) head->symbol_index[i] = -1;

            // get token type name
            head->token.type = (char*)(head + 1);

            strcpy(head->token.type, buff_line + TOKEN_MAGIC_SIZE + 1);
            head->token.type[type_size] = 0;

            head->maximum_count = atoi(strtok(NULL, "\n"));
            if(!head->maximum_count) {
                return NULL;
            }
            /*
            head->next = (NODE**) malloc(head->maximum_count * sizeof(NODE*));
            if(!head->next) {
                printf("[-] e de rau");
                return NULL;
            }
            */
            head->next = NULL;
            head->next_count = 0;

            // link with origin
            *(dfa->head.next + dfa->head.next_count) = head;
            dfa->head.next_count ++;

        }
        else if (buff_line_size > REGEX_MAGIC_SIZE && !strncmp(REGEX_MAGIC, buff_line, REGEX_MAGIC_SIZE)) {
            **(head->next + head->next_count) = construct_thompson(buff_line + REGEX_MAGIC_SIZE + 1);
            if(head->next_count + 1 <= head->next_count || ++head->next_count >= head->maximum_count) {
                return NULL;
            }
        }
        else {
       
           if(!head->build_transition(buff_line, 1, strlen(buff_line))){
               return NULL;
           }
           printf("%s\n", buff_line);
        }
        buff_line = strtok(NULL, "\n");
    }
    return dfa;
}
bool NODE::build_transition(char* value, unsigned int i, unsigned int size) {
    if(i == size) {
        return 1;
    }
    int index = (int)(strchr(symbols, *value) - symbols);

    if(index < 0 || index >= NUMBER_OF_SYMBOLS) return 0;

    if(this->symbol_index[index] == -1) {
        // create node
        this->next = (NODE**) realloc(this->next, (this->next_count + 1) * sizeof(NODE*));
        if(!this->next) {
            return 0;
        }
        *(this->next + this->next_count) = new NODE();
        (*this->next + this->next_count)->token.symbol = *value;
        (*this->next + this->next_count)->next = NULL;
        (*this->next + this->next_count)->next_count = 0;

        this->symbol_index[index] = this->next_count;
        return (i >= size) ? 1 : (*this->next + this->next_count)->build_transition(value + 1, i + 1, size);
    }
    return (i >= size) ? 1 : this->next[this->symbol_index[index]]->build_transition(value + 1, i + 1, size);
}
NODE LANGUAGE::construct_thompson(char* regex) {
    return NODE();
}

NODE LANGUAGE::construct_dfa(char* value) {
    return NODE();
}
