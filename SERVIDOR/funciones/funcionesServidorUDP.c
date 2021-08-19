/*
** Fichero: funcionesServidorUDP.c
** Autores:
** Erick José Mercado Hernández DNI 43840208T
** Javier Merchán Benito DNI 70923644Q
*/



#include <stdio.h> 
#include <stdlib.h> 
#include <unistd.h> 
#include <string.h> 
#include <sys/types.h> 
#include <sys/socket.h> 
#include <arpa/inet.h> 
#include <netinet/in.h> 
#include <stdio.h> 
#include <netdb.h> 
#include <sys/socket.h> 
#include <sys/types.h> 
#include <fcntl.h>
#include <time.h>
#include <dirent.h>
#include <pwd.h>
#include <sys/ipc.h>
#include <wait.h>
#include <sys/sem.h>
#include <sys/stat.h>
#include "funcionesServidorUDP.h" 
  
#define PORT     8080 
#define MAX 1024 

#define FICHEROGRUPOS "noticias/grupos"
#define FICHEROLOG "nntpd.log"


void listUDP(int sockfd, struct sockaddr_in cliaddr)
{

	char mensaje[] = "215 listado de los grupos: \n";
	int fd;
	char bufferFichero;
	char buffer[1024];
	int i;
	bzero(buffer,1024);
	int len, n; 
  
    len = sizeof(cliaddr);

	fd = open("noticias/grupos",O_RDONLY);
    if (fd == -1)
   	{
        puts("archivo no existe\n");
    }
 	else
    {
		
		sendto(sockfd, mensaje, sizeof(mensaje),  
        	MSG_CONFIRM, (const struct sockaddr *) &cliaddr, len);
    	bzero(mensaje,strlen(mensaje)); 

		i =0 ;
		while(read(fd,&bufferFichero,sizeof(bufferFichero))!=0)
		{
			buffer[i] = bufferFichero;
			i++;

		}
		close(fd);
		sendto(sockfd, buffer, sizeof(buffer),  
        	MSG_CONFIRM, (const struct sockaddr *) &cliaddr, len); 
	}
	printf("Servidor : %s\n", mensaje);

}//End of list
/*
*
*
*/
void newGroupsUDP(int sockfd, struct sockaddr_in cliaddr, char *buff)
{

	char bufferFichero;
    char buffer[1024];
    char mensaje[] = "231 listado de grupos desde:";
    char cadena[1024][1024];
    char matriz[1024][1024];
    char fecha[1024][1024];
    char grupos[200];
    char *token;
    int i, j, k, l;
    int fechaN;
    int fd;
    int len, n; 
  
    len = sizeof(cliaddr);

    

    i = 0;
	token = strtok(buff, " ");

	while(token!=NULL)
	{
		strcpy(cadena[i],token); 
		token = strtok(NULL," ");
		i++;
	}
	fd = open("noticias/grupos",O_RDONLY);
	if (fd == -1)
	{
		puts("archivo no existe\n");
		exit(2);
	}
	else
	{
		strcat(mensaje," ");
		strcat(mensaje,cadena[1]);
		strcat(mensaje," ");
		strcat(mensaje,cadena[2]);
		sendto(sockfd, mensaje, strlen(mensaje),  
        	MSG_CONFIRM, (const struct sockaddr *) &cliaddr, len);
		bzero(mensaje,strlen(mensaje));

		i=0;
		while(read(fd,&bufferFichero,sizeof(bufferFichero))!=0)
		{
			buffer[i] = bufferFichero;
			i++;
		}
   		close(fd);   

		i=0;
		token = strtok(buffer,"\n");
		while(token!=NULL)
		{
			strcpy(matriz[i],token); 
			token = strtok(NULL,"\n");
			i++;
		} 


		j=0;
		for ( k = 0; k < i; ++k)
		{
			token = strtok(matriz[k],"  ");
			while(token!=NULL)
			{
				strcpy(fecha[j],token); 
				token = strtok(NULL," ");
				j++;
			}
		} 

		for (l = 0; l < j; l++)
		{
			if ((fechaN=atoi(fecha[l]))!=0 )
			{
				if(atoi(cadena[1])<=fechaN && atoi(cadena[2])<= atoi(fecha[l+1]))
				{
					strcat(grupos,fecha[l-3]);
					strcat(grupos,"\n");
				}
			}
		}
		sendto(sockfd, grupos, strlen(grupos),  
        	MSG_CONFIRM, (const struct sockaddr *) &cliaddr, len);
		bzero(grupos,strlen(grupos));
	}

}// End of newsgroups

