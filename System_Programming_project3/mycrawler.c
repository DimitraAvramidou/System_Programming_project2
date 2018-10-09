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
#include <arpa/inet.h>
#include "queue.h"
#define _GNU_SOURCE
#define MAXINPUT 100
#define POOL_SIZE 6
#define CHUNK 1024 /* read 1024 bytes at a time */

//Kanw Global oti tha xreiastw//
char save_directory[MAXINPUT];
int PORT_SERVER;
char IP[20];
int pages=0,bytes=0;
pthread_mutex_t mtx;
pthread_cond_t cond_nonempty;
pthread_cond_t cond_nonfull;
struct Queue1 q;
struct Queue1 downloaded;
char * obtain(struct Queue1 *q){
  pthread_mutex_lock(&mtx);
  while(q->size <= 0){
    //printf(">>Found Buffer Empty\n");
    pthread_cond_wait(&cond_nonempty,&mtx);
  }
  char* link;
  link=malloc(MAXINPUT);
  link=pop1(q);
  //printf("evgala apo thn oura to %s\n",link);
  pthread_mutex_unlock(&mtx);
  return link;
}
void place(struct Queue1 *q,char *link){
  pthread_mutex_lock(&mtx);
  push1(q,link);
  //printf("evala sthn oura to %s\n",link);
  pthread_mutex_unlock(&mtx);
}
//---------------------------//


