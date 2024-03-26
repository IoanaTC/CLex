#include "language.h"

DFA* LANGUAGE::construct_dfa() {

    // create start NODEs for the token types dfas
    // for each token type in the spec file construct a coreponding dfa that recognizes it
    //
    char* buff_line = strtok(spec_buffer, "\n");   
    unsigned int buff_line_size = strlen(buff_line);

    if(buff_line_size != 2){
        return NULL;
    } 
    
    DFA* dfa = (atoi(buff_line) != 0 ) ? (DFA*) malloc(sizeof(DFA) + atoi(buff_line) * sizeof(NODE*)) : NULL;
    if(!dfa) {                               
        return NULL;
    }

    // create head NODE
    dfa->head.token.symbol = 0;
    dfa->head.next = (NODE**) dfa + 1;
    dfa->head.next_count = 0;

    buff_line = strtok(NULL, "\n");

    NODE* head = NULL;

    while(buff_line) {
        buff_line_size = strlen(buff_line);

        // check if current line is either a token type, or a token value
        if(buff_line_size > TOKEN_MAGIC_SIZE && !strncmp(TOKEN_MAGIC, buff_line, TOKEN_MAGIC_SIZE)) {                          // line holds token type 
                                                                                                                               //
                                                                                                                               // create a head NODE for current dfa, and current token type
            unsigned int type_size = buff_line_size - TOKEN_MAGIC_SIZE + 1;

            head = (NODE*) malloc(sizeof(NODE) + type_size * sizeof(char));
            if(!head) {
                return NULL;
            }
            for(unsigned int i = 0; i < NUMBER_OF_SYMBOLS; i++) head->symbol_index[i] = -1;
            // get token type name
            head->token.type = (char*)(head + 1);

            strcpy(head->token.type, buff_line + TOKEN_MAGIC_SIZE + 1);
            head->token.type[type_size] = 0;
            head->next = NULL;
            head->next_count = 0;

            // link with origin
            *(dfa->head.next + dfa->head.next_count) = head;
            dfa->head.next_count ++;

        }
        else if (buff_line_size > REGEX_MAGIC_SIZE && !strncmp(REGEX_MAGIC, buff_line, REGEX_MAGIC_SIZE)) {
            buff_line += REGEX_MAGIC_SIZE + 1;

            unsigned int i = 0;
            while(i < sizeof(buff_line) && buff_line[i] != ':') i++;
            char* type = (char*) malloc (i * sizeof(char));
            strncpy(type, buff_line, i);

            NODE* node = new NODE();
            if(!node) return 0;

            head->next = (NODE**) realloc(node->next, (node->next_count + 1) * sizeof(NODE*));
            *(head->next + head->next_count) = node;
            head->next_count++;

            if(!node->build_thompson(buff_line, 0, strlen(buff_line), type)) {
                return NULL;
            }
        }
        else {

            if(!head->build_transition(buff_line, 1, strlen(buff_line))){
                return NULL;
            }
        }
        buff_line = strtok(NULL, "\n");
        cout<<"out "<<buff_line<<endl;        
    }
    return dfa;
}

