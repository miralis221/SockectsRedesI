#
## Fichero: makefile
## Autores:
## Erick José Mercado Hernández DNI 43840208T
## Javier Merchán Benito DNI 70923644Q
#
# SERVIDOR Y CLIENTES

CC = gcc
OBJ0 = servidor.o
OBJ1 = cliente.o
LIBS =
BIN = servidorTCP
CFLAGS = -g

servidor: servidor.o funciones/funcionesServidorTCP.o funciones/funcionesEscritura.o funciones/funcionesServidorUDP.o
	${CC} servidor.o funciones/funcionesServidorTCP.o funciones/funcionesEscritura.o funciones/funcionesServidorUDP.o -o servidor
	
cliente: cliente.o funciones/funcionesClienteTCP.o
	${CC} cliente.o funciones/funcionesClienteTCP.o -o cliente

servidor.o: servidor.c funciones/funcionesServidorTCP.h
	${CC} $(CFLAGS) -c servidor.c -o servidor.o  

cliente.o: cliente.c funciones/funcionesClienteTCP.h
	${CC} $(CFLAGS) -c cliente.c -o cliente.o

funciones/funcionesClienteTCP.o: funciones/funcionesClienteTCP.c funciones/funcionesClienteTCP.h
	${CC} $(CFLAGS) -c funciones/funcionesClienteTCP.c -o funciones/funcionesClienteTCP.o

funciones/funcionesServidorTCP.o: funciones/funcionesServidorTCP.c funciones/funcionesServidorTCP.h	
	${CC} $(CFLAGS) -c funciones/funcionesServidorTCP.c -o funciones/funcionesServidorTCP.o
funciones/funcionesEscritura.o: funciones/funcionesEscritura.c funciones/funcionesEscritura.h	
	${CC} $(CFLAGS) -c funciones/funcionesEscritura.c -o funciones/funcionesEscritura.o	
funciones/funcionesServidorUDP.o: funciones/funcionesServidorUDP.c funciones/funcionesServidorUDP.h	
	${CC} $(CFLAGS) -c funciones/funcionesServidorUDP.c -o funciones/funcionesServidorUDP.o
limpiar:
	rm *.o
