/*
** Fichero: funcionesCleinteTCP.h
** Autores:
** Erick José Mercado Hernández DNI 43840208T
** Javier Merchán Benito DNI 70923644Q
*/

#ifndef FUNCIONESCLIENTETCP_H_INCLUDED
#define FUNCIONESCLIENTETCP_H_INCLUDED
struct sockaddr_in;
struct sembuf;
int list(int sockfd, struct sockaddr_in cliente, struct sembuf *semact);
void newsGroups(int sockfd, struct sockaddr_in cliente, struct sembuf *semact);
void newNew (int sockfd, struct sockaddr_in cliente, struct sembuf *semact);
void group (int sockfd, char *ordenes, struct sockaddr_in cliente, struct sembuf *semact);
void article (int sockfd, char * rutaDelArticulo, struct sockaddr_in cliente, struct sembuf *semact);
void head (int sockfd, char * rutaDelArticulo, struct sockaddr_in cliente, struct sembuf *semact);
void body (int sockfd, char * rutaDelArticulo, struct sockaddr_in cliente, struct sembuf *semact);
void post(int sockfd);
int comprobarOrden(char *buff);
void quit(int sockfd);
void errorComando(int sockfd);

#endif // FUNCIONESCLIENTETCP_H_INCLUDED
