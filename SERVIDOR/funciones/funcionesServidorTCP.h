/*
** Fichero: funcionesServidorTCP.h
** Autores:
** Erick José Mercado Hernández DNI 43840208T
** Javier Merchán Benito DNI 70923644Q
*/

#ifndef FUNCIONESSERVIDORTCP_H_INCLUDED
#define FUNCIONESSERVIDORTCP_H_INCLUDED

/*funciones con las ordenes del servidor TCP*/


void list (int sockfd, char *recuperado);
void newsGroups (int sockfd,char *buff,char *recuperado);
void newNew (int sockfd,char *buff,char *recuperado);
void group (int sockfd,char *buff,char *recuperado);
void article (int sockfd,char *buff,char *recuperado);
void head (int sockfd,char *buff,char *recuperado);
void body (int sockfd,char *buff,char *recuperado);
void post(int sockfd,char *buff,char *recuperado);
void quit(int sockfd,char *recuperado);

int comprobarOrden(char *buff);

/********************************************/
#endif // FUNCIONESCLIENTETCP_H_INCLUDED
