/*
** Fichero: servidor.c
** Autores:
** Erick José Mercado Hernández DNI 43840208T
** Javier Merchán Benito DNI 70923644Q
*/

#include <sys/types.h>
#include <sys/socket.h>
#include <sys/errno.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <netdb.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <sys/sem.h>
#include <fcntl.h>
#include "funciones/funcionesServidorTCP.h"
#include "funciones/funcionesEscritura.h"
#include "funciones/funcionesServidorUDP.h"

#define MAX 256 
#define PORT 3644 
#define SA struct sockaddr 
#define FICHEROLOG "nntp.log"
#define MAXHOST 128 
#define n_semaforos 10
#define BUFFERSIZE  1024
#define MAXLINE 1024 

void ordenesServidorUDP (int sockfd, struct sockaddr_in, struct sembuf *semact);
void ordenesServidor (int sockfd, struct sockaddr_in,struct sembuf *semact);
int FIN = 0;             /* Para el cierre ordenado */
void finalizar(){ FIN = 1; }

long int semid;

int main(argc,argv) 
int argc;
char *argv[];
{  
	/*Variables de servidor*/
    int ls_TCP, s_TCP, len; 
    int s_UDP;
    struct sockaddr_in servaddr;
    struct sockaddr_in cliente; 
    pid_t pid;
  	int fp = creat("nntp.log",0700);
    char hostname[MAX];
    char buffer[BUFFERSIZE];
    long timevar;
    struct sigaction sa = {.sa_handler = SIG_IGN};
    fd_set readmask;
    int numfds,s_mayor;
    struct sigaction vec;
    int cc;
    struct sembuf semact;
    key_t clave = ftok ("/bin/ls", '3');


    if((semid=semget(clave,n_semaforos,IPC_CREAT|0777))==-1)
    {
        perror("No se ha podido crear el array de semaforos");
        exit(1);
    }
    if(semctl(semid,1,SETVAL,1)==-1)
    {
        fprintf(stderr,"No se ha podido iniciar sem 1\n");
        exit(2);        
    }


    // socket create and verification 
    ls_TCP = socket(AF_INET, SOCK_STREAM, 0); 
    if (ls_TCP == -1) { 
        printf("socket creation failed...\n"); 
        exit(0); 
    } 
    else
        //printf("Socket successfully created..\n"); 
    bzero(&servaddr, sizeof(servaddr)); 
    
    memset ((char *)&servaddr, 0, sizeof(struct sockaddr_in));
    memset ((char *)&cliente, 0, sizeof(struct sockaddr_in));

    len = sizeof(struct sockaddr_in);

    
    // assign IP, PORT 
    servaddr.sin_family = AF_INET; 
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY); 
    servaddr.sin_port = htons(PORT); 
  
    // Binding newly created socket to given IP and verification 
    if ((bind(ls_TCP, (SA*)&servaddr, sizeof(servaddr))) != 0) { 
        printf("socket bind failed...\n"); 
        exit(0); 
    } 
    else
        //printf("Socket successfully binded..\n"); 

    // Now server is ready to listen and verification 
    if ((listen(ls_TCP, 5)) != 0) { 
        printf("Listen failed...\n"); 
        exit(0); 
    } 
    else
    //len = sizeof(cliente); 
    s_UDP = socket (AF_INET, SOCK_DGRAM, 0);
    if (s_UDP == -1) 
    {
        perror(argv[0]);
        printf("%s: unable to create socket UDP\n", argv[0]);
        exit(1);
       }
    // Bind the server's address to the socket. 
    if (bind(s_UDP, (struct sockaddr *) &servaddr, sizeof(struct sockaddr_in)) == -1) 
    {
        perror(argv[0]);
        printf("%s: unable to bind address UDP\n", argv[0]);
        exit(1);
    }
    
    setpgrp();
    switch(fork())
    {
        case -1:
            perror(argv[0]);
            fprintf(stderr, "%s: no se puede crear el daemon\n",argv[0]);
            exit(1);

        case 0:

        fclose(stdin);
        fclose(stderr);

        if (sigaction(SIGCHLD,&sa,NULL)== -1)
        {
            perror(" sigaction(SIGCHLD)");
            fprintf(stderr,"%s: unable to register the SIGCHLD signal\n",
             argv[0]);
            exit(1);
        }

        vec.sa_handler = (void *) finalizar;
        vec.sa_flags = 0;
        if ( sigaction(SIGTERM, &vec, (struct sigaction *) 0) == -1) 
        {
            perror(" sigaction(SIGTERM)");
            fprintf(stderr,"%s: unable to register the SIGTERM signal\n", argv[0]);
            exit(1);
        }
        while(!FIN)
        {
            FD_ZERO(&readmask);
            FD_SET(ls_TCP, &readmask);
            FD_SET(s_UDP, &readmask);

            if (ls_TCP > s_UDP) s_mayor=ls_TCP;
            else s_mayor=s_UDP;

            if ( (numfds = select(s_mayor+1, &readmask, (fd_set *)0, (fd_set *)0, NULL)) < 0) 
            {
                if (errno == EINTR) 
                {
                    FIN=1;
                    close (ls_TCP);
                    //close (s_UDP);
                    perror("\nFinalizando el servidor. SeÃal recibida en elect\n "); 
                }
            }
           else 
           { 

                /* Comprobamos si el socket seleccionado es el socket TCP */
                if (FD_ISSET(ls_TCP, &readmask)) 
                {
                    s_TCP = accept(ls_TCP, (SA*)&cliente, &len); 
                    if (s_TCP == -1) {exit(0);}
                    switch(fork())
                    {
                        case -1:
                            exit(1);
                        case 0:
                            close(ls_TCP);
                            ordenesServidor(s_TCP,cliente,&semact);
                            exit(0);
                        default:
                        close(s_TCP);
                    }
                }
                if (FD_ISSET(s_UDP, &readmask)) 
                {
                    ordenesServidorUDP (s_UDP, cliente, &semact);
                }
            }

        }
        close(ls_TCP);
        close(s_UDP);
    
        printf("\nFin de programa servidor!\n");
    default:
        exit(0);
    }
} 

