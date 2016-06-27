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
#include <mysql.h>
#define SOCKET int
#define DB_HOST "192.168.207.59"
#define DB_USER "root"
#define DB_PASS "apmsetup"
#define DB_NAME "board"
#define CHOP(x) x[strlen(x) - 1] = ' '
#define BUF_SIZE 100
#define MAX_CLNT 256

int       query_stat;
int user_join(void * clnt_sock);
void * handle_clnt(void * arg);
void send_msg(char * msg, int len);
void error_handling(char * msg);

int db2();
void DB_main();
int clnt_cnt=0;
int clnt_socks[MAX_CLNT];
char user_id[10];
char user_pw[20];
char filename[20];
char server_to_access_deni[]= "access denied";
char server_to_access_ok[]= "access success";
pthread_mutex_t mutx;
#define MAXLINE 127
void finish_with_error(MYSQL *conn)
{
    fprintf(stderr, "%s\n", mysql_error(conn));
    mysql_close(conn);
    exit(1);
}

int main(int argc, char *argv[])
{char cli_ip[20];
    int tmp;
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
    
    while(1)
    {
        
        clnt_adr_sz=sizeof(clnt_adr);
        clnt_sock=accept(serv_sock, (struct sockaddr*)&clnt_adr,&clnt_adr_sz);//클라이언트와의 통신을 위한 새로운 소켓 생성
        
        pthread_mutex_lock(&mutx);
        clnt_socks[tmp = clnt_cnt++]=clnt_sock;
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
    
    int filesize=0;
    int total=0, sread, fp;
    int clnt_sock=*((int*)arg);
    int str_len=0, i;
    char msg[BUF_SIZE];
    char mod[]="0";
    
    
    
    bzero( filename, 20 );

    recv(clnt_sock,user_id,sizeof(user_id),0);
    recv(clnt_sock,user_pw,sizeof(user_pw),0);
    
    if( user_join(&clnt_sock) == 0)// 거부 메세지 전송
    {
        send(clnt_sock, server_to_access_deni, sizeof(server_to_access_deni), 0 );
        return 0;
    }
    else //접속 성공
         send(clnt_sock, server_to_access_ok, sizeof(server_to_access_ok), 0 );
    
    
    recv(clnt_sock,filename,sizeof(filename),0);
    printf( "filename %s ", filename );
   
    read( clnt_sock, &filesize, sizeof(filesize) );
    printf( "%d ", filesize );
    
    
    pthread_mutex_lock(&mutx);
    fp = open( filename,  O_RDWR| O_CREAT| O_TRUNC,S_IRWXU);
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
    
    printf(" 전송 완료%s\n",filename);
      DB_main();
   
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

void DB_main()
{
    
    FILE *fp = fopen(filename, "rb");
    if( fp == NULL )
    {
        fprintf(stderr, "해당 파일을 찾을 수 없습니다.\n");
        exit(1);
    }
    
    fseek(fp, 0, SEEK_END);

    
    int flen = ftell(fp);
    int filename_size;
    int user_id_size;
    fseek(fp, 0, SEEK_SET);

    
    char data[flen + 1];
    int size = fread(data, 1, flen, fp);
    int size_st2;
    int size_st1;
    int r = fclose(fp);
    filename_size = strlen(filename);
    user_id_size = strlen(user_id);
    
    MYSQL *conn = mysql_init(NULL);
    
    if( conn == NULL )
    {
        fprintf(stderr, "mysql_init()함수 실패\n");
        exit(1);
    }
    
    if( mysql_real_connect(conn, DB_HOST, DB_USER, DB_PASS, DB_NAME, 0, NULL, 0) == NULL )
    {
        finish_with_error(conn);
    }
    char chunk[2 * size + 1];
    mysql_real_escape_string(conn, chunk, data, size);
    char st1[] = "INSERT INTO ";
    size_st1 = strlen(st1);
    char st2[] = "(filename,up_user,data) VALUES('%s','%s', '%s')";
    char stt[500];
    sprintf(stt,"%s%s%s",st1,user_id,st2);

 
    size_st2 = strlen(stt);
    char query[size_st2 + 2 * size + 1 + user_id_size  + filename_size ];
    int len = snprintf(query, 2 * size + 1 + user_id_size + filename_size + size_st2, stt,filename,user_id,chunk);
  
    if( mysql_real_query(conn, query, len) )
    {
        finish_with_error(conn);
    }
    mysql_close(conn);
   


    
    
}
int user_join(void * clnt_sock)//
{
    MYSQL       *connection=NULL, conn;
    MYSQL_RES   *sql_result;
    MYSQL_ROW   sql_row;
    char stat_deni[20] = "access denied";
    char stat_ok[20] = "access success";
    char buf1[100];
    
    mysql_init(&conn);
    
    connection = mysql_real_connect(&conn, DB_HOST, DB_USER, DB_PASS, DB_NAME, 3306, (char*)NULL, 0);
    
    if(connection==NULL)
    {
        fprintf(stderr, "Mysql connection error : %s", mysql_error(&conn));
        return EXIT_FAILURE;
    }
    
    else
        printf("Mysql connected\n");
    
    
   
    query_stat = mysql_query(connection,"SELECT *FROM user_data ");
    
    if(query_stat != 0)
    {
        fprintf(stderr, "Mysql query error : %s\n", mysql_error(&conn));
        return EXIT_FAILURE;
    }
    
    sql_result = mysql_store_result(connection);
    
    if(sql_result == NULL)
    {
        fprintf(stderr, "result is null\n");
        return EXIT_FAILURE;
    }
    
    while(1)
    {
        if((sql_row = mysql_fetch_row(sql_result)) == NULL)
        {
            printf("[%s] access denied\n",user_id);
            
            return 0;
        }
        
        if(strcmp(sql_row[0],user_id)  == 0 && strcmp(sql_row[1],user_pw) == 0)
        {
            printf("[%s] access \n",sql_row[0]);
            
            return 1;
        }
        
        else
            continue ;
        
    }
    
    
    
    
}

void error_handling(char * msg)
{
    fputs(msg, stderr);
    fputc('\n', stderr);
    exit(1);
}