void*thread_f(void *arg)
{
  printf("hello from thread\n");
  char link[MAXINPUT];
	//---CONNECTION---///
	  struct sockaddr_in address;
      int sock = 0, valread;
      struct sockaddr_in serv_addr;
	  struct sockaddr *serverptr=(struct sockaddr*)&serv_addr;
	  struct hostent* remDetails;
      struct hostent rD;
	 char array[9000];
      //char buffer[1024] = {0};
      
      //size_t buf=0;
      if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0)
      {
          printf("\n Socket creation error \n");
          //return -1;
      }
      //memset(&serv_addr, '0', sizeof(serv_addr));
      
      
      // Convert IPv4 and IPv6 addresses from text to binary form
		int tempor1,tempor2;
      if(inet_pton(AF_INET,IP, &serv_addr.sin_addr)==0)
      {
		tempor1=gethostbyname_r(IP,&rD,array,9000,&remDetails,&tempor2);
          //printf("\nInvalid address/ Address not supported \n");
          //return -1;
      }
	  else if(inet_pton(AF_INET,IP, &serv_addr.sin_addr) !=0){
			return 0;
	  }
	 bzero((char*)&serv_addr,sizeof(serv_addr));
	 serv_addr.sin_family = AF_INET;
	 bcopy((char *)remDetails->h_addr,(char *)&serv_addr.sin_addr.s_addr,remDetails->h_length);
     serv_addr.sin_port = htons(PORT_SERVER);
      if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
      {
          printf("\nConnection Failed \n");
          //return -1;
      }



  while(1){
    strcpy(link,obtain(&q));//sthn metavliti link exw apothikeusei to link pou zitithike
	char buf[256];
    if(strcmp(link,"-1")==0){
      //frees
      pthread_exit(0);
    }
    
    char* token,*cut,*copy_link;
    copy_link=malloc(strlen(link));
    strcpy(copy_link,link);
    token=strtok_r(copy_link,"/",&cut);
    token=strtok_r(NULL,"/",&cut);
    token=strtok_r(NULL,"",&cut);
	char little_path[50];
	strcpy(little_path,token);
    //printf("to token einai:%s\n",token); //ekopsa to url pou pira apo orismata gia na parw to path
    char pszRequest[256]={0};
    char *port;
    port=malloc(6);
    sprintf(port,"%d",PORT_SERVER);
    sprintf(pszRequest, "GET /%s HTTP/1.1\r\nHost:%s\r\nUser-Agent: Mozilla /4.0\r\nAccept-Language: en-us\r\nAccept-Encoding: gzip,deflate\r\nConnection: Keep-Alive\r\n\r\n",token,port);
    write(sock , pszRequest , strlen(pszRequest));
    valread=read(sock,buf,255);//ston buf exw to mhnuma apo ton server
    buf[255]='\0';
    size_t diavasa=0;
    char *pira,*token1,*token2,*cut1,*pira_copy,*pira_copy2,*token3,*cut2,*token4,*cut4,*token_check,*token2_check,*cut2_check,*check,*cut_check;
    pira_copy2=malloc(strlen(buf));
    strcpy(pira_copy2,buf);
    token4=strtok_r(pira_copy2," ",&cut4);
    token4=strtok_r(NULL," ",&cut4);    //edw exw apothikeumeno ton kwdiko(200/404/403)
    if(strcmp(token4,"200")==0){        //elegxw an mou epestrepse selida pou uparxei
      //gia na parw to path opou tha to apothikeuw an den to exw//


        check=malloc(strlen(token)); //to token exei mesa to path pou zhtaw apo ton server
        strcpy(check,token);
        check[strlen(token)]='\0';
        char diadromi[strlen(save_directory)+strlen(token)+1];
        strcat(diadromi,save_directory);
        strcat(diadromi,"/");
        strcat(diadromi,token);
        //printf("to path pou xreiazomai einai:%s\n",diadromi);
        token2_check=strtok_r(token,"/",&cut2_check);
        char dir[strlen(save_directory)+strlen(token2_check)+1];
        sprintf(dir,"%s/%s",save_directory,token2_check);
        //printf("pira gia mkdir:%s\n",dir);
        mkdir(dir,0777);
      //////////////////////////////////////////////////////////
      pira=malloc(strlen(buf));
      strcpy(pira,buf);
      pira_copy=malloc(strlen(buf));
      strcpy(pira_copy,buf);
      token3=strtok_r(pira_copy,"\n",&cut2);
      diavasa=diavasa+strlen(token3)+1;
      token3=strtok_r(NULL,"\n",&cut2);
      diavasa=diavasa+strlen(token3)+1;
      token3=strtok_r(NULL,"\n",&cut2);
      diavasa=diavasa+strlen(token3)+1;
      token3=strtok_r(NULL,"\n",&cut2);
      diavasa=diavasa+strlen(token3)+1;
      token3=strtok_r(NULL,"\n",&cut2);
      diavasa=diavasa+strlen(token3)+1;
      token3=strtok_r(NULL,"\n",&cut2);
      diavasa=diavasa+strlen(token3)+2;
      token3=strtok_r(NULL,"",&cut2); //edw einai apothikeumeno to kommati tou content pou diavasa
      pages++;
      FILE *f = fopen(diadromi, "w");//vazw mesa sto arxeio .html to periexomeno pou diavasa mesa sta 256
      if (f == NULL)
      {
          printf("Error opening file!\n");
          //exit(1);
      }
      fprintf(f, "%s", token3);
      bytes=bytes+strlen(token3);
      token1=strtok_r(pira,"\n",&cut);
      for(int i=0;i<3;i++){
        token1=strtok_r(NULL,"\n",&cut);
      }
      token2=strtok_r(token1," ",&cut1);
      token2=strtok_r(NULL,"\n",&cut1);
      size_t megethos=atoi(token2);
      size_t upoloipo=256-diavasa;
      size_t menei=megethos-upoloipo;
      char new_buf[menei];
      int valread2=read(sock,new_buf,menei);
      //printf("%s\n",new_buf);
      fprintf(f, "%s",new_buf); //vazw mesa sto arxeio .html to upoloipo
      bytes=bytes+strlen(new_buf);


		
		char dimitra[100];
		sprintf(dimitra,"http://%s:%d/%s",IP,PORT_SERVER,little_path);
		//printf(" to dimitra einai %s\n",dimitra);
		place(&downloaded,dimitra);
		pthread_cond_signal(&cond_nonempty);
	  




      //printf("to periexomaneo einai :\n%s\n",new_buf);
      //-----vriskw ta link-------//
      char *urls,*bts,*bts_cut,*copy_urls,*antigrafh,urls_queue[100];
      urls=strstr(new_buf,"<a href=");
      int telos=0;
      while(urls!=NULL){
        copy_urls=malloc(strlen(urls)+1);
        strcpy(copy_urls,urls);
        bts=strtok_r(copy_urls,">",&bts_cut);
        antigrafh=malloc(strlen(bts)+1);
        strcpy(antigrafh,bts);
        antigrafh=strtok_r(NULL,"",&bts_cut);
        //printf("ax thee mou:%s\n",antigrafh);
        bts=strtok_r(bts,"/",&bts_cut);
        bts=strtok_r(NULL,"",&bts_cut);
        //printf("to bts einai:%s\n",bts);
        sprintf(urls_queue,"http://%s:%d/%s",IP,PORT_SERVER,bts);
        //printf("tha valw stou queue ta :%s\n",urls_queue);
        int i;
        printf("exw to url :%s\n",urls_queue);
        printf("to size ths ouras einai %d\n",q.size);
        int exist=0;
        /*if(q.size==0){
          printf("bika\n");
          place(&q,urls_queue);
        }
        else{*/
          char saved_path[50];
          int bika=0,bika1=0;
          sprintf(saved_path,"%s/%s",save_directory,bts);
          //printf("tha paw na dw  to :%s\n",urls_queue);
          struct Queue1 tempo;
          tempo=q;
          /*for(i=0;i<tempo.size;i++){
            printf("sygkrinw ta :%s me to %s\n",urls_queue,tempo.front->link);
            if(strcmp(urls_queue,tempo.front->link)==0){
              printf("einai idia\n");
              exist++;
              break;
            }
            if(tempo.size>1 && i<tempo.size-1){
              tempo.front=tempo.front->next;
              printf("\nto next einai:%s\n",tempo.front->link);

            }
          }*/


          for(i=0;i<downloaded.size;i++){
            if(strcmp(urls_queue,downloaded.front->link)==0){
              printf("to exw katevasei\n");
				//continue;
              
              	break;
            }
			else if(strcmp(urls_queue,q.front->link)==0){
					printf("to exw zitisei\n");
					break;				
		
			}								
            else{
              place(&q,urls_queue);
              pthread_cond_signal(&cond_nonempty);
              

            }
            if(downloaded.size>1 && i<(downloaded.size-1)){
              downloaded.front=(downloaded.front)->next;
              printf("\nto next einai:%s\n",downloaded.front->link);
            }
			if(q.size>1 && i<(q.size-1)){
              q.front=(q.front)->next;
              printf("\nto next einai:%s\n",q.front->link);
            }
          }
			//q.front=q.front;
			//downloaded.front=downloaded.front;
          urls=strstr(antigrafh,"<a href=");

      }

      fclose(f);

    }
    else if(strcmp(token4,"404")==0){
      printf("Page not found\n");
      continue;

    }
    else if(strcmp(token4,"403")==0){
      printf("forbidden\n");
      continue;

    }

  }


}