/*
*
*
*
*/

void newNewsUDP (int sockfd, struct sockaddr_in cliaddr, char *buff)
{
	char bufferFichero;
	char lecturaFichero[1024];
    char mensaje[] = "230 Nuevos Articulos desde";
    char ArticulosNuevos[20];
    char ArticulosAntiguos[20];
    char cadena[1024][1024];
    char lineasDelFicheroGrupos[1024][1024];
    char lineasDelFicherosArticulos[1024][1024];
    char nombreGrupo[1024][1024];
    char direccionFinal[1024];
    char cadena2[256];
    char cadena3[3][20];
    char num[10];
    char algo[1024];
    char *token;
    int nArticulos;
    int i, j, k, l;
    int fd;
    char carpeta[20];
    FILE *fp;	
    ssize_t line_size;
    char *line_buf = NULL;
    size_t line_buf_size = 0;
    int len, n; 
  
    len = sizeof(cliaddr);

    i = 0;
	token = strtok(buff, " ");
	while(token!=NULL)
	{
		strcpy(cadena[i],token); 
		token = strtok(NULL," ");
		i++;
	}

	fd = open("noticias/grupos",O_RDWR);
	if (fd == -1)
	{
		puts("archivo no existe\n");
		exit(2);
	}
	else
	{
		strcat(cadena2,cadena[1]);
		strcat(mensaje," ");
		strcat(mensaje,cadena[2]);
		strcat(mensaje," ");
		strcat(mensaje,cadena[3]);

		sendto(sockfd, mensaje, strlen(mensaje),  
        	0, (const struct sockaddr *) &cliaddr, len);
		bzero(mensaje,strlen(mensaje));

		n = recvfrom(sockfd, algo, MAX,  
                0, (struct sockaddr *) &cliaddr, 
                &len);
    	if (n<0)
    	{
        	printf("no se ha recibido recv\n");
    	}

		i=0;
		while(read(fd,&bufferFichero,sizeof(bufferFichero))!=0)
		{
			lecturaFichero[i] = bufferFichero;
			i++;
		}

		i=0;
		token = strtok(lecturaFichero,"\n");
		while(token!=NULL)
		{
			strcpy(lineasDelFicheroGrupos[i],token); 
			token = strtok(NULL,"\n");
			i++;
		}

		j=0;
		for ( k = 0; k < i; ++k)
		{
			token = strtok(lineasDelFicheroGrupos[k],"  ");
			while(token!=NULL)
			{
				strcpy(nombreGrupo[j],token); 
				token = strtok(NULL," ");
				j++;
			}
		} 
   		close(fd);

   		for (l = 0; l < j; l++)
		{
			if (strncmp(cadena[1],nombreGrupo[l],9)==0)
			{
				nArticulos = atoi(nombreGrupo[l+1])-atoi(nombreGrupo[l+2])+1;
				token = strtok(cadena[1],".");
				j=0;
				while(token!=NULL)
				{
					strcpy(cadena3[j],token); 
					token = strtok(NULL,".");
					j++;
				}
				sprintf(cadena2,"noticias/articulos/%s/%s/",cadena3[0],cadena3[1]);
				j=0;
				for (i = 1; i <= nArticulos; ++i)
				{
					strcpy(direccionFinal,cadena2);
					sprintf(num,"%d",atoi(nombreGrupo[l+i]));
					strcat(direccionFinal,num);

					fp = fopen(direccionFinal,"r");
					line_size = getline(&line_buf,&line_buf_size,fp);
					strcpy(lineasDelFicherosArticulos[j],line_buf);

					while(line_size >= 0)
					{
						line_size = getline(&line_buf,&line_buf_size,fp);
						strcpy(lineasDelFicherosArticulos[j],line_buf);
						j++;
					}
					j++;
					fclose(fp);
				}


				
				sendto(sockfd, lineasDelFicherosArticulos,
				sizeof(lineasDelFicherosArticulos),  
        			0, (const struct sockaddr *) &cliaddr, len);
				
				
				break;
			}
		}

	}

}// End of newnew

/*
*
*
*
*
*/