/*
*FUNCIONES SERVIDOR TCP
*
*
*
*/

void ordenesServidor(int sockfd, struct sockaddr_in cliente,
 struct sembuf *semact)
{
    char buff[MAX];
    long timevar;
    time (&timevar);
    int len, len1, status;
    struct hostent *hp;
    char hostname[MAXHOST];
    struct linger linger;
    char mensajeErrorComando[] = "500 comando desconocido\n";
    char clienteConectado[1024];
    char ordenYCliente[1024];
    char *token;
    char recuperadoLi[1024];
    char recuperadoNg[1024];
    char recuperadoGr[1024];
    char recuperadoAr[1024];
    char recuperadoHe[1024];
    char recuperadoBo[1024];
    char recuperadoPo[1024];
    char recuperadoQu[1024];

    time_t rawtime = time(NULL);
    struct tm *ptm = localtime(&rawtime);

    status = getnameinfo((struct sockaddr *)&cliente,
        sizeof(cliente),
        hostname,MAXHOST,NULL,0,0);
     if(status){
            /* The information is unavailable for the remote
             * host.  Just format its internet address to be
             * printed out in the logging information.  The
             * address will be shown in "internet dot format".
             */
             /* inet_ntop para interoperatividad con IPv6 */
            if (inet_ntop(AF_INET, &(cliente.sin_addr),
             hostname, MAXHOST) == NULL)
                perror(" inet_ntop \n");
             }
    sprintf(clienteConectado,"Startup from %s port %u  client_IP %u at %s",
        hostname,
         ntohs(cliente.sin_port),
         ntohs(cliente.sin_addr.s_addr),
          (char *) ctime(&timevar));
    escrituraNNTP(clienteConectado,semact);


