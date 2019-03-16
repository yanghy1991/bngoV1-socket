#include <sys/types.h>  
#include <sys/socket.h>  
#include <stdio.h>  
#include <netinet/in.h>  
#include <arpa/inet.h>  
#include <unistd.h>  
#include <string.h>  
#include <stdlib.h>  
#include <fcntl.h>  
#include <sys/shm.h>  
  
#define BUFFER_SIZE 1024  
  
int main(int argc,char *argv[])  
{  	
	if(argc != 4){
		printf("usg:[%s] [IP] [PORT] [CMD]\n",argv[0]);
		return -1;
	}
	static struct timeval tv;
	tv.tv_sec = 5;  /* 1 second Timeout */
	tv.tv_usec = 0; 
	
    ///定义sockfd  
    int sock_cli = socket(AF_INET,SOCK_STREAM, 0);  
      
    ///定义sockaddr_in  
    struct sockaddr_in servaddr;  
    memset(&servaddr, 0, sizeof(servaddr));  
    servaddr.sin_family = AF_INET;  
    servaddr.sin_port = htons(atoi(argv[2]));  ///服务器端口  
    servaddr.sin_addr.s_addr = inet_addr(argv[1]);  ///服务器ip  
      
    printf("连接%s:%d\n",argv[1],atoi(argv[2]));  
    ///连接服务器，成功返回0，错误返回-1  
	//设置socket连接超时5秒
	setsockopt(sock_cli, SOL_SOCKET, SO_SNDTIMEO, (char *)&tv,sizeof(struct timeval));
	
    if (connect(sock_cli, (struct sockaddr *)&servaddr, sizeof(servaddr)) < 0)  
    {  
        perror("connect");  
        exit(1);  
    }  
    printf("服务器连接成功\n");   
    char recvbuf[BUFFER_SIZE];  
	memset(recvbuf, 0, sizeof(recvbuf)); 
	
	//strcpy(sendbuf,"你好小乐"); 今天天气
	printf("向服务器发送数据：%s\n",argv[3]);  
	send(sock_cli, argv[3], strlen(argv[3]),0); ///发送  

	recv(sock_cli, recvbuf, sizeof(recvbuf),0); ///接收  
	printf("从服务器接收数据：%s\n",recvbuf);  
 
    close(sock_cli);  
    return 0;  
}  