/*
** Fichero: funcionesClienteTCP.c
** Autores:
** Erick José Mercado Hernández DNI 43840208T
** Javier Merchán Benito DNI 70923644Q
*/

#include "funcionesClienteTCP.h"
#include "funcionesEscritura.h"
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
#include <sys/ipc.h>
#include <sys/sem.h>

#define MAX 256
#define SA struct sockaddr 
struct sembuf;

int list(int sockfd, struct sockaddr_in cliente, struct sembuf *semact) 
{ 
    char respuestaNumerica[MAX];
    bzero(respuestaNumerica, MAX);
    char datos[MAX];
    bzero(datos,MAX);

    char *recibido = "recibo";
    FILE *fp;
    char nombreCliente[100];

    if (recv(sockfd,respuestaNumerica,MAX,0)<0)
    {
        printf("no se ha recibido recv\n");
    }
    //printf("Servidor : %s", respuestaNumerica);
    escrituraCliente(respuestaNumerica,semact,cliente); 
    //bzero(buffer, MAX);
    send(sockfd,recibido,strlen(recibido),0);

    if (recv(sockfd,datos,MAX,0)<0)
    {
        printf("no se ha recibido recv\n");
    } 
    escrituraCliente(datos,semact,cliente); 
    //printf("Servidor : %s", datos); 
    //bzero(buffer, MAX); 
    

}//End of list 

/*
*
*
*
*/

void newsGroups(int sockfd, struct sockaddr_in cliente, struct sembuf *semact) 
{ 
   
    char respuestaNumerica[MAX];
    char datos[MAX];  
    bzero(respuestaNumerica, MAX);
    bzero(datos,MAX); 
    char *recibido = "recibo";

    if (recv(sockfd,respuestaNumerica,sizeof(respuestaNumerica),0)<0)
    {
        printf("no se ha recibido recv\n");
    }
    //printf("Servidor : %s\n", respuestaNumerica); 
    escrituraCliente(respuestaNumerica,semact,cliente);
    bzero(respuestaNumerica, MAX); 
    send(sockfd,recibido,strlen(recibido),0);
    if (recv(sockfd,datos,sizeof(datos),0)<0)
    {
        printf("no se ha recibido recv\n");
    }

    if (strcmp(datos,".")==0)
    {
        printf("Servidor : %s\n",datos);
	escrituraCliente(datos,semact,cliente);
    }
    else
    {
        printf("Servidor : %s\n",datos);
        send(sockfd,recibido,strlen(recibido),0);
        bzero(datos,MAX);
        if (recv(sockfd,datos,sizeof(datos),0)<0)
        {
            printf("no se ha recibido recv\n");
        }
        escrituraCliente(datos,semact,cliente);
    }
     


}//End of newsGroup 

/*
*
*
*
*/

void newNew (int sockfd, struct sockaddr_in cliente, struct sembuf *semact)
{
    char respuestaNumerica[MAX];
    char datos[MAX][MAX];
    bzero(respuestaNumerica, MAX);
    bzero(datos,MAX);
    char *recibido = "recibo";
    if (recv(sockfd,respuestaNumerica,sizeof(respuestaNumerica),0)<0)
    {
        printf("no se ha recibido recv\n");
    }
    send(sockfd,recibido,strlen(recibido),0);
    //printf("Servidor : %s\n", respuestaNumerica); 
    escrituraCliente(respuestaNumerica,semact,cliente);
    bzero(respuestaNumerica, MAX);

    if (recv(sockfd,datos,sizeof(datos),0)<0)
    {
        printf("no se ha recibido recv\n");
    }
    int i;
    while(strlen(datos[i])>0)
    {
        //printf("Servidor : %s", datos[i]);    /* code */
	escrituraCliente(datos[i],semact,cliente);
        i++;
        if (strlen(datos[i])==0)
        {
            i = i+1;    
        }
    }
    bzero(datos, MAX);

}// End of newnews

/*
*
*
*
*/

void group (int sockfd,char *ordenes, struct sockaddr_in cliente,
 struct sembuf *semact)
{

    char respuestaNumerica[MAX];
    char datos[MAX];
    bzero(respuestaNumerica, MAX);
    bzero(datos,MAX);
    char *recibido = "recibo";
    int n, i;
    int salir = 0;
    char articulo[1024];
    char *token;
    char *token2;
    char *token3;
    char carpetas[2][MAX];
    char nArticulo[MAX][MAX];
    char grupoArticulo[MAX];
    char nuevaOrden[MAX];
    char dummy[5][MAX];

