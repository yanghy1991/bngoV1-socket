/*********************************************************************
 * Filename: bcast_server.c
 * Description: 广播服务器端代码
 * Author: Eric(wongpz@foxmail.com)
 * Date: 2012-9-14
 ********************************************************************/
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <fcntl.h>
#include <linux/in.h>
#include <stdlib.h>

#define PORT 9998

int main(int argc, char*argv[])
{
    int ret = -1;
    int sock;
    struct sockaddr_in server_addr; //服务器端地址
    struct sockaddr_in from_addr; //客户端地址
    int from_len = sizeof(struct sockaddr_in);
    int count = -1;
    fd_set readfd; //读文件描述符集合
    char buffer[128];
    struct timeval timeout;
    timeout.tv_sec = 2;
    timeout.tv_usec = 0;

    sock = socket(AF_INET, SOCK_STREAM, 0); //建立数据报套接字
    if (sock < 0)
    {
        perror("sock error");
        return -1;
    }

    memset((void*) &server_addr, 0, sizeof(struct sockaddr_in));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY );
    server_addr.sin_port = htons(PORT);

    //将地址结构绑定到套接字上
    ret = bind(sock, (struct sockaddr*) &server_addr, sizeof(server_addr));
    if (ret < 0)
    {
        perror("bind error");
        return -1;
    }

	//监听端口 队列长度5
	ret = listen(sock,20);
	if (ret < 0)
    {
        perror("listen error");
        return -1;
    }
	
    /**
     * 循环等待客户端
     */
    while (1)
    {
		//获取新的客户端连接
		ret = accept(sock,(struct sockaddr*)&from_addr, &from_len);
		if (ret < 0)
		{
			perror("accept error");
			return -1;
		} 
		printf("客户端成功连接\n"); 
		while(1){
			    timeout.tv_sec = 100;
				timeout.tv_usec = 0;

				//文件描述符集合清0
				FD_ZERO(&readfd);

				//将套接字描述符加入到文件描述符集合
				FD_SET(sock, &readfd);

				//select侦听是否有数据到来
				ret = select(sock + 1, &readfd, NULL, NULL, &timeout); //侦听是否可读

				switch (ret)
				{
				case -1: //发生错误
					perror("select error:");
					break;
				case 0: //超时
					printf("select timeout\n");
					break;
				default:
					if (FD_ISSET(sock,&readfd))
					{	
						memset(buffer,0,128);
						count = recv(sock, buffer, 128, 0); //接收客户端发送的数据

							//响应客户端请求
							//打印客户端的IP地址和端口号
							printf("\nClient connection information:\n\t IP: %s, Port: %d\n msg:%s \n",
									(char *)inet_ntoa(from_addr.sin_addr),
									ntohs(from_addr.sin_port),buffer);

							strcpy(buffer,"hello client.");
							count = send(sock, buffer, strlen(buffer), 0); 
					}
					break;
				}
			
		}
		
    }
    return 0;
}