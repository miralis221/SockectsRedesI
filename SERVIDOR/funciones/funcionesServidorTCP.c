/*
** Fichero: funcionesServidorTCP.c
** Autores:
** Erick José Mercado Hernández DNI 43840208T
** Javier Merchán Benito DNI 70923644Q
*/

#include "funcionesServidorTCP.h"
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


#define MAX 256
#define FICHEROLOG "nntp.log"



void list(int sockfd, char *recuperado)
{

	char mensaje[] = "215 listado de los grupos: \n";
	int fd;
	char bufferFichero;
	char buffer[MAX];
	//char recuperado[MAX];
	int i;

	
	bzero(buffer,MAX);
	char buffs[10];
	fd = open("noticias/grupos",O_RDONLY);
    if (fd == -1)
   	{
        puts("archivo no existe\n");
    }
 	else
    {
    	sprintf(recuperado,"Servidor: %s",mensaje);
		send(sockfd,mensaje,strlen(mensaje),0);
		recv(sockfd,buffs,sizeof(buffs),0);
		i =0 ;
		while(read(fd,&bufferFichero,sizeof(bufferFichero))!=0)
		{
			buffer[i] = bufferFichero;
			i++;

		}
		close(fd);
		send(sockfd,buffer,strlen(buffer),0);
		sprintf(recuperado,"%s\n%s",mensaje,buffer);
	}
	

}//End of list
/*
*
*
*/
void newsGroups(int sockfd, char *buff, char *recuperado)
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
    int encontrado = 0;
    char buffs[10];
    char punto[] = ".";
    char recibo[10];
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

		send(sockfd,mensaje,strlen(mensaje),0);
		recv(sockfd,buffs,sizeof(buffs),0);
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
				encontrado = 1;
				if(atoi(cadena[1])<=fechaN && atoi(cadena[2])<= atoi(fecha[l+1]))
				{
					strcat(grupos,fecha[l-3]);
					strcat(grupos,"\n");
				}
			}
		}
		if (encontrado==1)
		{
			send(sockfd,grupos,strlen(grupos),0);
			recv(sockfd,recibo,sizeof(recibo),0);
			send(sockfd,punto,strlen(punto),0);	
		}
		else
		{
			send(sockfd,punto,strlen(punto),0);
		}
		
	} 

}// End of newsgroups

/*
*
*
*
*/

void newNew (int sockfd,char *buff,char *recuperado)
{
	char bufferFichero;
	char lecturaFichero[MAX];
    char mensaje[] = "230 Nuevos Articulos desde";
    char ArticulosNuevos[20];
    char ArticulosAntiguos[20];
    char cadena[MAX][MAX];
    char lineasDelFicheroGrupos[MAX][MAX];
    char lineasDelFicherosArticulos[MAX][MAX];
    char nombreGrupo[MAX][MAX];
    char direccionFinal[MAX];
    char cadena2[MAX];
    char cadena3[3][20];
    char num[10];
    char *token;
    int nArticulos;
    int i, j, k, l;
    int fd;
    char carpeta[20];
    FILE *fp;	
    ssize_t line_size;
    char *line_buf = NULL;
    size_t line_buf_size = 0;
    char buffs[20];

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
		send(sockfd,mensaje,strlen(mensaje),0);
		recv(sockfd,buffs,sizeof(buffs),0);

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
				send(sockfd,lineasDelFicherosArticulos,
					sizeof(lineasDelFicherosArticulos),0);
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

void group (int sockfd,char *buff,char *recuperado)
{
	char bufferFichero;
    char buffer[MAX];
    char mensaje[] = "211 numeros de articulos primero y ultimo de: ";
    char respuesta[MAX];
    char cadena[MAX][MAX];
    char matriz[MAX][MAX];
    char grupo[MAX][MAX];
    char *token;
    int nArticulos;
    int limites;
    int i, j, k, l;
    int fd;
    char buffs[20];

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
		send(sockfd,mensaje,strlen(mensaje),0);
		recv(sockfd,buffs,sizeof(buffs),0);

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
			//printf("%s\n",grupo[l] );
			if (strncmp(cadena[1],grupo[l],9)==0)
			{
				nArticulos = atoi(grupo[l+1])-atoi(grupo[l+2])+1;
				sprintf(respuesta,"Numero de articulos %d ",nArticulos);
				strcat(respuesta," Ultimo ");
				strcat(respuesta,grupo[l+1]);
				strcat(respuesta," primero: ");
				strcat(respuesta,grupo[l+2]);
				send(sockfd,respuesta,strlen(respuesta),0);
				break;
			}
		}
		recv(sockfd,buffs,sizeof(buffs),0);
		send(sockfd,cadena[1],strlen(cadena[1]),0); 
		sprintf(recuperado,"%s\n%s",mensaje,respuesta);
	} 
}

