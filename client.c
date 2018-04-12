#include<sys/ioctl.h>
#include<arpa/inet.h>
#include<stdlib.h>
#include<net/if_arp.h>
#include<sys/stat.h>
#include<fcntl.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<netdb.h>
#include<string.h>
#include <stdio.h>

#define FNAME file1
#define PORT 8010
#define BUFSIZE 1460

int sockfd, fd, n, size,count=0,act;
	long int size1,size2;
	char buf[BUFSIZE], fname[50];
	struct sockaddr_in servaddr;
struct stat stat_buf; 


void getFile(char *filename){

char request_msg[50];
strcpy(request_msg,"GET ");
strcat(request_msg,filename);

send(sockfd,request_msg,sizeof(request_msg),0);

		
	fd=open(filename,O_WRONLY|O_CREAT,S_IRWXU);		
	while ( (n = read(sockfd, buf, BUFSIZE-1)) > 0)
	{
		buf[n] = '\0';
		//printf("%s\n",buf);	
		write(fd,buf,n);
		if( n < BUFSIZE-1)
			break;
	}
	printf("file receiving completed \n");
return;
}

void sendFile(char *filename){
	char request_msg[50];
	strcpy(request_msg,"PUT ");
	strcat(request_msg,filename);
	send(sockfd,request_msg,sizeof(request_msg),0);
    printf("File name is %s :  ", filename);  

			fd=open(filename,O_RDONLY,S_IRUSR);
			fstat(fd, &stat_buf);
			size = stat_buf.st_size;
				
			printf(" size is %d\n", size); 
			

			printf("\nopened file\n");
			while ( (n = read(fd, buf, BUFSIZE-1)) > 0) 
			{
				buf[n] = '\0';
				//printf("%s\n",buf);
				write(sockfd,buf,n);
			}
			printf("file transfer completed \n");
	
return;
}

void getList(){
	char request_msg[50];
	strcpy(request_msg,"LIST ");
	send(sockfd,request_msg,sizeof(request_msg),0);

	fd=open("temp.txt",O_WRONLY|O_CREAT,S_IRWXU);		
	while ( (n = read(sockfd, buf, BUFSIZE-1)) > 0)
	{
		buf[n] = '\0';
		//printf("%s\n",buf);	
		write(fd,buf,n);
		if( n < BUFSIZE-1)
			break;
	}
	system("cat temp.txt");
	system("rm temp.txt");
	return;
}

int main(int argc, char **argv)
{
	

	if (argc != 2) { 
	printf("Usage: %s server_address", argv[0]); 
	exit(1);
	}

	printf("11\n");
	if ( (sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
		exit(1);

	bzero(&servaddr, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_port   = htons(PORT);	
	if (inet_pton(AF_INET, argv[1], &servaddr.sin_addr) <= 0)
		exit(1);

	printf("22\n");

	if (connect(sockfd, (struct sockaddr*) &servaddr, sizeof(servaddr)) < 0)
		exit(1);
	printf("connection established\n");


        printf("enter 1 to GET file\n");
        printf("enter 2 to PUT file\n");
        printf("enter 3 to list the files\n");
        scanf("%d", &act);

   
    switch(act){
	case 1:
	printf("Enter the name of the file u want to GET : ");
	scanf("%s",fname);
	getFile(fname);
	break;

	case 2:
	printf("Enter the name of the file u want to PUT : ");
	scanf("%s",fname);
	sendFile(fname);
	break;

	case 3:
        getList();
	break;
	default:
	break;
	
	}
	
	
	close(sockfd);
	close(fd);
	exit(0);
}