    FILE *fp;
    ssize_t line_size;
    char *line_buf = NULL;
    size_t line_buf_size = 0;
    char lineasOrdenes[90][90];
    char articuloAr[MAX];
    char articuloHe[MAX];
    char articuloBo[MAX];

    bzero(articulo,MAX);

    n=0;

    if (recv(sockfd,respuestaNumerica,sizeof(respuestaNumerica),0)<0)
    {
        printf("no se ha recibido recv\n");
    }

    escrituraCliente(respuestaNumerica,semact,cliente);
    bzero(respuestaNumerica, sizeof(respuestaNumerica));
    send(sockfd,recibido,strlen(recibido),0);

    if (recv(sockfd,datos,sizeof(datos),0)<0)
    {
        printf("no se ha recibido recv\n");
    } 
    escrituraCliente(datos,semact,cliente);
    bzero(datos, sizeof(datos));
    send(sockfd,recibido,strlen(recibido),0);
    recv(sockfd,grupoArticulo,MAX,0);

    i = 0;
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
		int ordenComprobar = comprobarOrden(nuevaOrden);
        switch(ordenComprobar)
        {
            case 4:
	    printf(" Ingrese por : %d\n",ordenComprobar);
            send(sockfd,nuevaOrden,strlen(nuevaOrden),0);
            token2 = strtok(carpetas[1],"\n");
            send(sockfd,nuevaOrden,strlen(nuevaOrden),0);

            token2 = strtok(carpetas[1],"\n");
            printf( " El Valor de token2 es : %s\n", token2 );
            //strcat(articulo,token2);

            token = strtok(nuevaOrden," ");
            i =0;

            while(token!=NULL)
            {
		printf( " El Valor de token es : %s\n", token );
                strcpy(nArticulo[i],token);
                token = strtok(NULL," ");
                i++;
            }
            //strcat(articulo,"/");

            token3 = strtok(nArticulo[1],"\n");

		printf( " El Valor de token3 es : %s\n", token3 );

            //strcat(articulo,token3);
		sprintf(articuloAr,"%s%s/\0",articulo, token2);
	    strcat(articuloAr,token3);
            article(sockfd,articuloAr,cliente,semact);
            n = 0;
            explicit_bzero(articulo,sizeof(articulo));
            bzero(nuevaOrden,sizeof(nuevaOrden));

            break;

            case 5:
            send(sockfd,nuevaOrden,strlen(nuevaOrden),0);

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
            head(sockfd,articulo,cliente,semact);
            n = 0;
            explicit_bzero(articulo,sizeof(articulo));
            bzero(nuevaOrden,sizeof(nuevaOrden));

            break;

            case 6:

            send(sockfd,nuevaOrden,strlen(nuevaOrden),0);    

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
            body(sockfd,articulo,cliente,semact);
            n = 0;
            explicit_bzero(articulo,sizeof(articulo));
            bzero(nuevaOrden,sizeof(nuevaOrden));

            break;

            case 8:
            send(sockfd,nuevaOrden,strlen(nuevaOrden),0);
            //salir=1;
            quit(sockfd);
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

void article (int sockfd, char *rutaDelArticulo,struct sockaddr_in cliente,
 struct sembuf *semact)
{
    char respuestaNumerica[MAX];
    char datos[MAX][MAX];
    char ruta[MAX];
    int i =0;
    size_t line_size;

    bzero(respuestaNumerica, sizeof(respuestaNumerica));
    sleep(1);
    strcpy(ruta,rutaDelArticulo);
    send(sockfd,ruta,MAX,0);
    recv(sockfd,respuestaNumerica,sizeof(respuestaNumerica),0);
    //printf("%s\n",respuestaNumerica);
    escrituraCliente(respuestaNumerica,semact,cliente);
    if (strncmp("423",respuestaNumerica,3)==0)
    {
        bzero(respuestaNumerica,sizeof(respuestaNumerica));
	escrituraCliente(respuestaNumerica,semact,cliente);
        return;
    }
    else
    {
    
    recv(sockfd,datos,sizeof(datos),0);
        while(strlen(datos[i])>0)
    {
        //printf("Servidor : %s", datos[i]);    /* code */
	escrituraCliente(datos[i],semact,cliente);
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

void head (int sockfd, char *rutaDelArticulo,
 struct sockaddr_in cliente, struct sembuf *semact)
{
    char respuestaNumerica[MAX];
    char datos[MAX][MAX];
    char ruta[MAX];
    int i =0;
    size_t line_size;

    bzero(respuestaNumerica, sizeof(respuestaNumerica));
    //printf("%s\n",rutaDelArticulo );
    sleep(1);
    strcpy(ruta,rutaDelArticulo);
    //printf("%s\n",ruta);
    send(sockfd,ruta,MAX,0);
    recv(sockfd,respuestaNumerica,sizeof(respuestaNumerica),0);
    //printf("%s\n",respuestaNumerica);
    escrituraCliente(respuestaNumerica,semact,cliente);
    if (strncmp("421",respuestaNumerica,3)==0)
    {
        //escrituraCliente(respuestaNumerica,semact,cliente);
        bzero(respuestaNumerica,sizeof(respuestaNumerica));
        return;
    }
    else
    {
    
    recv(sockfd,datos,sizeof(datos),0);
        while(strlen(datos[i])>0)
    {
        //printf("Servidor : %s", datos[i]);    /* code */
	escrituraCliente(datos[i],semact,cliente);
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

void body (int sockfd, char *rutaDelArticulo, struct sockaddr_in cliente,
 struct sembuf *semact)
{
    char respuestaNumerica[MAX];
    char datos[MAX][MAX];
    char ruta[MAX];
    int i =0;
    size_t line_size;

    bzero(respuestaNumerica, sizeof(respuestaNumerica));
    printf("%s\n",rutaDelArticulo );
    sleep(1);
    strcpy(ruta,rutaDelArticulo);
    printf("%s\n",ruta);
    send(sockfd,ruta,MAX,0);
    recv(sockfd,respuestaNumerica,sizeof(respuestaNumerica),0);
    //printf("%s\n",respuestaNumerica);
    escrituraCliente(respuestaNumerica,semact,cliente);
    if (strncmp("422",respuestaNumerica,3)==0)
    {
        bzero(respuestaNumerica,sizeof(respuestaNumerica));
    }
    else
    {
    
    recv(sockfd,datos,sizeof(datos),0);
        while(strlen(datos[i])>0)
    {
        //printf("Servidor : %s", datos[i]);    /* code */
        escrituraCliente(datos[i],semact,cliente);
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

void post(int sockfd)
{
    char respuestaNumerica[MAX];
    char datos[MAX];
    char grupo[MAX];
    char subject[MAX];
    char date[MAX];
    char cuerpo[MAX];
    char exit[10];
    int n=0,j=0,k=0,l=0;


    bzero(respuestaNumerica, MAX);
    bzero(datos,MAX);
    bzero(grupo,MAX);
    bzero(subject,MAX);
    bzero(date,MAX);
    bzero(cuerpo,MAX);

    recv(sockfd,respuestaNumerica,sizeof(respuestaNumerica),0);
    printf("%s",respuestaNumerica);

    printf("NEWSGROUPS: ");
    while ((grupo[n++] = getchar()) != '\n');
    send(sockfd,grupo,MAX,0);
    bzero(datos,MAX);
    recv(sockfd,datos,sizeof(datos),0);

    printf("SUBJECT: ");
    while ((subject[j++] = getchar()) != '\n');
    send(sockfd,subject,MAX,0);
    bzero(datos,MAX);
    recv(sockfd,datos,sizeof(datos),0);


    printf("BODY: \n");
    
    while ((cuerpo[l++] = getchar()) != '.');    
    while((exit[0]=getchar())!='\n');
    
    send(sockfd,cuerpo,MAX,0);
    bzero(datos,MAX);
    recv(sockfd,datos,sizeof(datos),0);
    printf("%s\n",datos);
}

/*
*
*
*
*/

void quit(int sockfd)
{
    char buff[MAX];
    bzero(buff,MAX);
    recv(sockfd,buff,MAX,0);
    send(sockfd,buff,strlen(buff),0);
    printf("Servidor: %s\n",buff );
    exit(2); 
}

void errorComando(int sockfd)
{
    char buff[MAX];
    bzero(buff,MAX);
    recv(sockfd,buff,sizeof(buff),0);
}

int comprobarOrden(char *buff)
{
    if (strncmp("LIST", buff, 4) == 0|| strncmp("list", buff, 4) == 0)
        {
            return 0;
        }
        else if (strncmp("NEWGROUPS", buff, 9) == 0|| strncmp("newgroups", buff, 9) == 0)
        {
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
        else if (strncmp("ARTICLE", buff, 4) == 0 || strncmp("article", buff, 4) == 0)
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

int escrituraCliente(char *buff,struct sembuf *semact, struct sockaddr_in cliente)
{
	FILE *fp;
	char nombreCliente[100];
	sprintf(nombreCliente,"clientes/cliente_%u.txt",ntohs(cliente.sin_port));
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