void groupUDP (int sockfd, struct sockaddr_in cliaddr, char *buff)
{
	char bufferFichero;
    char buffer[6000];
    char mensaje[] = "211 numeros de articulos primero y ultimo del grupo ";
    char respuesta[MAX];
    char cadena[1024][1024];
    char matriz[1024][1024];
    char grupo[1024][1024];
    char *token;
    int nArticulos;
    int limites;
    int i, j, k, l;
    int fd;
    int len, n; 
  
    len = sizeof(cliaddr);

    i = 0;
	token = strtok(buff, " ");
	while(token!=NULL)
	{
		strcpy(cadena[i],token); 
		token = strtok(NULL," ");
		i++;
	}
	fd = open("noticias/grupos",O_RDONLY);
	if (fd == -1)
	{
		puts("archivo no existe\n");
		exit(2);
	}
	else
	{
		strcat(mensaje,cadena[1]);
		sendto(sockfd, mensaje, strlen(mensaje),  
        	MSG_CONFIRM, (const struct sockaddr *) &cliaddr, len);
		bzero(mensaje,strlen(mensaje));

		i=0;
		while(read(fd,&bufferFichero,sizeof(bufferFichero))!=0)
		{
			buffer[i] = bufferFichero;
			i++;
		}
   		close(fd);   

		i=0;
		token = strtok(buffer,"\n");
		while(token!=NULL)
		{
			strcpy(matriz[i],token); 
			token = strtok(NULL,"\n");
			i++;
		} 
		j=0;
		for ( k = 0; k < i; ++k)
		{
			token = strtok(matriz[k],"  ");
			while(token!=NULL)
			{
				strcpy(grupo[j],token); 
				token = strtok(NULL," ");
				j++;
			}

		}
		for (l = 0; l < j; l++)
		{

			if (strncmp(cadena[1],grupo[l],9)==0)
			{
				nArticulos = atoi(grupo[l+1])-atoi(grupo[l+2])+1;
				sprintf(respuesta,"Numero de articulos %d ",nArticulos);
				strcat(respuesta," Ultimo ");
				strcat(respuesta,grupo[l+1]);
				strcat(respuesta," primero: ");
				strcat(respuesta,grupo[l+2]);
				sendto(sockfd, respuesta, strlen(respuesta),  
        			MSG_CONFIRM, (const struct sockaddr *) &cliaddr, len);
				break;
			}
		} 
		sendto(sockfd, cadena[1], strlen(cadena[1]),  
        			MSG_CONFIRM, (const struct sockaddr *) &cliaddr, len);
	} 
}

void articleUDP (int sockfd, struct sockaddr_in cliaddr, char *buff)
{
	FILE *fp;
	char respuesta1[] = "223 articulo recuperado\n";
	char respuestaFallo[] = "423 no existe este articulo en el grupo de noticias\n";
	char rutaDelArticulo[MAX];
	char vacio[] = "    \n";
	bzero(rutaDelArticulo,MAX);
	int j;
	ssize_t line_size;
    char *line_buf = NULL;
    size_t line_buf_size = 0;
    char lineasDelFicherosArticulos[80][256];
    int len, n; 
  
    len = sizeof(cliaddr);

	n = recvfrom(sockfd, rutaDelArticulo, MAX,  
                0, (struct sockaddr *) &cliaddr, 
                &len);
    if (n<0)
    {
        printf("no se ha recibido recv\n");
    }

	fp = fopen(rutaDelArticulo,"r");
	if (fp == NULL)
	{
		sendto(sockfd, respuestaFallo, sizeof(respuestaFallo),  
        			MSG_CONFIRM, (const struct sockaddr *) &cliaddr, len);

	}
	else
	{
		sendto(sockfd, respuesta1, sizeof(respuesta1),  
        			MSG_CONFIRM, (const struct sockaddr *) &cliaddr, len);
		j = 0;
		line_size = getline(&line_buf,&line_buf_size,fp);
		strcpy(lineasDelFicherosArticulos[j],line_buf);
		while(line_size >= 0)
		{
			j++;
			line_size = getline(&line_buf,&line_buf_size,fp);
			strcpy(lineasDelFicherosArticulos[j],line_buf);
			
		}
		fclose(fp);
		sendto(sockfd, lineasDelFicherosArticulos, sizeof(lineasDelFicherosArticulos),  
        			MSG_CONFIRM, (const struct sockaddr *) &cliaddr, len);
	}
	

}