    for (;;)
    {
        bzero(buff, MAX);
        recv(sockfd, buff, MAX,0);
        switch(comprobarOrden(buff))
        {
            case 0:
            printf("list\n");
            token = strtok(buff,"\n");
            sprintf(ordenYCliente,"%02d%02d%02d %02d%02d%02d TCPClient: %s, %s, %u ",ptm->tm_year-100
                ,ptm->tm_mon+1
                ,ptm->tm_mday
                ,ptm->tm_hour
                ,ptm->tm_min
                ,ptm->tm_sec
                ,token
                ,hostname
                ,ntohs(cliente.sin_port));
            escrituraNNTP(ordenYCliente,semact);
            list(sockfd,recuperadoLi);
            escrituraNNTP(recuperadoLi,semact);
            bzero(recuperadoLi,256);
            bzero(buff, MAX);
            break;

            case 1:
            printf("newsGroups\n");
            //token = strtok(buff," ");
            sprintf(ordenYCliente,"%02d%02d%02d %02d%02d%02d TCPClient: %s, %s, %u ",ptm->tm_year-100
                ,ptm->tm_mon+1
                ,ptm->tm_mday
                ,ptm->tm_hour
                ,ptm->tm_min
                ,ptm->tm_sec
                ,buff
                ,hostname
                ,ntohs(cliente.sin_port));
            escrituraNNTP(ordenYCliente,semact);
            newsGroups(sockfd,buff,recuperadoNg);
            bzero(buff, MAX);
            escrituraNNTP(recuperadoNg,semact);
            bzero(recuperadoNg,256);
            break;

            case 2:
            printf("newNew\n");
            //token = strtok(buff," ");
            sprintf(ordenYCliente,"%02d%02d%02d %02d%02d%02d TCPClient: %s, %s, %u ",ptm->tm_year-100
                ,ptm->tm_mon+1
                ,ptm->tm_mday
                ,ptm->tm_hour
                ,ptm->tm_min
                ,ptm->tm_sec
                ,token
                ,hostname
                ,ntohs(cliente.sin_port));
            escrituraNNTP(ordenYCliente,semact);
            newNew(sockfd,buff,recuperadoHe);
            bzero(buff,MAX);
            escrituraNNTP(recuperadoHe,semact);
            bzero(recuperadoHe,256);
            break;
            case 3:
            printf("group\n");
            sprintf(ordenYCliente,"%02d%02d%02d %02d%02d%02d TCPClient:%s, %s, %u ",ptm->tm_year-100
                ,ptm->tm_mon+1
                ,ptm->tm_mday
                ,ptm->tm_hour
                ,ptm->tm_min
                ,ptm->tm_sec
                ,buff
                ,hostname
                ,ntohs(cliente.sin_port));
            escrituraNNTP(ordenYCliente,semact);
            group(sockfd,buff,recuperadoGr);
            bzero(buff, MAX);
            escrituraNNTP(recuperadoGr,semact);
            bzero(recuperadoGr,256);
            break;

            case 4:
            printf("article\n");
            //token = strtok(buff," ");
            sprintf(ordenYCliente,"%02d%02d%02d %02d%02d%02d TCPClient:%s, %s, %u ",ptm->tm_year-100
                ,ptm->tm_mon+1
                ,ptm->tm_mday
                ,ptm->tm_hour
                ,ptm->tm_min
                ,ptm->tm_sec
                ,buff
                ,hostname
                ,ntohs(cliente.sin_port));
            escrituraNNTP(ordenYCliente,semact);
            article(sockfd,buff,recuperadoAr);
            bzero(buff, MAX);
            escrituraNNTP(recuperadoAr,semact);
            bzero(recuperadoAr,256);
            break;
            
            case 5:
            printf("head\n");
            token = strtok(buff," ");
            sprintf(ordenYCliente,"%02d%02d%02d %02d%02d%02d TCPClient:%s, %s, %u ",ptm->tm_year-100
                ,ptm->tm_mon+1
                ,ptm->tm_mday
                ,ptm->tm_hour
                ,ptm->tm_min
                ,ptm->tm_sec
                ,buff
                ,hostname
                ,ntohs(cliente.sin_port));
            escrituraNNTP(ordenYCliente,semact);
            head(sockfd,buff,recuperadoHe);
            bzero(buff, MAX);

            escrituraNNTP(recuperadoHe,semact);
            bzero(recuperadoHe,256);
            break;

            case 6:
            printf("body\n");
            token = strtok(buff," ");
            sprintf(ordenYCliente,"%02d%02d%02d %02d%02d%02d TCPClient:%s, %s, %u ",ptm->tm_year-100
                ,ptm->tm_mon+1
                ,ptm->tm_mday
                ,ptm->tm_hour
                ,ptm->tm_min
                ,ptm->tm_sec
                ,buff
                ,hostname
                ,ntohs(cliente.sin_port));
            body(sockfd,buff,recuperadoBo);
            bzero(buff, MAX);
            escrituraNNTP(recuperadoBo,semact);
            bzero(recuperadoBo,256);
            break;

            case 7:
            printf("post\n");
            token = strtok(buff," ");
            sprintf(ordenYCliente,"%02d%02d%02d %02d%02d%02d TCPClient: %s, %s, %u ",ptm->tm_year-100
                ,ptm->tm_mon+1
                ,ptm->tm_mday
                ,ptm->tm_hour
                ,ptm->tm_min
                ,ptm->tm_sec
                ,buff
                ,hostname
                ,ntohs(cliente.sin_port));
            escrituraNNTP(ordenYCliente,semact);
            post(sockfd,buff,recuperadoPo);
            bzero(buff, MAX);
            escrituraNNTP(recuperadoPo,semact);
            bzero(recuperadoPo,256);
            break;

            case 8:
            printf("quit\n");
            token = strtok(buff," ");
            sprintf(ordenYCliente,"%02d%02d%02d %02d%02d%02d TCPClient: %s, %s, %u ",ptm->tm_year-100
                ,ptm->tm_mon+1
                ,ptm->tm_mday
                ,ptm->tm_hour
                ,ptm->tm_min
                ,ptm->tm_sec
                ,buff
                ,hostname
                ,ntohs(cliente.sin_port));
            escrituraNNTP(ordenYCliente,semact);
            quit(sockfd,recuperadoQu);
            escrituraNNTP(recuperadoQu,semact);
            exit(1);
            break;

            default:
            printf("%s\n",buff );
            sprintf(ordenYCliente,"%02d%02d%02d %02d%02d%02d TCPClient: %s, %s, %u ,%s",ptm->tm_year-100
                ,ptm->tm_mon+1
                ,ptm->tm_mday
                ,ptm->tm_hour
                ,ptm->tm_min
                ,ptm->tm_sec
                ,buff
                ,hostname
                ,ntohs(cliente.sin_port)
                ,mensajeErrorComando);
            escrituraNNTP(ordenYCliente,semact);
            send(sockfd,mensajeErrorComando,strlen(mensajeErrorComando),0);
            //exit(0);
        }
    }
}

