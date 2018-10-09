#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<pthread.h>
#include<unistd.h>
#include <netdb.h>
#include <sys/socket.h>
#include <errno.h>
#include <ctype.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <netinet/in.h>
#include <sys/select.h>
#include <sys/time.h>
#include <time.h>
#include <dirent.h>
#include "queue.h"
#define _GNU_SOURCE
#define MAXINPUT 100
#define POOL_SIZE 6
#define CHUNK 1024 /* read 1024 bytes at a time */

char root_directory[50];
int pages=0,bytes=0;
pthread_mutex_t mtx;
pthread_cond_t cond_nonempty;
pthread_cond_t cond_nonfull;
struct Queue q;
int obtain(struct Queue *q){
  pthread_mutex_lock(&mtx);
  while(q->size <= 0){
    //printf(">>Found Buffer Empty\n");
    pthread_cond_wait(&cond_nonempty,&mtx);
  }
  int data=pop(q);
  pthread_mutex_unlock(&mtx);
  return data;
}
void place(struct Queue *q,int data){
  pthread_mutex_lock(&mtx);
  push(q,data);
  pthread_mutex_unlock(&mtx);
}

void* thread_f(void *arg)
{
     //<-------consumer------->//
     int sock,valread;
     char buffer[1024] = {0};
     struct tm *local;
     time_t t;
     char token[50],*token1;
     char **date = (char **)malloc(sizeof(char*)*10000);
     t = time(NULL);
     local = localtime(&t);
     strcpy(token,asctime(local));
     token1=strtok(token," ");
     //apothikeuw se enan pinaka ta stoixeia ths hmeromhnias/wras//
     int count=0;
     while(token1!=NULL){
       date[count]=token1;
       token1=strtok(NULL," \n\0");
       count++;
     }
     while(1){
       sock=obtain(&q);
       char * content = 0;
       char *OK;
       char *copy;
       char *full_path;
       char *full_path2,*full_path3;
       if( sock!=-1){
         valread=read(sock,buffer,1024);
         printf("%s\n",buffer );
         char path[50],*str_send;
         char *str,*site,*page,*last;
         copy=malloc(strlen(buffer));
         strcpy(copy,buffer);
         str=strtok_r(copy," \t",&str_send);
         str=strtok_r(NULL," \t",&str_send);
         full_path=malloc(strlen(str)+strlen(root_directory)+1);
         sprintf(full_path,"%s%s",root_directory,str);
         if(access(full_path, F_OK)==0){
           if(access(full_path,R_OK)==0){
             printf("exists\n");
             //*********diavazw to periexomeno tou page*********
             long  length;
             FILE * f = fopen (full_path, "rb");
             if (f)
             {
               fseek (f, 0, SEEK_END);
               length = ftell (f);
               fseek (f, 0, SEEK_SET);

               content = malloc (length+1);
               if (content)
               {
                 fread (content, 1, length, f);

               }
               content[length]='\0';
               fclose (f);
             }
             //***********************************************
             //*************creating OK message*************
           OK=malloc(length+1000);
           strcat(OK,"HTTP/1.1 200 OK\r\nDate: ");
           char day[strlen(date[0])+1],mikos[100];
           sprintf(mikos,"%ld",strlen(content));
           sprintf(day,"%s,",date[0]);
           strcat(OK,day);
           strcat(OK," ");
           strcat(OK,date[2]);
           strcat(OK," ");
           strcat(OK,date[1]);
           strcat(OK," ");
           strcat(OK,date[4]);
           strcat(OK," ");
           strcat(OK,date[3]);
           strcat(OK," GMT\r\n");
           strcat(OK,"Server: myhttpd/1.0.0 (Ubuntu64)\r\n");
           strcat(OK,"Content-Length: ");
           strcat(OK,mikos);
           strcat(OK,"\r\nContent-Type: text/html\r\nConnection: Closed\r\n\n");
           strcat(OK,content);
           pages++;
           bytes=bytes+length;
           //************************************************
            write(sock,OK,strlen(OK));

           }
           else{
             printf("no permission");
             char forbidden[200];
             char day[strlen(date[0])+1],mikos[100];
             strcat(forbidden,"HTTP/1.1 403 Forbidden\r\nDate: ");
             sprintf(day,"%s,",date[0]);
             strcat(forbidden,day);
             strcat(forbidden," ");
             strcat(forbidden,date[2]);
             strcat(forbidden," ");
             strcat(forbidden,date[1]);
             strcat(forbidden," ");
             strcat(forbidden,date[4]);
             strcat(forbidden," ");
             strcat(forbidden,date[3]);
             strcat(forbidden," GMT\r\n");
             strcat(forbidden,"Server: myhttpd/1.0.0 (Ubuntu64)\r\n");
             strcat(forbidden,"Content-Length: 124");
             strcat(forbidden,"\r\nContent-Type: text/html\r\nConnection: Closed\r\n\n");
             write(sock,forbidden,strlen(forbidden));
             char message_forbit[124];
             memset(message_forbit,124,'\0');
             strcpy(message_forbit,"<html>Trying to access this file but don't think i can make it.</html>");
             write(sock,message_forbit,124);
             printf("%s\n",message_forbit);
           }
         }
         else{
             printf("not found\n");
             //*******************NOT FOUND MESSAGE*******************
             char not_found[256];
             char day[strlen(date[0])+1],mikos[100];
             strcat(not_found,"HTTP/1.1 404 Not Found\r\nDate: ");
             sprintf(day,"%s,",date[0]);
             strcat(not_found,day);
             strcat(not_found," ");
             strcat(not_found,date[2]);
             strcat(not_found," ");
             strcat(not_found,date[1]);
             strcat(not_found," ");
             strcat(not_found,date[4]);
             strcat(not_found," ");
             strcat(not_found,date[3]);
             strcat(not_found," GMT\r\n");
             strcat(not_found,"Server: myhttpd/1.0.0 (Ubuntu64)\r\n");
             strcat(not_found,"Content-Length: 124");
             strcat(not_found,"\r\nContent-Type: text/html\r\nConnection: Closed\r\n\n");
             write(sock,not_found,strlen(not_found));
             char message[124];
             memset(message,124,'\0');
             strcpy(message,"<html>Sorry dude, couldn't find this file.</html>");
             write(sock,message,124);
             printf("%s\n",not_found);
           }
           close(sock);
       }
       else{

         free(copy);
         free(date);
         free(content);
         free(OK);
         pthread_exit(0);
       }
     }
}

