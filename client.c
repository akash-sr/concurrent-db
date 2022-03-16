#include "header.h"


int main(int argc, char* argv[]){
  if(argc!=3){
    printf("Usage: ./client <server ip> <server port>\n");
    exit(0);
  }
  int sockfd;
  struct sockaddr_in servaddr;

  char inputbuf[MAX_INPUT_SIZE];

  sockfd = socket(AF_INET, SOCK_STREAM, 0);
  bzero(&servaddr, sizeof(servaddr));
  servaddr.sin_family = AF_INET;
  servaddr.sin_port = htons(atoi(argv[2]));
  if(!inet_aton(argv[1], &servaddr.sin_addr)){
  	fprintf(stderr, "ERROR invalid server IP address\n");
  	exit(1);
  }

  connect(sockfd, (struct sockaddr*) &servaddr, sizeof(servaddr));

  do{
    printf("Enter request: ");
    bzero(inputbuf,MAX_INPUT_SIZE);
    fgets(inputbuf,MAX_INPUT_SIZE-1,stdin);
  	if (write(sockfd,inputbuf,strlen(inputbuf))< 0){
	    fprintf(stderr, "ERROR writing to socket\n");
	    exit(1);
  	}
  	bzero(inputbuf,MAX_INPUT_SIZE);
  	if(read(sockfd,inputbuf,(MAX_INPUT_SIZE-1)) < 0){
	    fprintf(stderr, "ERROR reading from socket\n");
	    exit(1);
  	}
    if(inputbuf[strlen(inputbuf)-1]=='\n')
      inputbuf[strlen(inputbuf)-1] = '\0';
    // printf("%s %d\n", inputbuf, strlen(inputbuf));
  	printf("Server replied: %s\n",inputbuf);
  }while(strcmp(inputbuf, exitResponse));

  close(sockfd);
  return 0;
}