void headUDP (int sockfd, struct sockaddr_in cliaddr, char *buff)
{
	FILE *fp;
	char respuesta1[] = "221 cabezera del articulo recuperada\n";
	char respuestaFallo[] = "421 no existe este articulo en el grupo de noticias\n";
	char rutaDelArticulo[MAX];
	bzero(rutaDelArticulo,MAX);
	int j;
	ssize_t line_size;
    char *line_buf = NULL;
    size_t line_buf_size = 0;
    char lineasDelFicherosArticulos[80][256];
    int len, n; 
  
    len = sizeof(cliaddr);

	n = recvfrom(sockfd, rutaDelArticulo, MAX,  
                0, (struct sockaddr *) &cliaddr, 
                &len);
    if (n<0)
    {
        printf("no se ha recibido recv\n");
    }
	printf("%s\n",rutaDelArticulo );
	fp = fopen(rutaDelArticulo,"r");
	if (fp == NULL)
	{
		sendto(sockfd, respuestaFallo, sizeof(respuestaFallo),  
        	MSG_CONFIRM, (const struct sockaddr *) &cliaddr, len);
		
	}
	else
	{
		
		sendto(sockfd, respuesta1, sizeof(respuesta1),  
        			MSG_CONFIRM, (const struct sockaddr *) &cliaddr, len);
		j = 0;
		line_size = getline(&line_buf,&line_buf_size,fp);
		strcpy(lineasDelFicherosArticulos[j],line_buf);
		while(line_size >= 0)
		{
			j++;
			line_size = getline(&line_buf,&line_buf_size,fp);
			if (j<=3)
			{
				strcpy(lineasDelFicherosArticulos[j],line_buf);
			}
			
			
		}
		fclose(fp);
		sendto(sockfd, lineasDelFicherosArticulos, sizeof(lineasDelFicherosArticulos),  
        			MSG_CONFIRM, (const struct sockaddr *) &cliaddr, len);
	}

}

void bodyUDP(int sockfd, struct sockaddr_in cliaddr, char *buff)
{
	FILE *fp;
	char respuesta1[] = "222 cuerpo del articulo recuperado\n";
	char respuestaFallo[] = "422 no existe este articulo en el grupo de noticias\n";
	char rutaDelArticulo[MAX];
	bzero(rutaDelArticulo,MAX);
	int j,i;
	ssize_t line_size;
    char *line_buf = NULL;
    size_t line_buf_size = 0;
    char lineasDelFicherosArticulos[80][256];
    int len, n; 
  
    len = sizeof(cliaddr);

	n = recvfrom(sockfd, rutaDelArticulo, MAX,  
                0, (struct sockaddr *) &cliaddr, 
                &len);
    if (n<0)
    {
        printf("no se ha recibido recv\n");
    }
	
	fp = fopen(rutaDelArticulo,"r");
	if (fp == NULL)
	{
		sendto(sockfd, respuestaFallo, sizeof(respuestaFallo),  
        	MSG_CONFIRM, (const struct sockaddr *) &cliaddr, len);
		
	}
	else
	{
		
		sendto(sockfd, respuesta1, sizeof(respuesta1),  
        			MSG_CONFIRM, (const struct sockaddr *) &cliaddr, len);
		j = 0;
		i = 0;
		line_size = getline(&line_buf,&line_buf_size,fp);
		while(line_size >= 0)
		{
			j++;
			line_size = getline(&line_buf,&line_buf_size,fp);
			if (j>3)
			{
				strcpy(lineasDelFicherosArticulos[i],line_buf);
				i++;
			}
			
			
		}
		fclose(fp);
		sendto(sockfd, lineasDelFicherosArticulos, sizeof(lineasDelFicherosArticulos),  
        			MSG_CONFIRM, (const struct sockaddr *) &cliaddr, len);

	}
}

