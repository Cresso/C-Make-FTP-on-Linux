/*FTP Client*/
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
/*for getting file size using stat()*/
#include <sys/stat.h>
/*for sendfile()*/
/*#include <sys/sendfile.h>*/

 #include <sys/types.h>
 #include <sys/socket.h>
 #include <sys/uio.h>
/*for O_RDONLY*/
#include <fcntl.h>
int main(int argc,char *argv[])
{
  struct sockaddr_in server;
  struct stat obj;
  struct hostent *hostinfo = NULL;
  //char *hostname ;
  int sock;
  int choice;
  char buf[100], command[5], filename[20], *f;
  char * rep;
  rep = malloc(100);
  int k, size, status;
  int filehandle;
  sock = socket(AF_INET, SOCK_STREAM, 0);
  if(sock == -1)
    {
      printf("socket creation failed");
      exit(1);
    }   
  
  hostinfo = gethostbyname(argv[1]); /* on récupère les informations de l'hôte auquel on veut se connecter */
  if (hostinfo == NULL) /* l'hôte n'existe pas */
  {
      fprintf (stderr, "Unknown host %s.\n", argv[1]);
      exit(EXIT_FAILURE);
  }
  for (int i = 0; i < 100; ++i)
    {
    	buf[i]='\0';
   	}
  server.sin_family = AF_INET;
  server.sin_port = htons(atoi(argv[2]));
  server.sin_addr.s_addr = inet_addr(argv[1]);
  k = connect(sock,(struct sockaddr*)&server, sizeof(server));
  if(k == -1)
    {
      printf("Impossible de se connecter");
      exit(1);
    }
  int i = 1;
  i = recv(sock, rep, 100, 0);
  rep[i-1]='\0';
  while(1)
    {
deb:
      printf("\033[1;37m%s:%s~>%s$\033[0m",argv[1],argv[2],rep);
      for (int i = 0; i < 100; ++i)
    	{
    		buf[i]='\0';
    	}

      fgets(buf, 100, stdin);
      sscanf(buf, "%s", command);

      if(!strcmp(command, "ls")){
      	  //strcpy(buf, "ls");
	      send(sock, buf, 100, 0);
	      //printf("\033[1;37mls >\n");
	      printf("\x1B[36m");
	      while(1){
	      	size = recv(sock, buf, 100, 0);
	      	buf[size]='\0';
	      	
	      	if(!strcmp(buf, "#end#")){
	      		break;
	      	}
	      	//buf[size-1]='\0';
	      	printf("%s", buf);
	      	send(sock, "ls", 100, 0);
	      	
	      }	
	      printf("\033[0m");
      }else if (!strcmp(command, "cd"))
      {
		  sscanf(buf, "%s%s", command, filename);
		  strcpy(buf, "cd ");
		  strcpy(buf+3, filename);
	      send(sock, buf, 100, 0);
		  i = recv(sock, buf, 100, 0);
	          if(strcmp(buf, "#error"))
	          {
	          	buf[i-1]='\0';
	            //printf("Repertoire changé > \x1B[36m%s\033[0m\n", buf);
	        	strcpy(rep, buf);
	          }
	          else
	            printf("Impossible de changer au repertoire demandé!\n");
      }else if (!strcmp(command, "pwd"))
      {
		  send(sock, buf, 100, 0);
		  recv(sock, buf, 100, 0);
		  printf("\x1B[36m%s\033[0m", buf);
      }else if (!strcmp(command, "get"))
      {
		  send(sock, buf, 100, 0);
		  recv(sock, &size, sizeof(int), 0);
		  if(!size)
		    {
		      printf("Aucun fichier de ce type sur le répertoire distant\n\n");
		      goto deb;
		    }
		  f = malloc(size);
		  sscanf(buf, "%s%s", command, filename);
		  printf("Téléchargement en cour ...\n\n");
		  recv(sock, f, size, 0);
		  while(1)
		    {
		      filehandle = open(filename, O_CREAT | O_EXCL | O_WRONLY, 0666);
		      if(filehandle == -1)
				{
				  sprintf(filename + strlen(filename), "%d", i);//needed only if same directory is used for both server and client
				}
		      else break;
		    }
		  write(filehandle, f, size, 0);
		  close(filehandle);
		  printf("Téléchargement terminé.\n\n");
      }else if (!strcmp(command, "put"))
      {
	      sscanf(buf, "%s%s", command, filename);
	      stat(filename, &obj);
		  filehandle = open(filename, O_RDONLY);
	          if(filehandle == -1)
	            {
	              printf("Aucun fichier de ce type sur l'annuaire local\n\n");
	              goto deb;
	            }
		  send(sock, buf, 100, 0);
		  size = obj.st_size;
		  send(sock, &size, sizeof(int), 0);
		  sendfile(sock, filehandle, NULL, size);
		  recv(sock, &status, sizeof(int), 0);
		  if(status)
		    printf("Fichier stocké avec succès\n");
		  else
		    printf("Le fichier n'a pas pu être enregistré sur une machine distante\n");
      }else if (!strcmp(command, "bye")||!strcmp(command, "quit")||!strcmp(command, "exit"))
      {
      	  strcpy(buf, "quit");
	          send(sock, buf, 100, 0);
	          recv(sock, &status, 100, 0);
		  if(status)
		    {
		      printf("Serveur fermé\nQuitting..\n");
		      exit(0);
		    }
		    printf("Le serveur n'a pas fermé la connexion\n");
      }else if (!strcmp(command, "lls"))
      {
      	system("ls");
      }else if (!strcmp(command, "lcd"))
      {
      	system(buf+1);
      }else if (!strcmp(command, "lpwd"))
      {
      	system("pwd");
      }


    }
}