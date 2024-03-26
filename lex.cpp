#include "lex.h"

bool LEXER::compute_flow(HANDLE hOutputFile) {

    unsigned int line = 0;
    unsigned int index_left = 0, index_right = 0;

    NODE* head = &dfa->head;
    NODE* current = NULL;

    char* type = NULL;
    char* token = NULL;
    unsigned int token_size = 0;

    char* buff_line = strtok(buffer, "\n");
    buffer_size = strlen(buff_line);
    while(buff_line){

        while(index_left < buffer_size) {

            if(this->run_types(head, type, index_right)) {

                token_size = index_right - index_left + 1;
                token = (char*) malloc(token_size * sizeof(char));
                strncpy(token, buffer + index_left, token_size);
                token[token_size] = 0;
                
                char output[256];
                sprintf(output, "category: %s, type: %s, token: %s, line: %d\n", current->token.type, type, token, line);
                DWORD bytesWritten = 0;
                if (!WriteFile(hOutputFile, output, strlen(output), &bytesWritten, NULL)) {
                    return 0;
                }

                cout<<"category: "<< current->token.type <<" type: "<< type <<" token: "<< token <<" line: "<<line<<endl;
                index_left = index_right;

                free(token);
                token = NULL;

                break;
            }
            else return 0;
        }
        buff_line = strtok(NULL, "\n");
        buffer_size = strlen(buff_line);
        line++;
    }
    return 1;
}
bool LEXER::run_types(NODE* head, char* type, unsigned int& index) {

    if(head->is_final_state) {
        type = (char*) malloc(strlen(type) * sizeof(char));
        strcpy(type, head->token.type);
        return 1;
    }

    for(unsigned int type_index = 0; type_index < head->next_count; type_index ++){
        if(index >= buffer_size) return 1;

        if(isspace(buffer[index])) return run_types(head, type, ++index);

        //symbol
        int symbol = (int)(strchr(symbols, *(buffer + index)) - symbols);
        unsigned int next_symbol_index = head->next[type_index]->symbol_index[symbol];
        if(next_symbol_index) {
            return run_types(head->next[type_index]->next[next_symbol_index], type, ++index);
        }

        //regex
        for(unsigned int regex = 0; regex < head->next[type_index]->next_count; regex ++){
            char* token = head->next[type_index]->next[regex]->token.symbol;
            if(strlen(token) == 2) {
                if(token[0] == '/'){
                    return run_types(head->next[type_index]->next[regex], type, ++index);
                }
                if(buffer[index] >= token[0] && buffer[index] <= token[1]) {
                    return run_types(head->next[type_index]->next[regex], type, ++index);
                }
            }
        }
    }
    return 0;
    //cout <<1;
}
