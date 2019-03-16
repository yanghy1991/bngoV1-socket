/*********************************************************************
 * Filename: bcast_client.c
 * Description:广播客户端代码
 * Author: Eric(wongpz@foxmail.com)
 * Date: 2012-9-14
 ********************************************************************/
#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<string.h>
#include<sys/socket.h>
#include<arpa/inet.h>
#include<netinet/in.h>
#include<sys/types.h>
#include<netdb.h>
#include <sys/ioctl.h>
#include <net/if.h>

#define SERVER_IP "192.168.1.141"
#define MCAST_PORT 9998

int main(int argc, char*argv[])
{
    int ret = -1;
    int sock = -1;
    int so_broadcast = 1;
    struct sockaddr_in server_addr; //广播地址

    int count = -1;
    fd_set readfd; //读文件描述符集合
    char buffer[1024];
    struct timeval timeout;
	

    //建立数据报套接字
    sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0)
    {
        perror("create socket failed:");
        return -1;
    }
	
	memset((void*) &server_addr, 0, sizeof(struct sockaddr_in));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = inet_addr(SERVER_IP);
    server_addr.sin_port = htons(MCAST_PORT);
   
   //将地址结构绑定到套接字上
    ret = connect(sock, (struct sockaddr*) &server_addr, sizeof(server_addr));
    if (ret < 0)
    {
        perror("connect error");
        return -1;
    }

    while (fgets(buffer, sizeof(buffer), stdin) != NULL)  
    {  
        printf("向服务器发送数据：%s\n",buffer);  
        send(sock, buffer, strlen(buffer),0); ///发送  
        if(strcmp(buffer,"exit\n")==0)  
            break;  
		memset(buffer,0,sizeof(buffer));
        recv(sock, buffer, sizeof(buffer),0); ///接收  
        printf("从服务器接收数据：%s\n",buffer);  
    }
    
    return 0;
}