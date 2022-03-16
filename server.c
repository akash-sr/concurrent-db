#include "header.h"


int main(int argc, char* argv[]){
  if(argc!=2){
    printf("Usage: ./server <server port> \n");
    exit(0);
  }
  printf("\033[0;32m");
  printf("***New Server Started***\n");
  printf("\033[0;37m");

  int listenfd, connfd;
  pid_t childpid;
  struct sockaddr_in cliaddr, servaddr;
  socklen_t clilen;
  char inputbuf[MAX_INPUT_SIZE];

  listenfd = socket(AF_INET, SOCK_STREAM, 0);

  bzero(&servaddr, sizeof(servaddr));
  servaddr.sin_family = AF_INET;
  servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
  servaddr.sin_port = htons(atoi(argv[1]));

  bind(listenfd, (struct sockaddr*) &servaddr, sizeof(servaddr));

  listen(listenfd, LISTENQ);

  while(1){
    clilen = sizeof(cliaddr);
    connfd = accept(listenfd, (struct sockaddr*) &cliaddr, &clilen);
    char str[INET_ADDRSTRLEN];
    char* ptr = inet_ntop(AF_INET, &cliaddr.sin_addr, str, sizeof(str));
    printf("\033[0;34m");
    printf("***Connection Established with client ip: %s, port: %d***\n", str, ntohs(cliaddr.sin_port));
    printf("\033[0;37m");
    if((childpid=fork())==0){
      close(listenfd);
      bool flag = false;
      while(1){
        bzero(inputbuf,MAX_INPUT_SIZE);
      	if(read(connfd,inputbuf,(MAX_INPUT_SIZE-1)) < 0){
    	    fprintf(stderr, "ERROR reading from socket\n");
    	    exit(1);
      	}
        if(!strncmp(inputbuf, "Bye",3)){
          bzero(inputbuf,MAX_INPUT_SIZE);
          strcpy(inputbuf, "Goodbye");
          flag = true;
        }
        else{
          bzero(inputbuf,MAX_INPUT_SIZE);
          strcpy(inputbuf, "200 OK");
        }
        if (write(connfd,inputbuf,strlen(inputbuf))< 0){
    	    fprintf(stderr, "ERROR writing to socket\n");
    	    exit(1);
      	}
        if(flag)
          break;
      }
      printf("\033[0;31m");
      printf("***Connection closed with client ip: %s, port: %d***\n", str, ntohs(cliaddr.sin_port));
      printf("\033[0;37m");
      close(connfd);
      exit(0);
    }
    close(connfd);
  }
}
