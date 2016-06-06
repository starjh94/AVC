#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <pthread.h>
#include <fcntl.h>

#define BUF_SIZE 100
#define MAX_CLNT 256

void * handle_clnt(void * arg);
void send_msg(char * msg, int len);
void error_handling(char * msg);

int clnt_cnt=0;
int clnt_socks[MAX_CLNT];
pthread_mutex_t mutx;
#define MAXLINE 127
int main(int argc, char *argv[])
{char cli_ip[20];
    int serv_sock, clnt_sock;
    struct sockaddr_in serv_adr, clnt_adr;
    int clnt_adr_sz;
    pthread_t t_id;
    if(argc!=2) {
        printf("Usage : %s <port>\n", argv[0]);
        exit(1);
    }
    
    pthread_mutex_init(&mutx, NULL);
    serv_sock=socket(PF_INET, SOCK_STREAM, 0);
    
    memset(&serv_adr, 0, sizeof(serv_adr));
    serv_adr.sin_family=AF_INET;
    serv_adr.sin_addr.s_addr=htonl(INADDR_ANY);
    serv_adr.sin_port=htons(atoi(argv[1]));
    
    if(bind(serv_sock, (struct sockaddr*) &serv_adr, sizeof(serv_adr))==-1)
    error_handling("bind() error");
    if(listen(serv_sock, 5)==-1)
    error_handling("listen() error");
    inet_ntop(AF_INET, &clnt_adr.sin_addr.s_addr, cli_ip, sizeof(cli_ip));
    printf( "IP : %s ", cli_ip );
    printf( "Port : %x \n", ntohs( clnt_adr.sin_port) );
    
    
    while(1)
    {
        clnt_adr_sz=sizeof(clnt_adr);
        clnt_sock=accept(serv_sock, (struct sockaddr*)&clnt_adr,&clnt_adr_sz);
        
        pthread_mutex_lock(&mutx);
        clnt_socks[clnt_cnt++]=clnt_sock;
        pthread_mutex_unlock(&mutx);
        
        pthread_create(&t_id, NULL, handle_clnt, (void*)&clnt_sock);
        pthread_detach(t_id);
        printf("Connected client IP: %s \n", inet_ntoa(clnt_adr.sin_addr));
    }
    close(serv_sock);
    return 0;
}

void * handle_clnt(void * arg)
{char buf[MAXLINE+1];
    
    char filename[20];
    int filesize=0;
    int total=0, sread, fp;
    int clnt_sock=*((int*)arg);
    int str_len=0, i;
    char msg[BUF_SIZE];
    bzero( filename, 20 );
    recv( clnt_sock, filename, sizeof(filename), 0 );
    printf( "%s ", filename );
    read( clnt_sock, &filesize, sizeof(filesize) );
    printf( "%d ", filesize );
    
    fp = open( filename,  O_RDWR| O_CREAT | O_TRUNC);
    pthread_mutex_lock(&mutx);
    while( total != filesize )
    {
        sread = recv( clnt_sock, buf, 100, 0 );
        printf( "file is receiving now.. " );
        total += sread;
        buf[sread] = 0;
        write( fp, buf, sread );
        bzero( buf, sizeof(buf) );
        printf( "processing : %4.2f%% \n", total*100 / (float)filesize );
        usleep(1000);
        
    }
    pthread_mutex_unlock(&mutx);
    
    pthread_mutex_lock(&mutx);
    for(i=0; i<clnt_cnt; i++)   // remove disconnected client
    {
        if(clnt_sock==clnt_socks[i])
        {
            while(i++<clnt_cnt-1)
            clnt_socks[i]=clnt_socks[i+1];
            break;
        }
    }
    clnt_cnt--;
    pthread_mutex_unlock(&mutx);
    close(clnt_sock);
    return NULL;
}
void send_msg(char * msg, int len)   // send to all
{
    int i;
    pthread_mutex_lock(&mutx);
    for(i=0; i<clnt_cnt; i++)
    write(clnt_socks[i], msg, len);
    pthread_mutex_unlock(&mutx);
}
void error_handling(char * msg)
{
    fputs(msg, stderr);
    fputc('\n', stderr);
    exit(1);
}