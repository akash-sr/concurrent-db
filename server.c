#include "header.h"

void getRecord(char** request, char* response){
  FILE *fp = fopen("database.txt", "r");
  char record[MAX_REC_SIZE];
  // bzero(response, sizeof(response));
  bool keyFound = false;
  while(fgets(record, sizeof(record), fp)){
    // if(strlen(record)==1)
    //   continue;
    char* key = strtok(record, " ");
    char* value = strtok(NULL, " \n");
    if(!strcmp(key, request[1])){
      strcpy(response, value);
      keyFound = true;
      break;
    }
  }
  if(!keyFound){
    strcpy(response, "Key not found");
    response[13] = '\0';
  }
  fclose(fp);
}

void putRecord(char** request, char* response){
  // bzero(response, sizeof(response));
  getRecord(request, response);
  if(strcmp(response, "Key not found")){
    strcpy(response, "Key already exists");
    response[18] = '\0';
    return;
  }
  // else{
  FILE* fp = fopen("database.txt", "r+");
  char record[MAX_REC_SIZE];
  char newRecord[MAX_REC_SIZE];
  // printf("*%s\n", newRecord);
  bzero(newRecord, sizeof(newRecord));
  strcat(newRecord, request[1]);
  strcat(newRecord, " ");
  strcat(newRecord, request[2]);
  strcat(newRecord, "\n");
  // printf("**%s\n", newRecord);
  // bool keyPut = false;
  // while(fgets(record, sizeof(record), fp)){
  //   if(strlen(record)==1){
  //     fputs(newRecord, fp);
  //     keyPut = true;
  //     break;
  //   }
  // }
  fseek(fp, 0, SEEK_END);
  fputs(newRecord, fp);
  strcpy(response, "OK");
  response[2] = '\0';
  // }
  fclose(fp);
}

void delRecord(char** request, char* response){
  FILE *fpr = fopen("database.txt", "r");
  FILE *fpw = fopen("temp.txt", "w");
  char record[MAX_REC_SIZE];
  bool keyFound = false;
  while (!feof(fpr)){
    bzero(record, sizeof(record));
    fgets(record, MAX_REC_SIZE, fpr);
    if (!feof(fpr)){
      char* key = strtok(record, " ");
      char* value = strtok(NULL, " \n");

      if(!strcmp(key, request[1])){
        keyFound = true;
        continue;
      }
      fprintf(fpw, "%s %s\n", key, value);
    }
  }
  if(!keyFound){
    strcpy(response, "Key not found");
    response[13] = '\0';
  }
  else{
    strcpy(respone, "OK");
    respone[2] = '\0';
  }
  // fputc(EOF, fpw);
  fclose(fpw);
  fclose(fpr);
  remove("database.txt");
  rename("temp.txt", "database.txt");
}

void handle_request(char* inputbuf, char* outputbuf){
  int requestType = -1;
  char* request[3];

  // parse request
  int ind = 0;
  char* tok = strtok(inputbuf, " ");
  while(tok!=NULL){
    request[ind++] = tok;
    tok = strtok(NULL, " \n");
  }

  // process request
  if(!strcmp(request[0], "get")){
    return getRecord(request, outputbuf);
  }
  else if(!strcmp(request[0], "put")){
    return putRecord(request, outputbuf);
  }
  else if(!strcmp(request[0], "del")){
    return delRecord(request, outputbuf);
  }
  strcpy(outputbuf, "ERROR 404: Bad Request");
}

int main(int argc, char* argv[]){
  if(argc!=2){
    printf("Usage: ./server <server port> \n");
    exit(0);
  }
  printf("\033[0;32m");
  printf("***New Server Started***\n");
  printf("\033[0;37m");

  unlink("database.txt");
  FILE* fp = fopen("database.txt", "w");
  fclose(fp);

  int listenfd, connfd;
  pid_t childpid;
  struct sockaddr_in cliaddr, servaddr;
  socklen_t clilen;
  char inputbuf[MAX_INPUT_SIZE];
  char outputbuf[MAX_OUTPUT_SIZE];

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
        if(inputbuf[strlen(inputbuf)-1]=='\n')
          inputbuf[strlen(inputbuf)-1] = '\0';
        // printf("%s %d\n", inputbuf, strlen(inputbuf));
        if(!strcmp(inputbuf, exitTrigger)){
          bzero(outputbuf,MAX_INPUT_SIZE);
          strcpy(outputbuf, exitResponse);
          flag = true;
        }
        else{
          handle_request(inputbuf, outputbuf);
          // bzero(inputbuf,MAX_INPUT_SIZE);
          // strcpy(inputbuf, "200 OK");
        }
        if (write(connfd,outputbuf,strlen(outputbuf))< 0){
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
