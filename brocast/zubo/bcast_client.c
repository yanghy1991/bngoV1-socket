/*********************************************************************
 * Filename: bcast_client.c
 * Description:组播客户端代码
 * Author: Eric(wongpz@foxmail.com)
 * Date: 2012-9-14
 ********************************************************************/
#include <stdio.h>  
#include <string.h>  
#include <stdlib.h>  
#include <sys/types.h>  
#include <sys/socket.h>  
#include <netinet/in.h>  
#include <arpa/inet.h>  

#define TCP_FOUND "TCP_FOUND"
#define IP_FOUND "IP_FOUND"
#define IP_BIND_OK "IP_BIND_OK"
#define IP_FOUND_ACK "IP_FOUND_ACK"
#define MCAST_PORT 9999

int main(int argc, char*argv[])
{
	printf("111111111111111111\n");
    int ret = -1;
    int sock = -1;

    struct sockaddr_in broadcast_addr; //广播地址
    struct sockaddr_in from_addr; //服务端地址
    int from_len = sizeof(from_addr);
    int count = -1;
	
    fd_set readfd; //读文件描述符集合
    char buffer[1024];
    struct timeval timeout;
    timeout.tv_sec = 2; //超时时间为2秒
    timeout.tv_usec = 0;

    //建立数据报套接字
    sock = socket(AF_INET, SOCK_DGRAM, 0);
    if (sock < 0)
    {
        perror("create socket failed:");
        return -1;
    }
	printf("111111111111111111\n");
    memset(&broadcast_addr, 0, sizeof(struct sockaddr_in));
    //设置多播端口号
    broadcast_addr.sin_family = AF_INET;
    broadcast_addr.sin_port = htons(MCAST_PORT);
	inet_pton(AF_INET, "224.0.0.88", &broadcast_addr.sin_addr); 
	
	printf("send data to UDP Server %s: %d\n", inet_ntoa(broadcast_addr.sin_addr), ntohs(broadcast_addr.sin_port));  
  
    //发送多次广播，看网络上是否有服务器存在
    int times = 10;
    int i = 0;
    for (i = 0; i < times; i++)
    {
        //一共发送10次广播，每次等待2秒是否有回应
        //广播发送服务器地址请求
        timeout.tv_sec = 2;  //超时时间为2秒
        timeout.tv_usec = 0;
		
        ret = sendto(sock, IP_FOUND, strlen(IP_FOUND), 0,
                (struct sockaddr*) &broadcast_addr, sizeof(broadcast_addr));
        if (ret < 0)
        {
            continue;
        }
		
        //文件描述符清0
        FD_ZERO(&readfd);
        //将套接字文件描述符加入到文件描述符集合中
        FD_SET(sock, &readfd);
        //select侦听是否有数据到来
        ret = select(sock + 1, &readfd, NULL, NULL, &timeout);
        switch (ret)
        {
        case -1:
            break;
        case 0:
            perror("select timeout\n");
            break;
        default:
            //接收到数据
			memset(buffer,0,1024);
            if (FD_ISSET(sock,&readfd))
            {
                count = recvfrom(sock, buffer, 1024, 0,
                        (struct sockaddr*) &from_addr, &from_len); //from_addr为服务器端地址
                printf("\trecvmsg is %s\n", buffer);
                
				sleep(1);
               
            }
            break;

        }
    }
    return 0;
}