bool NODE::build_transition(char* value, unsigned int i, unsigned int size) {

    if(i == size) {
        this->is_final_state = TRUE;
        return 1;
    }
    int index = (int)(strchr(symbols, *value) - symbols);

    if(index < 0 || index >= NUMBER_OF_SYMBOLS) return 0;

    if(this->symbol_index[index] == -1) {
        // create NODE
        this->next = (NODE**) realloc(this->next, (this->next_count + 1) * sizeof(NODE*));
        if(!this->next) {
            return 0;
        }
        *(this->next + this->next_count) = new NODE();
        (*this->next + this->next_count)->token.symbol = (char*) malloc(2 * sizeof(char));
        strncpy((*this->next + this->next_count)->token.symbol, value, 1);
        (*this->next + this->next_count)->next = NULL;
        (*this->next + this->next_count)->next_count = 0;

        this->symbol_index[index] = this->next_count;
        return (i > size) ? 1 : (*this->next + this->next_count)->build_transition(value + 1, i + 1, size);
    }
    return (i > size) ? 1 : this->next[this->symbol_index[index]]->build_transition(value + 1, i + 1, size);
}
bool NODE::build_thompson(char* regex, unsigned int i, unsigned int size, char* type) {

    stack<NODE*> stiva;
    NODE* from_stack_first = NULL;
    NODE* from_stack_last = NULL;

    NODE* final_node = NULL;
    NODE* initial_node = NULL;

    while(i < size && regex[i]) {
        switch(regex[i]){
            case '/':
                i++;
                if(i >= size) return 0;
                switch(regex[i]) {
                    case '-':
                        if(i + 3 >= size) return 0;
                        i++;

                        initial_node = new NODE();
                        if(!initial_node) return 0;

                        initial_node->token.symbol = (char*) malloc(3 * sizeof(char));
                        if(!initial_node) return 0;

                        strncpy(initial_node->token.symbol, regex + i, 2);
                        initial_node->token.symbol[2] = 0;

                        initial_node->next = (NODE**) malloc((initial_node->next_count + 1) * sizeof(NODE*));
                        if(!initial_node->next) return 0;

                        final_node = new NODE();
                        if(!final_node) return 0;

                        final_node->is_final_state = TRUE;
                        final_node->token.type = (char*) malloc(sizeof(type) * sizeof(char));
                        strcpy(final_node->token.type, type);

                        *(initial_node->next + initial_node->next_count) = final_node;
                        initial_node->next_count++;

                        i++;

                        stiva.push(initial_node);
                        break;

                    case '+': //union

                        if(stiva.empty()) return 0;
                        from_stack_first = stiva.top();
                        stiva.pop(); 
                        delete from_stack_first->next[0];

                        if(stiva.empty()) return 0;
                        from_stack_last = stiva.top();
                        stiva.pop();
                        delete from_stack_last->next[0];

                        initial_node = new NODE();
                        if(!initial_node) return 0;
                        initial_node->token.symbol = NULL;

                        initial_node->next = (NODE**) malloc(2 * sizeof(NODE*));
                        if(!initial_node->next) return 0;
                        initial_node->next_count = 2;

                        initial_node->next[0] = from_stack_last;
                        initial_node->next[1] = from_stack_first;

                        final_node = new NODE();
                        if(!final_node) return 0;
                        final_node->is_final_state = 0;

                        final_node->token.type = (char*) malloc(sizeof(type) * sizeof(char));
                        strcpy(final_node->token.type, type);

                        from_stack_first->next[0] = final_node;
                        from_stack_last->next[0] = final_node;

                        stiva.push(initial_node);
                        break;

                    case '?': //concat


                        if(stiva.empty()) return 0;
                        from_stack_first = stiva.top();
                        stiva.pop(); 

                        if(stiva.empty()) return 0;
                        from_stack_last = stiva.top();
                        stiva.pop();
                        delete from_stack_last->next[0];

                        from_stack_last->next[0] = from_stack_first;

                        stiva.push(from_stack_last);
                        break;

                    case '*': //kleene


                        if(stiva.empty()) return 0;
                        from_stack_first = stiva.top();
                        stiva.pop();

                        final_node = new NODE();
                        if(!final_node) return 0;

                        final_node->token.type = (char*) malloc(sizeof(type) * sizeof(char));
                        strcpy(final_node->token.type, type);

                        from_stack_first->next[0]->is_final_state = FALSE;
                        from_stack_first->next[0]->token.type = NULL;

                        from_stack_first->next[0]->next = (NODE**) realloc (from_stack_first->next[0]->next, (from_stack_first->next[0]->next_count + 1) * sizeof(NODE*));
                        from_stack_first->next[0]->next[from_stack_first->next[0]->next_count] = from_stack_first;
                        from_stack_first->next[0]->next[0] = final_node;
                        from_stack_first->next[0] = final_node;

                        stiva.push(from_stack_first);
                        break; 

                    default:

                        initial_node = new NODE();
                        if(!initial_node) return 0;

                        initial_node->token.symbol = (char*) malloc(3 * sizeof(char));
                        strncpy(initial_node->token.symbol, regex - 1, 2);

                        int index = (int)(strchr(symbols, *initial_node->token.symbol) - symbols);
                        initial_node->symbol_index[index] = this->next_count;

                        initial_node->next = (NODE**) malloc((initial_node->next_count + 1) * sizeof(NODE*));
                        if(!initial_node->next) return 0;

                        final_node = new NODE();
                        if(!final_node) return 0;

                        final_node->is_final_state = TRUE;
                        final_node->token.type = (char*) malloc(sizeof(type) * sizeof(char));
                        strcpy(final_node->token.type, type);

                        *(initial_node->next + initial_node->next_count) = final_node;
                        initial_node->next_count++;

                        stiva.push(initial_node);
                        break;
                }
                i++;
                break;

            default:

                initial_node = new NODE();
                if(!initial_node) return 0;

                initial_node->token.symbol = (char*) malloc(2 * sizeof(char));
                strncpy(initial_node->token.symbol, regex, 1);
                initial_node->next = (NODE**) malloc((initial_node->next_count + 1) * sizeof(NODE*));
                if(!initial_node->next) return 0;

                final_node = new NODE();
                if(!final_node) return 0;

                final_node->is_final_state = TRUE;
                final_node->token.type = (char*) malloc(sizeof(type) * sizeof(char));
                strcpy(final_node->token.type, type);

                *(initial_node->next + initial_node->next_count) = final_node;
                initial_node->next_count++;

                stiva.push(initial_node);
                i++;

                break;
        }
    }

    this->next = (NODE**) malloc(sizeof(NODE*));

    if(!this->next) return 0;
    this->next_count++;

    if(stiva.empty()) return 0;
    from_stack_first = stiva.top();
    stiva.pop();
    this->next[0] = from_stack_first; 
    return 1;
}