/*
* FUNCIONES DE UDP
*
*
*
*/


void ordenesServidorUDP(int sockfd, struct sockaddr_in cliaddr, struct sembuf *semact)
{
    char buff[MAXLINE];
    long timevar;
    time (&timevar);
    //printf("Startup from local host port %u at %s",
    //ntohs(cliente.sin_port), (char *) ctime(&timevar));
    for (;;)
    {
        bzero(buff, MAXLINE);
        int len, n; 
  
        len = sizeof(cliaddr);  //len is value/resuslt 
  
        recvfrom(sockfd, (char *)buff, MAXLINE, MSG_WAITALL, ( struct sockaddr *) &cliaddr, &len); 
        switch(comprobarOrden(buff))
        {
            case 0:
            printf("Client : %s\n", buff);
            listUDP(sockfd,cliaddr);
            bzero(buff, MAXLINE);
            break;

            case 1:
            printf("Client : %s\n", buff);
            newGroupsUDP(sockfd,cliaddr,buff);
            bzero(buff, MAXLINE);
            break;

            case 2:
            newNewsUDP(sockfd,cliaddr,buff);
            bzero(buff,MAXLINE);
            break;
            case 3:
            groupUDP(sockfd,cliaddr,buff);
            bzero(buff, MAXLINE);
            break;

            case 4:
            articleUDP(sockfd,cliaddr,buff);
            bzero(buff, MAXLINE);
            break;
            
            case 5:
            headUDP(sockfd,cliaddr,buff);
            bzero(buff, MAXLINE);
            break;

            case 6:
            bodyUDP(sockfd,cliaddr,buff);
            bzero(buff, MAXLINE);
            break;

            case 7:
            postUDP(sockfd,cliaddr,buff);
            bzero(buff, MAXLINE);
            break;

            case 8:
            quitUDP(sockfd,cliaddr);
            exit(0);
            break;

            default:
            printf("500 comando desconocido\n");
            exit(5);
        }
    }
}