int main(int argc,char*argv[]){
  pthread_mutex_init(&mtx,0);
  pthread_cond_init(&cond_nonempty,0);
	init(&q);
  time_t start, end;
  time(&start);
  struct tm *local;
  time_t t;
  char token[50],*token1;
  char **date = (char **)malloc(sizeof(char*)*10000);
  t = time(NULL);
  local = localtime(&t);
  strcpy(token,asctime(local));
  token1=strtok(token," ");
  int count=0;
  while(token1!=NULL){
    date[count]=token1;
    token1=strtok(NULL," \n\0");
    count++;
  }

  //<----------------Checking if arguments are valid------------------------->//
  char input1[MAXINPUT] = "";
  char input2[MAXINPUT] = "";
  char input3[MAXINPUT] = "";
  int length1,length2,length3,i;
  strcpy(input1,argv[2]);
  strcpy(input2,argv[4]);
  strcpy(input3,argv[6]);
  length1 = strlen (input1);
  length2 = strlen (input2);
  length3 = strlen (input3);
  for (i=0;i<length1; i++){
      if (!isdigit(input1[i]))
      {
          printf ("Entered input for service port is not a number\n");
          exit(1);
      }
    }
  for (i=0;i<length2; i++){
      if (!isdigit(input2[i]))
      {
          printf ("Entered input for command port is not a number\n");
          exit(1);
      }
    }
  for (i=0;i<length3; i++){
      if (!isdigit(input3[i]))
      {
          printf ("Entered input for number of threads is not a number\n");
          exit(1);
      }
    }
    printf ("Given inputs are numbers\n");
    struct stat buff;
    stat(argv[8],&buff);
    if(S_ISDIR(buff.st_mode))
      printf(" Its a directoy\n");
    else
      printf("Its not a directory\n");
  //<-------------------------------------------------------------------->//


  //<-------------------------------------------------------------------->//
  strcpy(root_directory,argv[8]);
  printf("to directory einai:%s\n",root_directory);

  //<---------------------- CREATING THREADS------------------------------->//
  int err,serving_port,command_port;
  pthread_t thread_array[atoi(argv[6])];
  for(i=0;i<atoi(argv[6]);i++){
          err = pthread_create(&thread_array[i], 0, thread_f,NULL);
          if (err != 0)
              printf("can't create thread :[%d]\n", i);
          else
              printf("Thread created successfully with id:%ld\n",thread_array[i]);
   }
   //<-------------------------------------------------------------------->//

    //<---------------------- SERVE SOCKET CONNECTION -------------------------------->//

    serving_port=atoi(argv[2]);
    command_port=atoi(argv[4]);
    int new_socket, valread;
    char buffer[1024] = {0};
    int opt = 1;
    struct sockaddr_in hints;
    socklen_t addrlen = sizeof(hints);
    //<---------------------- CREATING SOCKETS----------------------------->//
    int socket(int domain,int type,int protocol),sock;
    // Creating socket file descriptor
    if((sock=socket(AF_INET,SOCK_STREAM,0))==-0){
      perror("Socket creation failed\n");
    }
    hints.sin_family = AF_INET;
    hints.sin_addr.s_addr = INADDR_ANY;
    hints.sin_port = htons( serving_port );
    // Forcefully attaching socket to the port 8080
    if (setsockopt(sock, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT,
                                                  &opt, sizeof(opt)))
    {
        perror("setsockopt");
        exit(EXIT_FAILURE);
    }
    // Forcefully attaching socket to the port 8080
    if (bind(sock, (struct sockaddr *)&hints,
                                 sizeof( hints))<0)
    {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }
    if (listen(sock, 3) < 0)
    {
        perror("listen");
        exit(EXIT_FAILURE);
    }
    //<---------------------- COMMAND SOCKET CONNECTION -------------------------------->//
    int new_socket1, valread1;
    char buffer1[1024] = {0};
    int opt1 = 1;
    struct sockaddr_in hints1;
    socklen_t addrlen1 = sizeof(hints1);
    //<---------------------- CREATING SOCKETS----------------------------->//
    int socket(int domain,int type,int protocol),sock1;
    // Creating socket file descriptor
    if((sock1=socket(AF_INET,SOCK_STREAM,0))==-0){
      perror("Socket creation failed\n");
    }
    hints1.sin_family = AF_INET;
    hints1.sin_addr.s_addr = INADDR_ANY;
    hints1.sin_port = htons( command_port );
    // Forcefully attaching socket to the port 8080
    if (setsockopt(sock1, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT,
                                                  &opt1, sizeof(opt1)))
    {
        perror("setsockopt");
        exit(EXIT_FAILURE);
    }
    // Forcefully attaching socket to the port 8080
    if (bind(sock1, (struct sockaddr *)&hints1,
                                 sizeof( hints1))<0)
    {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }
    if (listen(sock1, 3) < 0)
    {
        perror("listen");
        exit(EXIT_FAILURE);
    }
    //<-------------------------------------------------------------------->//
    int new_sock,new_sock1;
    fd_set read_fds;
    while(1){
      FD_ZERO(&read_fds);
      FD_SET(sock,&read_fds);
      FD_SET(sock1,&read_fds);
      int ret=select(sock1+1,&read_fds,NULL,NULL,NULL);
      switch(ret){
        case(-1):
            printf("Failed \n");
            break;
        case(0):
            printf("Timeout \n");
            break;
        default:
            if(FD_ISSET(sock,&read_fds)){
              new_sock=accept(sock, (struct sockaddr *)&hints,(socklen_t*)&addrlen);
              if(new_sock<0){
                perror("accept");
                exit(EXIT_FAILURE);
                }
                place(&q,new_sock);
                pthread_cond_signal(&cond_nonempty);
            }
            if(FD_ISSET(sock1,&read_fds)){
              new_sock1=accept(sock1, (struct sockaddr *)&hints1,(socklen_t*)&addrlen1);
              valread1 = read( new_sock1 , buffer1, 1024);
              printf("%s\n",buffer1 );
              char steile[100];
              size_t size_stats=strlen("STATS");
              size_t size_shutdown=strlen("SHUTDOWN");
              if(strncmp(buffer1,"STATS",size_stats)==0){
                time(&end);
                time_t hours,mins,secs;
                hours=(end-start)/3600;
                mins=((end-start)%3600)/60;
                secs=((end-start)%3600)%60;
                sprintf(steile,"%s%ld%s%ld%s%ld%s%d%s%d%s\n","Server up for : " ,hours, ":" , mins ,".",secs,", served ",pages," pages, ",bytes," bytes");
                write(new_sock1,steile,strlen(steile));
                FD_CLR(sock1,&read_fds);
                close(new_sock1);
              }
              if(strncmp(buffer1,"SHUTDOWN",size_shutdown)==0){
                for (i=0;i<atoi(argv[6]);i++)
                {
                  place(&q,-1);
                  pthread_cond_signal(&cond_nonempty);
                }
                //char * temp;

                for (i=0;i<atoi(argv[6]);i++)
                {
                  int threads;
                  threads=pthread_join(thread_array[i],NULL);
                  /*if(threads==0){
                    printf("swsto join\n");
                  }
                  else{
                    printf("join failed\n");
                  }*/
                }
                free(date);
                pthread_cond_destroy(&cond_nonempty);
                pthread_mutex_destroy(&mtx);
                return 0;

              }
            }
      }
    }
		return 0;
}
