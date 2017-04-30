all: protocolo cliente servidor clear

protocolo: protocolo.h
	gcc -Wall -pedantic -c protocolo.c

cliente: protocolo.o cliente.c
	gcc -Wall -pedantic protocolo.o cliente.c -o cliente

servidor: protocolo.o servidor.c
	gcc -Wall -pedantic protocolo.o servidor.c -o servidor

clear: protocolo.o
	rm -rf *.o
