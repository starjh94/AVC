#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <pthread.h>

#include <sys/stat.h>
#include <fcntl.h>
#include <sys/types.h>
#include <netinet/in.h>
#define BUF_SIZE 100
#define NAME_SIZE 10
#define MAXLINE 127

void error_handling(char * msg);
void menue();
char tmp_tmp[400];
char name[10];
char msg[BUF_SIZE];

void zip(char *tmp_cod2)
{
    
    char zip_cod[200];
    char tmp_cod1[] = "zip backup.zip ";
    char tmp[200];
    char tmp_cod3[] ="\*";
    char tmp_bname[] ="\backup.zip";
    sprintf(zip_cod,"%s%s%s", tmp_cod1,tmp_cod2,tmp_cod3);
    
    system(zip_cod);
    sprintf(tmp_tmp,"%s%s",tmp_cod2,tmp_bname);
    printf("이름:%s",tmp_tmp);
    // return
    
}
int main(int argc, char *argv[])
{
    int sock;
    char name_msg[NAME_SIZE+BUF_SIZE];
    struct sockaddr_in serv_addr;
    int s, nbyte;
    char server_from_access_msg[20]= "access denied";
    char server_from_access_stat[20];
    char buf[MAXLINE+1];
    
    char name[10]; char user_pw[20];    char filename[20];
    
    int filesize, fp, filenamesize;
    int sread, total=0;
    int mod;
    
    if(argc!=5)
    {
        printf("Usage : %s <IP> <port> <name> <pw>\n", argv[0]);
        exit(1);
    }
    
    sprintf(name, "%s", argv[3]);
    sprintf(user_pw, "%s", argv[4]);

    sock=socket(PF_INET, SOCK_STREAM, 0);
    
    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family=AF_INET;
    serv_addr.sin_addr.s_addr=inet_addr(argv[1]);
    serv_addr.sin_port=htons(atoi(argv[2]));
    
    if(connect(sock, (struct sockaddr*)&serv_addr, sizeof(serv_addr))==-1)
        error_handling("connect() error");
    
    send(sock, name, sizeof(name),0);                                   // user name 전송
    send(sock, user_pw, sizeof(user_pw),0);
    recv(sock,server_from_access_stat,sizeof(server_from_access_stat),0);
    
    if(strcmp(server_from_access_stat,server_from_access_msg) == 0)
    {
        printf("access denied \n");
        return 0;
    }
    else
        printf("access success\n");
    printf("select backup folder path: ");
   
    if ( fgets(filename, sizeof(filename), stdin) == NULL )
        exit(0);
  
  //  zip(&filename);

    
    filenamesize = strlen(filename);
    filename[filenamesize-1] = 0;

    if( (fp = open( filename, O_RDWR,S_IRWXU|S_IRUSR)) < 0 )
    {
        printf( "open failed" );
        exit(0);
    }
    
 
                                 //user pw전송
    send(sock, filename, sizeof(filename), 0 );                         //파일이름.확장자 전송

    filesize = lseek( fp, 0, SEEK_END );
    send( sock, &filesize, sizeof(filesize), 0 );                       //file size전송
    lseek(fp, 0, SEEK_SET );
    
    while( total != filesize )                                          //file 전송 부
    {
        sread = read( fp, buf, 100 );
        printf( "file is sending now.. " );
        total += sread;
        buf[sread] = 0;
        send( sock, buf, sread, 0 );
        printf( "processing :%4.2f%% \n ", total*100 / (float)filesize );
        usleep(10000);
    }
    
    
    
    
    close(sock);
    return 0;
}
void menue()
{
    printf("you can se");
    
    
}
void error_handling(char *msg)
{
    fputs(msg, stderr);
    fputc('\n', stderr);
    exit(1);
}