void postUDP(int sockfd, struct sockaddr_in cliaddr, char *buff)
{
	char respuesta1[] = "340 Subiendo un articulo; finalize con una linea que solo contenga un .\n";
	char respuesta2[] = "Carpeta Creada\n";
	char respuestaExito[] = "240 Articulo Recibido correctemente";
	char respuestaFallo[] = "441 No se ha podido subir el artículo";
	char grupo[MAX];
	char subject[MAX];
	char date[256];
	char body[256];
	char entrada[MAX][MAX];
	char carpetas[MAX][MAX];
	char *token;
	char *entrada2;
	char direccionGrupo[MAX];
	char direccionArticulos[]="noticias/articulos";
	int i,j;
	FILE *fp;
	long timevar;
    time (&timevar);
    int len, n; 
  
    len = sizeof(cliaddr);

    struct dirent **entradas= NULL;
    int numeroEntradas;
    int nArticulo;
    char buffnumerico[10];

    time_t rawtime = time(NULL);
    struct tm *ptm = localtime(&rawtime);

	sendto(sockfd, respuesta1, sizeof(respuesta1),  
        	MSG_CONFIRM, (const struct sockaddr *) &cliaddr, len);

	n = recvfrom(sockfd, grupo, MAX,  
                0, (struct sockaddr *) &cliaddr, 
                &len);
    if (n<0)
    {
        printf("no se ha recibido recv\n");
    }

	entrada2 = grupo;
	i=0;
	token = strtok(grupo,".");
	while(token != NULL)
	{
		strcpy(entrada[i],token);
		token = strtok(NULL,".");
		i++;
	}

	sprintf(direccionGrupo,"%s/%s/%s",direccionArticulos,
		entrada[0],
		entrada[1]);

	numeroEntradas = scandir (direccionGrupo, &entradas, NULL, NULL);
	if (numeroEntradas<0)
	{
		mkdir(direccionGrupo,S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
	}
	numeroEntradas = scandir (direccionGrupo, &entradas, NULL, NULL);
	printf("%d\n",numeroEntradas);

	if (numeroEntradas == 2)
	{
		nArticulo = 1;
		free(entradas);
		entradas = NULL;
	}
	else
	{
		nArticulo = (numeroEntradas-2)+1;
		for (i = 0; i < numeroEntradas; ++i)
		{
			free(entradas[i]);
			entradas[i] = NULL;
		}
		free(entradas);
		entradas = NULL;
	}

	sprintf(buffnumerico,"%d",nArticulo);
	strcat(direccionGrupo,"/");
	strcat(direccionGrupo,buffnumerico);
	fp = fopen(direccionGrupo,"w");
	fprintf(fp, "NEWSGROUPS: %s.%s\n",
		entrada[0],
		entrada[1]);

	fclose(fp);

	sendto(sockfd, respuesta2, sizeof(respuesta2),  
        	MSG_CONFIRM, (const struct sockaddr *) &cliaddr, len);
	n = recvfrom(sockfd, subject, MAX,  
                0, (struct sockaddr *) &cliaddr, 
                &len);
    if (n<0)
    {
        printf("no se ha recibido recv\n");
    }
	fp = fopen(direccionGrupo,"a");
	fprintf(fp,"SUBJECT: %s",subject);
	fprintf(fp,"DATE: %02d%02d%02d %02d%02d%02d %s",ptm->tm_year-100,
		ptm->tm_mon+1,
		ptm->tm_mday,
		ptm->tm_hour,
		ptm->tm_min,
		ptm->tm_sec,
		(char *) ctime(&timevar));
	fprintf(fp,"Message-ID: <%s@micasa>\n",buffnumerico);
	fclose(fp);

	sendto(sockfd, respuesta2, sizeof(respuesta2),  
        	MSG_CONFIRM, (const struct sockaddr *) &cliaddr, len);
	n = recvfrom(sockfd, body, MAX,  
                0, (struct sockaddr *) &cliaddr, 
                &len);
    if (n<0)
    {
        printf("no se ha recibido recv\n");
    }
	fp = fopen(direccionGrupo,"a");
	fprintf(fp, "%s",body);
	fclose(fp);

	sendto(sockfd, respuestaExito, sizeof(respuestaExito),  
        	MSG_CONFIRM, (const struct sockaddr *) &cliaddr, len);
}

/*
*
*
*
*/

void quitUDP(int sockfd,struct sockaddr_in cliaddr)
{
	char mensaje[]="205 adios";
	char buff[20];
	int len, n; 
  
    len = sizeof(cliaddr);

	sendto(sockfd, mensaje, sizeof(mensaje),  
        MSG_CONFIRM, (const struct sockaddr *) &cliaddr, len);


	n = recvfrom(sockfd, buff, MAX,  
        	0, (struct sockaddr *) &cliaddr, &len);
    if (n<0)
    {
        printf("no se ha recibido recv\n");
    }
}

int comprobarOrdenUDP(char *buff)
{
    if (strncmp("LIST", buff, 4) == 0|| strncmp("list", buff, 4) == 0)
        {	
        	printf("%s\n", buff);
            return 0;
        }
        else if (strncmp("NEWGROUPS", buff, 9) == 0|| strncmp("newgroups", buff, 9) == 0)
        {

            return 1;
        }
        else if (strncmp("NEWNEWS", buff, 4) == 0|| strncmp("newnews", buff, 4) == 0)
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
