CC = g++
CFLAGS = -m64 -g -std=c++11 -pthread -c -Wall -Werror
LDFLAGS = 
OBJ = main.o lex.o language.o

main : $(OBJ)
	$(CC) $(LDFLAGS) $(OBJ) -o lexer

main.o : main.cpp lex.h language.h
	$(CC) $(CFLAGS) main.cpp -o main.o

lex.o : lex.cpp language.h
	$(CC) $(CFLAGS) lex.cpp -o lex.o

language.o : language.cpp
	$(CC) $(CFLAGS) language.cpp -o language.o

clean :
	rm -rf $(OBJ) lexer