int main(int argc,char*argv[]){
  char link[MAXINPUT];
  time_t start, end;
  time(&start);               //ksekinaei na metraei posh wra trexei
  pthread_mutex_init(&mtx,0); //initialize mutex
  pthread_cond_init(&cond_nonempty,0);
	init1(&q);
  init1(&downloaded);                   //initialize queue
  strcpy(save_directory,argv[10]);
  strcpy(link,argv[11]);

//-------------Checking in arguments are valid--------------------//
  char input1[MAXINPUT] = "";
  char input2[MAXINPUT] = "";
  char input3[MAXINPUT] = "";
  strcpy(IP,argv[2]);
  int length1,length2,length3,i;
  strcpy(input1,argv[4]);
  strcpy(input2,argv[6]);
  strcpy(input3,argv[8]);
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
    stat(argv[10],&buff);
    if(S_ISDIR(buff.st_mode))
      printf(" Its a directoy\n");
    else
      printf("Its not a directory\n");
//---------------------------------------------------------------------//

PORT_SERVER=atoi(argv[4]);

//<---------------------- CREATING THREADS------------------------------->//
int err;
pthread_t thread_array[atoi(argv[8])];
/*for(i=0;i<atoi(argv[8]);i++){
        err = pthread_create(&thread_array[i], 0, thread_f,NULL);
        if (err != 0)
            printf("can't create thread :[%d]\n", i);
        else
            printf("Thread created successfully with id:%ld\n",thread_array[i]);
 }*/

 //<-------------------------------------------------------------------->//






  //<------------------ SOCKET CONNECTION -------------------------------->//


    char url[100];
    sprintf(url,"http://%s:%d/%s",IP,PORT_SERVER,link);
    //printf("to link pou tha paw na valw einai:%s\n",url);
    place(&q,url);
    pthread_cond_signal(&cond_nonempty);
    //place(&q,url);
    //pthread_cond_signal(&cond_nonempty);

    //<-------------------------------------------------------------------->//

    //<---------------------- COMMAND SOCKET CONNECTION -------------------------------->//
    int new_socket1, valread1,command_port;
    command_port=atoi(argv[6]);
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
    // Forcefully attaching socket to the port 
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
    fd_set read_fds;
    int new_sock1;
    while(1){
      FD_ZERO(&read_fds);
      FD_SET(sock1,&read_fds);
      if(FD_ISSET(sock1,&read_fds)){
          new_sock1=accept(sock1, (struct sockaddr *)&hints1,(socklen_t*)&addrlen1);
          valread1 = read( new_sock1 , buffer1, 1024);
          printf("%s\n",buffer1 );
          char steile[100];
          size_t size_stats=strlen("STATS");
          size_t size_shutdown=strlen("SHUTDOWN");
		  size_t size_search=strlen("SEARCH");
		  char *search,*tok_search;
		  search=malloc(strlen(buffer1)+1);
		  strcpy(search,buffer1);
		  tok_search=strtok(search," ");
          if(strncmp(buffer1,"STATS",size_stats)==0){
            time(&end);
            time_t hours,mins,secs;
            hours=(end-start)/3600;
            mins=((end-start)%3600)/60;
            secs=((end-start)%3600)%60;
            sprintf(steile,"%s%ld%s%ld%s%ld%s%d%s%d%s\n","Crawler up for : " ,hours, ":" , mins ,".",secs,", served ",pages," pages, ",bytes," bytes");
            write(new_sock1,steile,strlen(steile));
            FD_CLR(sock1,&read_fds);
            close(new_sock1);
          }
          if(strncmp(buffer1,"SHUTDOWN",size_shutdown)==0){
            for (i=0;i<atoi(argv[8]);i++)
            {
              place(&q,"-1");
              pthread_cond_signal(&cond_nonempty);
            }
            for (i=0;i<atoi(argv[8]);i++)
            {
              int threads;
              threads=pthread_join(thread_array[i],NULL);
              if(threads==0){
                printf("swsto join\n");
              }
              else{
                printf("join failed\n");
              }
            }

            pthread_cond_destroy(&cond_nonempty);
            pthread_mutex_destroy(&mtx);
            return 0;

          }
		  else if(strncmp(tok_search,"SEARCH",size_search)==0){
				tok_search=strtok(NULL,"");
				printf("i am searching for : %s\n",tok_search);
				
			}
        }
      }
		return 0;
}
