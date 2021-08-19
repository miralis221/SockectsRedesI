/*
** Fichero: funcionesEscritura.c
** Autores:
** Erick José Mercado Hernández DNI 43840208T
** Javier Merchán Benito DNI 70923644Q
*/
#include "funcionesEscritura.h"
#include <stdio.h> 
#include <netdb.h> 
#include <netinet/in.h> 
#include <stdlib.h> 
#include <string.h> 
#include <sys/socket.h> 
#include <sys/types.h> 
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <time.h>
#include <dirent.h>
#include <pwd.h>
#include <wait.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/errno.h>


#define FICHEROLOG "nntp.log"
#define semaforo 

int escrituraNNTP(char *buff,struct sembuf *semact)
{

	FILE *fp;

	*semact = sem_wait(1);
	fp = fopen(FICHEROLOG,"a");
	if (fp == NULL)
	{
		printf("no existe %s ",FICHEROLOG);
		return 1;
	}
	fprintf(fp, "%s\n", buff);
	fclose(fp);
	*semact = sem_signal(1);

}

int escrituraCliente(char *buff,struct sembuf *semact, struct sockaddr_in cliente)
{
	FILE *fp;
	char nombreCliente[100];
	sprintf(nombreCliente,"cliente_%u.txt",ntohs(cliente.sin_port));
	*semact = sem_wait(1);
	fp = fopen(nombreCliente,"a");
	if (fp == NULL)
	{
		printf("no existe %s ",nombreCliente);
		return 1;
	}
	fprintf(fp, "%s\n", buff);
	fclose(fp);
	*semact = sem_signal(1);
}

struct sembuf sem_wait(int n)
{
	struct sembuf semact;

	semact.sem_num=n;	
	semact.sem_op=-1;
	semact.sem_flg=0;
	return semact;
}
struct sembuf sem_signal(int n)
{
	struct sembuf semact;

	semact.sem_num=n;
	semact.sem_op =1;
	semact.sem_flg=0;

	return semact;
}



