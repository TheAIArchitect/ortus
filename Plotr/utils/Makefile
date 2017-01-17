.PHONY: lib exec

CC=g++

exec:
	$(CC) main.cpp -lpython -o main

lib:
	$(CC) -c Interpreter.cpp -o lib/Interpreter.o 
	$(CC) -c Plotr.cpp -lpython -o lib/Plotr.o
	ar crsv lib/libplotr.a lib/Plotr.o lib/Interpreter.o
	