void article (int sockfd,char *buff, char *recuperado)
{
	FILE *fp5;
	char respuesta1[] = "223 articulo recuperado\n";
	char respuestaFallo[] = "423 no existe este articulo en el grupo de noticias\n";
	char rutaDelArticulo[MAX];
	bzero(rutaDelArticulo,MAX);
	int j;
	ssize_t line_size;
    char *line_buf = NULL;
    size_t line_buf_size = 0;
    char lineasDelFicherosArticulos[80][256];


	recv(sockfd,rutaDelArticulo,MAX,0);
	printf("%s\n",rutaDelArticulo );
	fp5 = fopen(rutaDelArticulo,"r");
	//fp5 = fopen("noticias/articulos/local/redes/1","r");
	if (fp5 == NULL)
	{
		send(sockfd,respuestaFallo,strlen(respuestaFallo),0);
		sprintf(recuperado,"%s\n",respuestaFallo);
	}
	else
	{
		//printf("lo encontre\n");
		send(sockfd,respuesta1,strlen(respuesta1),0);
		j = 0;
		line_size = getline(&line_buf,&line_buf_size,fp5);
		strcpy(lineasDelFicherosArticulos[j],line_buf);
		while(line_size >= 0)
		{
			j++;
			line_size = getline(&line_buf,&line_buf_size,fp5);
			strcpy(lineasDelFicherosArticulos[j],line_buf);
			
		}
		fclose(fp5);
		send(sockfd,lineasDelFicherosArticulos,
			sizeof(lineasDelFicherosArticulos),0);
		sprintf(recuperado,"%s\n%s",respuesta1,
			lineasDelFicherosArticulos[0]);

	}
	

}

void head (int sockfd,char *buff,char* recuperado)
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
    char lineasDelFicherosArticulos[MAX][MAX];

	//send(sockfd,respuesta1,strlen(respuesta1),0);

	recv(sockfd,rutaDelArticulo,MAX,0);
	printf("%s\n",rutaDelArticulo );
	fp = fopen(rutaDelArticulo,"r");
	if (fp == NULL)
	{
		send(sockfd,respuestaFallo,strlen(respuestaFallo),0);
		//printf("no lo encontre\n");
		sprintf(recuperado,"%s\n",respuestaFallo);
	}
	else
	{
		printf("lo encontre\n");
		send(sockfd,respuesta1,strlen(respuesta1),0);
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
		send(sockfd,lineasDelFicherosArticulos,
			sizeof(lineasDelFicherosArticulos),0);
	}
}

void body(int sockfd, char *buff,char *recuperado)
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

	//send(sockfd,respuesta1,strlen(respuesta1),0);

	recv(sockfd,rutaDelArticulo,MAX,0);
	printf("%s\n",rutaDelArticulo );
	fp = fopen(rutaDelArticulo,"r");
	if (fp == NULL)
	{
		send(sockfd,respuestaFallo,strlen(respuestaFallo),0);
		//printf("no lo encontre\n");
		sprintf(recuperado,"%s\n",respuestaFallo);
	}
	else
	{
		//printf("lo encontre\n");
		send(sockfd,respuesta1,strlen(respuesta1),0);
		j = 0;
		i = 0;
		line_size = getline(&line_buf,&line_buf_size,fp);
		//strcpy(lineasDelFicherosArticulos[j],line_buf);
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
		send(sockfd,lineasDelFicherosArticulos,
			sizeof(lineasDelFicherosArticulos),0);

	}
	

}

