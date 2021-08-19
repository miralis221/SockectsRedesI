/*
** Fichero: funcionesServidorUDP.h
** Autores:
** Erick José Mercado Hernández DNI 43840208T
** Javier Merchán Benito DNI 70923644Q
*/

#ifndef FUNCIONESSERVIDORUDP_H_INCLUDED
#define FUNCIONESSERVIDORUDP_H_INCLUDED



/*funciones con las ordenes del servidor TCP*/
void listUDP (int sockfd,struct sockaddr_in cliaddr);
void newGroupsUDP (int sockfd,struct sockaddr_in cliaddr,char *buff);
void newNewsUDP (int sockfd,struct sockaddr_in cliaddr,char *buff);
void groupUDP (int sockfd,struct sockaddr_in cliaddr,char *buff);
void articleUDP(int sockfd,struct sockaddr_in cliaddr,char *buff);
void headUDP (int sockfd,struct sockaddr_in cliaddr,char *buff);
void bodyUDP (int sockfd,struct sockaddr_in cliaddr,char *buff);
void postUDP(int sockfd,struct sockaddr_in cliaddr,char *buff);
void quitUDP(int sockf,struct sockaddr_in cliaddr);
/********************************************/
int comprobarOrdenUDP(char *buff);

#endif // FUNCIONESCLIENTEUDP_H_INCLUDED
