#include<iostream>
#include<windows.h>
#include "lex.h" 
#include "language.h"

using namespace std;

struct ThreadParams {

    DFA* dfa;
    char * input_file; 
};

char* ReadFileWrapper(char* filename) {
    bool status = FALSE;
    //open file
    HANDLE hFile = CreateFileA(filename, GENERIC_READ, /* dwShareMode = */ 0, /* lpSecurityAttributes = */ NULL, 
            OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, /* hTemplateFile = */ NULL);
    if(hFile == INVALID_HANDLE_VALUE) {
        // hot spot thread
        cout << "Failed to open file " << endl;
        return NULL;
    }

    // get file-size to allocate buffer
    DWORD fileSize = GetFileSize(hFile, /* lpFileSizeHigh = */ NULL);
    status = FALSE;

    if(fileSize == INVALID_FILE_SIZE) {
        cout << "Failed to get file size " << endl;

        status = CloseHandle(hFile);
        if(!status) {
            DWORD errorCode = GetLastError();
            cout << "Failed to close handle for thread " << ", Error code: " << errorCode << endl;
            return NULL;
        }
    }

    char* buffer = new char[fileSize + 1];
    if(!buffer) {
        cout << "Failed to allocate buffer for file " << endl;

        status = CloseHandle(hFile);
        if(!status) {
            DWORD errorCode = GetLastError();
            cout << "Failed to close handle for thread " << ", Error code: " << errorCode << endl;
            return NULL;
        }
    }

    DWORD bytesRead = 0;
    if(!ReadFile(hFile, buffer, fileSize, &bytesRead, NULL) || bytesRead != fileSize) {
        cout << "Failed to read file " << filename << endl;
        delete[] buffer;

        status = CloseHandle(hFile);
        if(!status) {
            DWORD errorCode = GetLastError();
            cout << "Failed to close handle for thread " << ", Error code: " << errorCode << endl;
            return NULL;
        }
    }
    buffer[bytesRead] = 0;
    return buffer;
}

void compute_file(ThreadParams p) {
    char* file = p.input_file;
    bool status = FALSE;

    char* buffer = ReadFileWrapper(file);
    if(!buffer) {
        cout << "nasol";
    }
    // create output file for current input
    char* output = new char[strlen(file) + 5]; // TODO: limit for name
    if(!output) {

        cout << "Failed to create output file name " << endl;
        delete[] buffer;
    }
    strcpy(output, file);
    strcat(output, ".out\0");

    HANDLE hOutputFile = CreateFile(output, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
    if(hOutputFile == INVALID_HANDLE_VALUE) {

        cout << "Failed to create output file name " << endl;
        delete[] buffer;
        delete[] output;
    }
    // debugging purposes
    // cout<< buffer; 

    // compute lexical parsing
    LEXER lex(buffer, hOutputFile, p.dfa);
    if(!lex.compute_flow(hOutputFile)) {
        cout << "Failed to compute lexer flow for " << file << endl;
    } 
    // delete used objects and free allocated memory
    delete[] buffer;
    delete[] output;

    status = CloseHandle(hOutputFile);
    if(!status) {
        DWORD errorCode = GetLastError();
        cout << "Failed to close handle for thread " << ", Error code: " << errorCode << endl;
    }
    return;
}
int main(int argc, char** argv) {

    // parse the input given by the user
    if(argc == 1) {
        //display application help menu
    }
    else if(argc <= 10) {
        // get rules from the language description file

        // TODO: add checks for the spec file to be correct
        char* spec_buffer = ReadFileWrapper(argv[1]);
        if(!spec_buffer) {
            cout << "nasol spec";
            return EXIT_FAILURE;
        }
        
        // create dfa based on the received rules
        LANGUAGE description(spec_buffer, sizeof(spec_buffer));
        DFA* dfa = description.construct_dfa();
        if(!dfa) {
            cout << "nasol status";
            return EXIT_FAILURE;
        }
        return 0;

        // get user input file
        unsigned int threads_count = argc - 2;
        HANDLE threads[threads_count];

        for(unsigned int i = 2; i < (unsigned int) argc; i++) {
            ThreadParams* p = NULL;
            p->dfa = dfa;
            p->input_file = argv[i];
            
            threads[i - 2] = CreateThread(/* lpThreadAttributes = */ NULL, /* dwStackSize = */ 0, 
                    (LPTHREAD_START_ROUTINE)compute_file, p, 
                    /* dwCreationFlags = */ 0, /* lpThreadId = */ NULL);
            if(threads[i-2] == NULL) {
                cout << "Failed to create thread for file " << argv[i] << endl;
                return EXIT_FAILURE;
            }
        }
        WaitForMultipleObjects(threads_count, threads, /*bWaitAll =*/ TRUE, /* dwMilliseconds = */ 0xFFFFFFFF);
        for(unsigned int i = 0; i < threads_count; i++) {
            bool status = CloseHandle(threads[i]);
            if(!status) {
                DWORD errorCode = GetLastError();
                cout << "Failed to close handle for thread " << i << ", Error code: " << errorCode << endl;
            }
        }
    }
    else {
        // incorrect input
    }
    return EXIT_SUCCESS;
}
