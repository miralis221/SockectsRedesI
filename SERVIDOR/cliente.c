/*
** Fichero: TCPClient.c
** Autores:
** Erick José Mercado Hernández DNI 43840208T
** Javier Merchán Benito DNI 70923644Q
*/
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <netdb.h>
#include <string.h>
#include <time.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/sem.h>
#include "funciones/funcionesClienteTCP.h"
#include "funciones/funcionesEscritura.h"

#define MAX 256 
#define MAXLINE 1024 
#define PORT 3644 
#define SA struct sockaddr 
#define n_semaforos 10

int listUDP(int,struct sockaddr_in);
void newGroupsUDP(int,struct sockaddr_in);
void newNewsUDP (int,struct sockaddr_in);
void groupUDP (int,struct sockaddr_in);
void articleUDP (int,struct sockaddr_in,char *);
void headUDP (int,struct sockaddr_in,char *);
void bodyUDP(int,struct sockaddr_in,char *);
void postUDP(int,struct sockaddr_in);
void quitUDP(int,struct sockaddr_in);
int comprobarOrdenUDP(char *buff);

long int semid;
  
int main(argc,argv) 
int argc;
char *argv[];
{ 
    int sockfd; 
    struct addrinfo hints, *res;
    struct sockaddr_in servaddr; /*for server socket address */
    struct sockaddr_in myaddres; /*for local socket address*/ 
    char buff[MAX];
    char buffer[MAXLINE];
    int n,errcode;
    long timevar;
    int addrlen, i, j;
    int tipoCliente;
    int len; 
    FILE *fp;
	ssize_t line_size;
    char *line_buf = NULL;
    size_t line_buf_size = 0;
    char lineasOrdenes[90][90];
    char *token;

    struct sembuf semact;
    key_t clave = ftok ("/bin/ls", '3');

    // socket create and varification 

    if (argc != 4) 
    {
        fprintf(stderr, "Usage:  %s <remote host>\n", argv[0]);
        exit(1);
    }
    if (strcmp(argv[2],"TCP")==0)
    {
        tipoCliente = 0;
    }
    else if (strcmp(argv[2],"UDP")==0)
    {
        tipoCliente = 1;
    }
    else
    {
        fprintf(stderr, "No ha seleccionado ningun cliente\n");
        exit(1);
    }
	
    if((semid=semget(clave,n_semaforos,IPC_CREAT|0777))==-1)
    {
        perror("No se ha podido crear el array de semaforos");
        exit(1);
    }
    if(semctl(semid,2,SETVAL,1)==-1)
    {
        fprintf(stderr,"No se ha podido iniciar sem 1\n");
        exit(2);        
    }

    switch(tipoCliente)
    {
        case 0:
            sockfd = socket(AF_INET, SOCK_STREAM, 0); 
            if (sockfd == -1) 
            { 
                printf("socket creation failed...\n"); 
                exit(0); 
            } 
            bzero(&servaddr, sizeof(servaddr)); 
            
            memset ((char *)&myaddres, 0, sizeof(struct sockaddr_in));
            memset ((char *)&servaddr, 0, sizeof(struct sockaddr_in));

            /* Set up the peer address to which we will connect. */
            servaddr.sin_family = AF_INET;
            // assign IP, PORT 
            memset (&hints, 0, sizeof (hints));
            hints.ai_family = AF_INET;

             /* esta función es la recomendada para la compatibilidad con IPv6 gethostbyname queda obsoleta*/
            errcode = getaddrinfo(argv[1], NULL, &hints, &res); 
            if (errcode != 0)
            {
                    /* Name was not found.  Return a
                     * special value signifying the error. */
                fprintf(stderr, "%s: No es posible resolver la IP de %s\n",
                    argv[0],argv[1]);
                exit(1);

            }
            else 
            {
               /* Copy address of host */
               servaddr.sin_addr = ((struct sockaddr_in *) res->ai_addr)->sin_addr;
            }

            freeaddrinfo(res);
            servaddr.sin_port = htons(PORT); 
          
            // connect the client socket to server socket 
            if (connect(sockfd, (SA*)&servaddr, sizeof(servaddr)) != 0) { 
                printf("connection with the server failed...\n"); 
                exit(0); 
            } 

            addrlen = sizeof(struct sockaddr_in);
            if (getsockname(sockfd, (struct sockaddr *)&myaddres, &addrlen) == -1)
            {
                perror(argv[0]);
                fprintf(stderr, "%s: unable to read socket address\n", argv[0]);
                exit(1);
            }

            time(&timevar);

            fp = fopen(argv[3],"r");
        	if (fp == NULL)
        	{
        		fprintf(stderr,"No se encuentra el archivo de ordenes\n");
        		exit(2);
        	}
        	n = 0;
        	line_size = getline(&line_buf,&line_buf_size,fp); 
        	strcpy(lineasOrdenes[n],line_buf);
        	while(line_size>=0)
        	{
        		n++;
        		line_size = getline(&line_buf,&line_buf_size,fp);
        		strcpy(lineasOrdenes[n],line_buf);
        	}
        	fclose(fp);
        	n = 0;    
        for (;;)
        {

            switch(comprobarOrden(lineasOrdenes[n]))
            {
                case 0:
                                  
        	    send(sockfd, lineasOrdenes[n], strlen(lineasOrdenes[n]),0);
                    list(sockfd,myaddres,&semact);
                    n++;
                    break;
                case 1:

                    send(sockfd, lineasOrdenes[n], strlen(lineasOrdenes[n]),0);
                    newsGroups(sockfd,myaddres,&semact);
                    n++;
                    break;
                case 2:

                    send(sockfd, lineasOrdenes[n], strlen(lineasOrdenes[n]),0);
                    newNew(sockfd,myaddres,&semact);
                    bzero(buff,MAX);
                    n++;
                break; 
                
                case 3:
                    send(sockfd, lineasOrdenes[n], strlen(lineasOrdenes[n]),0);
                    group(sockfd,argv[3],myaddres,&semact);
                    n++;
                break;
                case 7:
                    send(sockfd, lineasOrdenes[n], strlen(lineasOrdenes[n]),0);
                    post(sockfd);   
                    n++; 
                break;        
                case 8:
                    send(sockfd, lineasOrdenes[n], strlen(lineasOrdenes[n]),0);
                    quit(sockfd);
                    break;
      			default:
      			printf("%s\n",lineasOrdenes[n] );
      			send(sockfd, lineasOrdenes[n], strlen(lineasOrdenes[n]),0);
      			n++;
      			break;
            }
        }
  
        // close the socket 
        close(sockfd); 
        break;

        case 1:
            // Creating socket file descriptor 
            if ( (sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0 ) 
            { 
                perror("socket creation failed"); 
                exit(EXIT_FAILURE); 
            } 
  
            memset(&servaddr, 0, sizeof(servaddr)); 
              
            // Filling server information 
            servaddr.sin_family = AF_INET; 
            servaddr.sin_port = htons(PORT); 
            servaddr.sin_addr.s_addr = INADDR_ANY; 

            for (;;)
            {
                n = 0;
                bzero(buffer, sizeof(buffer));  
                printf("Introduzca una orden : ");
                while ((buffer[n++] = getchar()) != '\n');

                switch(comprobarOrden(buffer))
                {
                    case 0: 
                        sendto(sockfd, (const char *)buffer, sizeof(buffer), MSG_CONFIRM, (const struct sockaddr *) &servaddr,  sizeof(servaddr));
                        listUDP(sockfd,servaddr);
                        bzero(buffer,MAXLINE);
                        break;
                    case 1:
                        sendto(sockfd, (const char *)buffer, sizeof(buffer), MSG_CONFIRM, (const struct sockaddr *) &servaddr,  sizeof(servaddr));
                        newGroupsUDP(sockfd,servaddr);
                        bzero(buffer,MAXLINE);
                        break;
                    case 2:
                        sendto(sockfd, (const char *)buffer, sizeof(buffer), MSG_CONFIRM, (const struct sockaddr *) &servaddr,  sizeof(servaddr));
                        newNewsUDP(sockfd,servaddr);
                        bzero(buffer,MAXLINE);
                    break; 
                    case 3:
                        sendto(sockfd, (const char *)buffer, sizeof(buffer), MSG_CONFIRM, (const struct sockaddr *) &servaddr,  sizeof(servaddr));
                        groupUDP(sockfd,servaddr);
                        bzero(buffer,MAXLINE);
                    break;

                    case 7:
                        sendto(sockfd, (const char *)buffer, sizeof(buffer), MSG_CONFIRM, (const struct sockaddr *) &servaddr,  sizeof(servaddr));
                        postUDP(sockfd,servaddr);   
                        bzero(buffer,MAXLINE);
                    break;          
                    case 8:
                        sendto(sockfd, (const char *)buffer, sizeof(buffer), MSG_CONFIRM, (const struct sockaddr *) &servaddr,  sizeof(servaddr));
                        bzero(buffer,MAXLINE);
                        quitUDP(sockfd,servaddr);  
                }
            }
            // close the socket 
            close(sockfd); 
        break;

        default:
        break;
    }
    
} 
/*
*
*
*
*/

int listUDP(int sockfd,struct sockaddr_in servaddr) 
{ 
    char buffer[MAXLINE];
    bzero(buffer, MAXLINE);

    int n, len; 

    n = recvfrom(sockfd, (char *)buffer, MAXLINE,  
                MSG_WAITALL, (struct sockaddr *) &servaddr, 
                &len);
    if (n<0)
    {
        printf("no se ha recibido recv\n");
    } 

    printf("Servidor : %s", buffer); 
    bzero(buffer, 1024);
    
    n = recvfrom(sockfd, (char *)buffer, MAXLINE,  
                MSG_WAITALL, (struct sockaddr *) &servaddr, 
                &len);
    if (n<0)
    {
        printf("no se ha recibido recv\n");
    }  
    
    printf("Servidor : %s", buffer); 
    bzero(buffer, 1024);
    

}//End of list 

/*
*
*
*
*/

void newGroupsUDP(int sockfd,struct sockaddr_in servaddr) 
{ 
    
    //struct sockaddr_in servaddr; 
    char respuestaNumerica[1024];
    char datos[1024];  
    bzero(respuestaNumerica, 1024);
    bzero(datos,1024); 

    int n, len; 

    n = recvfrom(sockfd, (char *)respuestaNumerica, MAXLINE,  
                MSG_WAITALL, (struct sockaddr *) &servaddr, 
                &len);
    if (n<0)
    {
        printf("no se ha recibido recv\n");
    } 

    printf("Servidor : %s\n", respuestaNumerica); 
    bzero(respuestaNumerica, 1024); 

    n = recvfrom(sockfd, (char *)datos, MAXLINE,  
                MSG_WAITALL, (struct sockaddr *) &servaddr, 
                &len);
    if (n<0)
    {
        printf("no se ha recibido recv\n");
    } 

    printf("Servidor : %s\n", datos); 
    bzero(datos, 1024); 


}//End of newsGroup 

/*
*
*
*
*/

void newNewsUDP (int sockfd,struct sockaddr_in servaddr)
{
    //struct sockaddr_in servaddr; 
    char respuestaNumerica[1024];
    char datos[1024][1024];  
    bzero(respuestaNumerica, 1024);
    bzero(datos,1024); 
    char mensaje[MAXLINE]="Ha llegado";

    int n, len; 
    int i;

    n = recvfrom(sockfd, (char *)respuestaNumerica, MAXLINE,  
                0, (struct sockaddr *) &servaddr, 
                &len);
    if (n<0)
    {
        printf("no se ha recibido recv\n");
    } 
    printf("Servidor : %s\n", respuestaNumerica); 
    bzero(respuestaNumerica, 1024);

    sendto(sockfd, mensaje, strlen(mensaje),  
        MSG_CONFIRM, (const struct sockaddr *) &servaddr, sizeof(servaddr));
    printf("HOLI" );
    
    n = recvfrom(sockfd, datos, MAXLINE,  
                0, (struct sockaddr *) &servaddr, 
                &len);
    if (n<0)
    {
        printf("no se ha recibido recv\n");
    }
    //read(sockfd,datos,MAXLINE);
    printf("HOLI" );
    i=0;
    while(strlen(datos[i])>0)
    {
        printf("Servidor : %s", datos[i]); 
        i++;
        if (strlen(datos[i])==0)
        {
            i = i+1;    
        }
    }
    //printf("Servidor : %s\n", datos); 
    bzero(datos, 1024);

}// End of newnews

/*
*
*
*
*/

void headUDP (int sockfd,struct sockaddr_in servaddr,char *rutaDelArticulo)
{

    
    char respuestaNumerica[256];
    char datos[256][256];
    char ruta[MAXLINE];
    int i =0;
    size_t line_size;
    int len, n;

    //len = sizeof(cliaddr);

    bzero(respuestaNumerica, sizeof(respuestaNumerica));
   
    sleep(1);
    strcpy(ruta,rutaDelArticulo);
    
    sendto(sockfd, ruta, strlen(ruta),  
        MSG_CONFIRM, (const struct sockaddr *) &servaddr, sizeof(servaddr));
    n = recvfrom(sockfd, (char *)respuestaNumerica, MAXLINE,  
                0, (struct sockaddr *) &servaddr, 
                &len);
    if (n<0)
    {
        printf("no se ha recibido recv\n");
    } 
    printf("%s\n",respuestaNumerica);
    if (strncmp("421",respuestaNumerica,3)==0)
    {
        bzero(respuestaNumerica,sizeof(respuestaNumerica));
    }
    else
    {
    
        n = recvfrom(sockfd, (char *)datos, MAXLINE,  
                0, (struct sockaddr *) &servaddr, 
                &len);
        if (n<0)
        {
            printf("no se ha recibido recv\n");
        }

        while(strlen(datos[i])>0)
        {
            printf("Servidor : %s", datos[i]);    /* code */
            i++;
            if (strlen(datos[i])==0)
            {
                i = i+1;    
            }
        }
    }

}//End of head

/*
*
*
*/

void articleUDP (int sockfd,struct sockaddr_in servaddr,char *rutaDelArticulo)
{   
    char respuestaNumerica[256];
    char datos[256][256];
    char ruta[MAXLINE];
    int i =0;
    size_t line_size;
    int len, n; 
  
   //len = sizeof(cliaddr);


    bzero(respuestaNumerica, sizeof(respuestaNumerica));
    
    sleep(1);
    strcpy(ruta,rutaDelArticulo);
  
    sendto(sockfd, ruta, strlen(ruta),  
        MSG_CONFIRM, (const struct sockaddr *) &servaddr, sizeof(servaddr));
    n = recvfrom(sockfd, (char *)respuestaNumerica, MAXLINE,  
                0, (struct sockaddr *) &servaddr, 
                &len);
    if (n<0)
    {
        printf("no se ha recibido recv\n");
    } 
    printf("%s\n",respuestaNumerica);
    if (strncmp("423",respuestaNumerica,3)==0)
    {
        bzero(respuestaNumerica,sizeof(respuestaNumerica));
    }
    else
    {
    
        n = recvfrom(sockfd, (char *)datos, MAXLINE,  
                0, (struct sockaddr *) &servaddr, 
                &len);
        if (n<0)
        {
            printf("no se ha recibido recv\n");
        }

        while(strlen(datos[i])>0)
        {
            printf("Servidor : %s", datos[i]);    /* code */
            i++;
            if (strlen(datos[i])==0)
            {
                i = i+1;    
            }
        }
    }
}//End of article

/*
*
*
*/

void bodyUDP (int sockfd,struct sockaddr_in servaddr,char *rutaDelArticulo)
{
    char respuestaNumerica[256];
    char datos[256][256];
    char ruta[MAXLINE];
    int i =0;
    size_t line_size;
    int len, n; 
  
    //len = sizeof(cliaddr);


    bzero(respuestaNumerica, sizeof(respuestaNumerica));
  
    sleep(1);
    strcpy(ruta,rutaDelArticulo);
    
    sendto(sockfd, ruta, strlen(ruta),  
        MSG_CONFIRM, (const struct sockaddr *) &servaddr, sizeof(servaddr));
    n = recvfrom(sockfd, (char *)respuestaNumerica, MAXLINE,  
                0, (struct sockaddr *) &servaddr, 
                &len);
    if (n<0)
    {
        printf("no se ha recibido recv\n");
    } 
    printf("%s\n",respuestaNumerica);
    if (strncmp("422",respuestaNumerica,3)==0)
    {
        bzero(respuestaNumerica,sizeof(respuestaNumerica));
    }
    else
    {
    
        n = recvfrom(sockfd, (char *)datos, MAXLINE,  
                0, (struct sockaddr *) &servaddr, 
                &len);
        if (n<0)
        {
            printf("no se ha recibido recv\n");
        } 
            while(strlen(datos[i])>0)
        {
            printf("Servidor : %s", datos[i]);    /* code */
            i++;
            if (strlen(datos[i])==0)
            {
                i = i+1;    
            }
        }
    }
}//End of body

/*
*
*
*/

void groupUDP (int sockfd,struct sockaddr_in servaddr)
{

    char respuestaNumerica[256];
    char datos[256];
    bzero(respuestaNumerica, 256);
    bzero(datos,256);
    char *recibido = "recibo";
    int n, i;
    int salir = 0;
    char articulo[MAXLINE];
    char *token;
    char *token2;
    char *token3;
    char carpetas[2][MAXLINE];
    char nArticulo[MAXLINE][MAXLINE];
    char grupoArticulo[MAXLINE];
    char nuevaOrden[MAXLINE];
    char dummy[5][MAXLINE];
    int len, j; 
  

    n=0;

    j = recvfrom(sockfd, (char *)respuestaNumerica, MAXLINE,  
                0, (struct sockaddr *) &servaddr, 
                &len);
    if (j<0)
    {
        printf("no se ha recibido recv\n");
    } 

    printf("Servidor : %s", respuestaNumerica);

    bzero(respuestaNumerica, sizeof(respuestaNumerica));
   

    j = recvfrom(sockfd, datos, MAXLINE,  
                0, (struct sockaddr *) &servaddr, 
                &len);
    if (j<0)
    {
        printf("no se ha recibido recv\n");
    }
    printf("Servidor : %s\n", datos);
    bzero(datos, sizeof(datos));

    j = recvfrom(sockfd, grupoArticulo, MAXLINE,  
                0, (struct sockaddr *) &servaddr, 
                &len);
    if (j<0)
    {
        printf("no se ha recibido recv\n");
    }

    i = 0;
    printf("%s\n", grupoArticulo);
    token2 = strtok(grupoArticulo,".");
    while(token2!=NULL)
    {
        strcpy(carpetas[i],token2);
        token2 = strtok(NULL,".");
        i++;
    }     
   
        while(!salir)
        {
            strcpy(articulo,"noticias/articulos/local/");
            while ((nuevaOrden[n++] = getchar()) != '\n');
                switch(comprobarOrden(nuevaOrden))
                {
                    case 4:

                    sendto(sockfd, nuevaOrden, strlen(nuevaOrden),  
                        MSG_CONFIRM, (const struct sockaddr *) &servaddr, sizeof(servaddr));
                    

                    token2 = strtok(carpetas[1],"\n");
                    strcat(articulo,token2);

                    token = strtok(nuevaOrden," ");
                    i =0;

                    while(token!=NULL)
                    {
                        strcpy(nArticulo[i],token);
                        token = strtok(NULL," ");
                        i++;
                    }
                    strcat(articulo,"/");

                    token3 = strtok(nArticulo[1],"\n");
                    strcat(articulo,token3);
                    articleUDP(sockfd,servaddr,articulo);
                    n = 0;
                    explicit_bzero(articulo,sizeof(articulo));
                    bzero(nuevaOrden,sizeof(nuevaOrden));

                    break;

                    case 5:
                    sendto(sockfd, nuevaOrden, strlen(nuevaOrden),  
                        MSG_CONFIRM, (const struct sockaddr *) &servaddr, sizeof(servaddr));                

                    token2 = strtok(carpetas[1],"\n");
                    strcat(articulo,token2);

                    token = strtok(nuevaOrden," ");
                    i =0;

                    while(token!=NULL)
                    {
                        strcpy(nArticulo[i],token);
                        token = strtok(NULL," ");
                        i++;
                    }
                    strcat(articulo,"/");

                    token3 = strtok(nArticulo[1],"\n");
                    strcat(articulo,token3);
                    headUDP(sockfd,servaddr,articulo);
                    n = 0;
                    explicit_bzero(articulo,sizeof(articulo));
                    bzero(nuevaOrden,sizeof(nuevaOrden));

                    break;

                    case 6:

                    sendto(sockfd, nuevaOrden, strlen(nuevaOrden),  
                        MSG_CONFIRM, (const struct sockaddr *) &servaddr, sizeof(servaddr));  

                    token2 = strtok(carpetas[1],"\n");
                    strcat(articulo,token2);

                    token = strtok(nuevaOrden," ");
                    i =0;

                    while(token!=NULL)
                    {
                        strcpy(nArticulo[i],token);
                        token = strtok(NULL," ");
                        i++;
                    }
                    strcat(articulo,"/");

                    token3 = strtok(nArticulo[1],"\n");
                    strcat(articulo,token3);
                    bodyUDP(sockfd,servaddr,articulo);
                    n = 0;
                    explicit_bzero(articulo,sizeof(articulo));
                    bzero(nuevaOrden,sizeof(nuevaOrden));

                    break;

                    case 8:
                    sendto(sockfd, nuevaOrden, strlen(nuevaOrden),  
                        MSG_CONFIRM, (const struct sockaddr *) &servaddr, sizeof(servaddr));
                    quitUDP(sockfd,servaddr);
                    break;
                    default:
                    salir=1;
                    break;
            }
    }

}//End of Groups

/*
*
*
*
*/

void postUDP(int sockfd,struct sockaddr_in servaddr)
{

    char respuestaNumerica[256];
    char datos[256];
    char grupo[MAXLINE];
    char subject[MAXLINE];
    char date[MAXLINE];
    char cuerpo[256];
    char exit[10];
    int n=0,j=0,k=0,l=0;
    int len, q;

    bzero(respuestaNumerica, 256);
    bzero(datos,256);
    bzero(grupo,MAXLINE);
    bzero(subject,MAXLINE);
    bzero(date,MAXLINE);
    bzero(cuerpo,256);

    q = recvfrom(sockfd, respuestaNumerica, MAXLINE,  
                0, (struct sockaddr *) &servaddr, 
                &len);
    if (q<0)
    {
        printf("no se ha recibido recv\n");
    }
    printf("%s",respuestaNumerica);

    printf("NEWSGROUPS: ");
    while ((grupo[n++] = getchar()) != '\n');
    sendto(sockfd, grupo, sizeof(grupo),  
        MSG_CONFIRM, (const struct sockaddr *) &servaddr, sizeof(servaddr));

    bzero(datos,256);
    q = recvfrom(sockfd, datos, MAXLINE,  
                0, (struct sockaddr *) &servaddr, 
                &len);
    if (q<0)
    {
        printf("no se ha recibido recv\n");
    }
    printf("SUBJECT: ");
    while ((subject[j++] = getchar()) != '\n');
    sendto(sockfd, subject, sizeof(subject),  
        MSG_CONFIRM, (const struct sockaddr *) &servaddr, sizeof(servaddr));
    bzero(datos,256);
    q = recvfrom(sockfd, datos, MAXLINE,  
                0, (struct sockaddr *) &servaddr, 
                &len);
    if (q<0)
    {
        printf("no se ha recibido recv\n");
    }

    printf("BODY: \n");
    
    while ((cuerpo[l++] = getchar()) != '.');    
    while((exit[0]=getchar())!='\n');
    
    sendto(sockfd, cuerpo, sizeof(cuerpo),  
        MSG_CONFIRM, (const struct sockaddr *) &servaddr, sizeof(servaddr));
    bzero(datos,256);
    q = recvfrom(sockfd, datos, MAXLINE,  
                0, (struct sockaddr *) &servaddr, 
                &len);
    if (q<0)
    {
        printf("no se ha recibido recv\n");
    }
    printf("%s\n",datos);

}

/*
*
*
*
*/
void quitUDP(int sockfd,struct sockaddr_in servaddr)
{
    char buff[MAXLINE];
    bzero(buff,MAXLINE);
    int len, n; 

    n = recvfrom(sockfd, (char *)buff, MAXLINE,  
            0, (struct sockaddr *) &servaddr, &len);
    if (n<0)
    {
        printf("no se ha recibido recv\n");
    } 

    sendto(sockfd, buff, sizeof(buff),  
        MSG_CONFIRM, (const struct sockaddr *) &servaddr, sizeof(servaddr));

    printf("Servidor: %s\n",buff );
    exit(2); 
}
/*
*
*
*
*/
int comprobarOrdenUDP(char *buff)
{
    //printf("%s\n",buff );
    if (strncmp("LIST", buff, 4) == 0|| strncmp("list", buff, 4) == 0)
        {
            return 0;
        }
        else if (strncmp("NEWGROUPS", buff, 9) == 0|| strncmp("newgroups", buff, 9) == 0)
        {
            //printf("%s\n", buff);
            return 1;
        }
        else if (strncmp("NEWNEW", buff, 4) == 0|| strncmp("newnew", buff, 4) == 0)
        {
            return 2;
        }
        else if (strncmp("GROUP", buff, 4) == 0 || strncmp("group", buff, 4) == 0)
        {
            return 3;
        }
        else if (strncmp("ARTICLE", buff, 7) == 0 || strncmp("article", buff, 7) == 0)
        {
            return 4;
        }
        else if (strncmp("HEAD", buff, 4) == 0 || strncmp("head", buff, 4) == 0)
        {
            return 5;
        }
        else if (strncmp("BODY", buff, 4) == 0 || strncmp("body", buff, 4) == 0)
        {
            return 6;
        }
        else if (strncmp("POST", buff, 4) == 0 || strncmp("post", buff, 4) == 0)
        {
            return 7;
        }
        else if (strncmp("QUIT", buff, 4) == 0 || strncmp("quit", buff, 4) == 0)
        {
            return 8;
        }
} 
