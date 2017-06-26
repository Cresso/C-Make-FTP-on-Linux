
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
	int numOfCPU;
	FILE *fp = popen("grep -c ^processor /proc/cpuinfo", "r");

	fscanf(fp, "%d", &numOfCPU);
	pclose(fp);

	char * fi;
	FILE *fp2 = popen("pwd", "r");

	char buf[256];
	char * file;
	int tail = 0;
	file = malloc(25600);
	while (fgets(buf, sizeof(buf), fp2) != 0) {
	    //printf("%s", buf);
	    printf("%s\n", buf);
	    //strcpy(file+tail, buf);
	    //tail = tail + sizeof(buf);
	    //printf("%d\n", tail);
	    //strcpy(file+tail-3, ";");
	    //tail = tail + 1;
	    
	    //buf=malloc(100);
	}
	printf("buf = %s\n", buf);
	//printf("%s\n", file);
	
}