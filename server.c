/*FTP server*/
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
/*for getting file size using stat()*/
#include <sys/stat.h>
/*for sendfile()*/
#include <sys/sendfile.h>
/*for O_RDONLY*/
#include <fcntl.h>

int main(int argc,char *argv[])
{
  printf("Démarrage du serveur FTP\n");
  struct sockaddr_in server, client;
  struct stat obj;
  int sock1, sock2;
  char buf[100], command[5], filename[20];
  char * file;
  file = malloc(100);
  int k, i, size, len, c;
  int filehandle;
  sock1 = socket(AF_INET, SOCK_STREAM, 0);
  printf("2\n");
  if(sock1 == -1)
    {
      printf("Echec de creation du socket");
      exit(1);
    }
  server.sin_port = htons(atoi(argv[1]));
  server.sin_addr.s_addr = INADDR_ANY;
  server.sin_family = AF_INET;
  bzero(&(server.sin_zero),8);
  k = bind(sock1,(struct sockaddr*)&server,sizeof(server));
  if(k == -1)
    {
      printf("Binding error\n");
      exit(1);
    }
  k = listen(sock1,1);
  if(k == -1)
    {
      printf("Listen failed");
      exit(1);
    }
  len = sizeof(struct sockaddr_in);
  sock2 = accept(sock1,(struct sockaddr*)&client, &len);
  printf("Nouvelle connexion > %d\n", sock2);
  FILE *fp2 = popen("pwd", "r");
  if (fgets(file, 100, fp2) != 0) {
      send(sock2, file, strlen(file),0);
  }
  i = 1;
  while(1)
    {
    	for (int i = 0; i < 100; ++i)
    	{
    		buf[i]='\0';
    	}
      recv(sock2, buf, 100, 0);
      printf("%s\n", buf);
      sscanf(buf, "%s", command);
      if(!strcmp(command, "ls"))
		{
		  FILE *fp2 = popen("ls", "r");
		  char buf[256];
		  int tail = 0;
		  while (fgets(file, 100, fp2) != 0) {
		      send(sock2, file, strlen(file),0);
		      recv(sock2, buf, 100, 0);
		  }
		  send(sock2, "#end#", sizeof(char)*5,0);
		}
      else if(!strcmp(command,"get"))
		{
		  sscanf(buf, "%s%s", filename, filename);
		  stat(filename, &obj);
		  filehandle = open(filename, O_RDONLY);
		  size = obj.st_size;
		  if(filehandle == -1)
		      size = 0;
		  send(sock2, &size, sizeof(int), 0);
		  if(size)
		  sendfile(sock2, filehandle, NULL, size);
	      
		}
      else if(!strcmp(command, "put"))
        {
		  int c = 0, len;
		  char *f;
		  sscanf(buf+strlen(command), "%s", filename);
		  recv(sock2, &size, sizeof(int), 0);
		  i = 1;
		  while(1)
		    {
		      filehandle = open(filename, O_CREAT | O_EXCL | O_WRONLY, 0666);
		      if(filehandle == -1)
			{
			  sprintf(filename + strlen(filename), "%d", i);
			}
		      else
			break;
		    }
		  f = malloc(size);
		  recv(sock2, f, size, 0);
		  c = write(filehandle, f, size);
		  close(filehandle);
		  send(sock2, &c, sizeof(int), 0);
        }
      else if(!strcmp(command, "pwd"))
		{
		  FILE *fp2 = popen("pwd", "r");
		  if (fgets(file, 100, fp2) != 0) {
		      send(sock2, file, strlen(file),0);
		  }
		}
      else if(!strcmp(command, "cd"))
        {
        	printf("%s\n", buf+3);
          if(chdir(buf+3) == 0){
          	printf("cd > %s\n", buf);
          	FILE *fp2 = popen("pwd", "r");
		    
		    if (fgets(file, 100, fp2) != 0) {
		        send(sock2, file, strlen(file),0);
		    }else{
		    	send(sock2, "#error", sizeof(char)*6,0);
		    }
          }else{
          	printf("error > %s\n", buf);
          	send(sock2, "#error", sizeof(char)*6,0);
          }
        }
      else if(!strcmp(command, "bye") || !strcmp(command, "quit"))
		{
		  printf("FTP server quitting..\n");
		  i = 1;
		  send(sock2, &i, sizeof(int), 0);
		  exit(0);
		}
    }
  return 0;
}