
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <string.h>
#include <fcntl.h>
#include <stdio.h>
#include<stdlib.h>
#include<math.h>
#include<omp.h>
#include<time.h>


#define FNAME file1
#define PORT 8010
#define BUFSIZE 1460
#define LISTENQ 5

int	listenfd, connfd, fd, pid, n, size;
//	FILE *fp;
	struct sockaddr_in servaddr;
	char buf[BUFSIZE],fname[50],command[10],speed_string[50];

	struct stat stat_buf; 	



int storeFile(char *filename,int count_speed){
	
	char count_filename[10];
	sprintf(count_filename,"%d.txt",count_speed);
	int fd_speed;
	//printf("%");
	fd_speed=open(&count_filename,O_WRONLY|O_CREAT,S_IRWXU);
	double start_speed,end_speed;


	fd=open(filename,O_WRONLY|O_CREAT,S_IRWXU);		
	while ( (n = read(connfd, buf, BUFSIZE-1)) > 0)
	{
		start_speed=omp_get_wtime();
		buf[n] = '\0';
		//printf("%s\n",buf);	
		write(fd,buf,n);
		if( n < BUFSIZE-2)
			break;

		end_speed=omp_get_wtime();
		sprintf(speed_string,"%.10lf, %.10lf\n",start_speed,end_speed);
		write(fd_speed,speed_string,strlen(speed_string));
	}
	close(fd_speed);
	//printf("File stored\n");
	fstat(fd, &stat_buf);
	size = stat_buf.st_size;
	return size;
}

int sendFile(char *filename,int count_speed){
			char count_filename[10];
			sprintf(count_filename,"%d.txt",count_speed);
			int fd_speed;
			//printf("%");
			fd_speed=open(&count_filename,O_WRONLY|O_CREAT,S_IRWXU);
			double start_speed,end_speed;
			

			fd=open(filename,O_RDONLY,S_IRUSR);
			fstat(fd, &stat_buf);
			size = stat_buf.st_size;
				
			//printf(" size is %d\n", size); 
			
		//	write(connfd,size,sizeof(size));
		//	send(connfd,size,sizeof(size),0);                                                                       
		//	fd=open(fname,O_RDONLY,S_IRUSR);

			//printf("\nopened file\n");
			while ( (n = read(fd, buf, BUFSIZE-1)) > 0) 
			{
				start_speed=omp_get_wtime();
				buf[n] = '\0';
				//printf("%s\n",buf);
				write(connfd,buf,n);
				end_speed=omp_get_wtime();
				sprintf(speed_string,"%.10lf, %.10lf\n",start_speed,end_speed);
				write(fd_speed,speed_string,strlen(speed_string));
			}
			//printf("file transfer completed \n");
			close(fd_speed);
			close(connfd);
			close(fd);
			return size;
}
int sendlist(){
	fd=open("temp_server.txt",O_RDONLY,S_IRUSR);
	fstat(fd, &stat_buf);
	size = stat_buf.st_size;
		
	//printf(" size is %d\n", size); 
	
//	write(connfd,size,sizeof(size));
//	send(connfd,size,sizeof(size),0);                                                                       
//	fd=open(fname,O_RDONLY,S_IRUSR);

	//printf("\nopened file\n");
	while ( (n = read(fd, buf, BUFSIZE-1)) > 0) 
	{
		buf[n] = '\0';
		//printf("%s\n",buf);
		write(connfd,buf,n);
	}
	//printf("file transfer completed \n");
	close(connfd);
	fstat(fd, &stat_buf);
	size = stat_buf.st_size;
	close(fd);
	return size;
}

int main(int argc, char **argv)
{
	
	

	listenfd = socket(AF_INET, SOCK_STREAM, 0);

	bzero(&servaddr, sizeof(servaddr));
	servaddr.sin_family      = AF_INET;
	servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	servaddr.sin_port        = htons(PORT);	

	bind(listenfd, (struct sockaddr *) &servaddr, sizeof(servaddr));

	listen(listenfd, LISTENQ);
		//printf("listening\n");
		static int count=1;


	while(1)
	{
		
		connfd = accept(listenfd, (struct sockaddr *) NULL, NULL);
		//printf("connected to client: %d\n",count);
		count++;
				pid=fork();
				if(pid==0)
				{
					int sizeofdata;
					double starto,endo,startin,endin;
					starto=omp_get_wtime();
					int function;
			//printf("Handling connection request\n");
			recv(connfd,buf,50,0);
			sscanf(buf,"%s %s",command,fname);
			if(!strcmp(command,"GET")){
				//printf("want to recieve File : %s \n", fname);
				startin=omp_get_wtime();
                sizeofdata=sendFile(fname,count);
				endin=omp_get_wtime();
				function=1;
			}
		    else if(!strcmp(command,"PUT")){
				//printf("want to store file : %s \n",fname);  
				startin=omp_get_wtime();
				sizeofdata=storeFile(fname,count);
				endin=omp_get_wtime();  
				function=2;                      
			}
			else if(!strcmp(command,"LIST")){
				system("ls -I temp_server.txt > temp_server.txt");
				startin=omp_get_wtime();
				sizeofdata=sendlist();
				endin=omp_get_wtime();
				system("rm temp_server.txt");
				function=3;
			}
			close(connfd);
			close(fd);
			endo=omp_get_wtime();
			printf("%d, %d, %.10lf, %.10lf, %.10lf, %.10lf, %d\n",count,function,starto,endo,startin,endin,sizeofdata);
			exit(1);
		}
	}
	return 0;
}