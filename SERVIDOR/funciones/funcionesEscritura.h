/*
** Fichero: funcionesEscritura.h
** Autores:
** Erick José Mercado Hernández DNI 43840208T
** Javier Merchán Benito DNI 70923644Q
*/
#ifndef FUNCIONESESCRITURA_H_INCLUDED
#define FUNCIONESESCRITURA_H_INCLUDED

struct sockaddr_in;

struct sembuf sem_wait(int n);
struct sembuf sem_signal(int n);

int escrituraNNTP(char *buff, struct sembuf *semact);
int escrituraCliente(char *buff,struct sembuf *semact,struct sockaddr_in cliente);


#endif