void post(int sockfd,char *buff,char *recuperado)
{
	char respuesta1[] = "340 Subiendo un articulo; finalize con una linea que solo contenga un .\n";
	char respuesta2[] = "Carpeta Creada\n";
	char respuestaExito[] = "240 Articulo Recibido correctemente";
	char respuestaFallo[] = "441 No se ha podido subir el artículo";
	char grupo[MAX];
	char subject[MAX];
	char date[MAX];
	char body[MAX];
	char entrada[80][MAX];
	char carpetas[MAX][MAX];
	char *token;
	char *entrada2;
	char direccionGrupo[MAX*3];
	char direccionArticulos[]="noticias/articulos";
	int i,j;
	FILE *fp;
	long timevar;
    time (&timevar);

    struct dirent **entradas= NULL;
    int numeroEntradas;
    int nArticulo;
    char buffnumerico[10];

    time_t rawtime = time(NULL);
    struct tm *ptm = localtime(&rawtime);

	send(sockfd, respuesta1,strlen(respuesta1),0);

	recv(sockfd,grupo,MAX,0);
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
	printf("%s\n",direccionGrupo );
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
			printf("%s\n", entradas[i]->d_name);
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
	if (fp == NULL)
	{
		send(sockfd,respuestaFallo,strlen(respuestaFallo),0);
		return;
	}	
	fprintf(fp, "NEWSGROUPS: %s.%s\n",
		entrada[0],
		entrada[1]);

	fclose(fp);

	send(sockfd,respuesta2,strlen(respuesta2),0);
	recv(sockfd,subject,MAX,0);
	fp = fopen(direccionGrupo,"a");
	if (fp == NULL)
	{
		send(sockfd,respuestaFallo,strlen(respuestaFallo),0);
		return;
	}
	fprintf(fp,"SUBJECT: %s",subject);

	fprintf(fp,"DATE: %02d%02d%02d %02d%02d%02d %s",ptm->tm_year-100,
		ptm->tm_mon+1,
		ptm->tm_mday,
		ptm->tm_hour,
		ptm->tm_min,
		ptm->tm_sec,
		(char *) ctime(&timevar));

	fprintf(fp,"Message-ID: <%s@nogal.usal.es>\n",buffnumerico);
	fclose(fp);

	send(sockfd,respuesta2,strlen(respuesta2),0);
	recv(sockfd,body,256,0);
	fp = fopen(direccionGrupo,"a");
	if (fp == NULL)
	{
		send(sockfd,respuestaFallo,strlen(respuestaFallo),0);
		return;
	}
	fprintf(fp, "%s",body);
	fclose(fp);
	fp = fopen(direccionGrupo,"r");
	if (fp == NULL)
	{
		send(sockfd,respuestaFallo,strlen(respuestaFallo),0);
		return;
	}
	else
	{
		fclose(fp);
		send(sockfd,respuestaExito,strlen(respuestaExito),0);	
	}

}

/*
*
*
*
*/

void quit(int sockfd,char *recuperado)
{
	char mensaje[]="205 adios";
	char buffs[20];
	send(sockfd,mensaje,strlen(mensaje),0);
	recv(sockfd,buffs,sizeof(buffs),0);
	sprintf(recuperado,"%s",mensaje);
	
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
        else if (strncmp("NEWNEWS", buff, 7) == 0|| strncmp("newnews", buff, 7) == 0)
        {
            return 2;
        }
        else if (strncmp("GROUP", buff, 4) == 0 || strncmp("group ", buff, 4) == 0)
        {
            return 3;
        }
        else if (strncmp("ARTICLE", buff, 6) == 0 || strncmp("article", buff, 6) == 0)
        {
            return 4;
        }
        else if (strncmp("HEAD", buff, 4) == 0 || strncmp("head", buff, 4) == 0)
        {
            return 5;
        }
        else if (strncmp("BODY ", buff, 4) == 0 || strncmp("body", buff, 4) == 0)
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
        else
        {
        	return 9;
        }
}

