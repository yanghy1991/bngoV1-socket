/*********************************************************************
 * Filename: bcast_server.c
 * Description: 广播服务器端代码
 * Author: yhy
 * Date: 2012-9-14
 ********************************************************************/
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <fcntl.h>
#include <linux/in.h>
#include <stdlib.h>

#define IP_FOUND "IP_FOUND"
#define IP_FOUND_ACK "IP_FOUND_ACK"
#define PORT 9998

int main(int argc, char*argv[])
{
	FILE *devFd;
    int ret = -1;
    int sock;
    struct sockaddr_in server_addr; //服务器端地址
    struct sockaddr_in from_addr; //客户端地址
    int from_len = sizeof(struct sockaddr_in);
    int count = -1;
    fd_set readfd; //读文件描述符集合
    char buffer[256];
    struct timeval timeout;
    timeout.tv_sec = 2;
    timeout.tv_usec = 0;
	char BinGO_meg[256] = "{\"BINGO_ID\":\"SZH1-BINGOV1-ccb8a82c4df8\",\"location\":\"深圳\",\"netconfig\":\"NO\",\"NAME\":\"123456\"}";

    sock = socket(AF_INET, SOCK_DGRAM, 0); //建立数据报套接字
    if (sock < 0)
    {
        perror("sock error");
        return -1;
    }
	
	int nRecvBuf = 34*1024;
	setsockopt(sock,SOL_SOCKET,SO_RCVBUF,(const char*)&nRecvBuf,sizeof(int));
	setsockopt(sock,SOL_SOCKET,SO_SNDBUF,(const char*)&nRecvBuf,sizeof(int));
	
	int reuseaddr=1;//开启端口重复使用功能
	if(setsockopt(sock,SOL_SOCKET,SO_REUSEADDR,&reuseaddr,sizeof(reuseaddr)) < 0)
		perror("SO_REUSEADDR ERROR");
	
	const int opt = 1;  
    //设置该套接字为广播类型，  
    
    if(setsockopt(sock, SOL_SOCKET, SO_BROADCAST, (char *)&opt, sizeof(opt)) < 0)  
		perror("SO_BROADCAST ERROR");
	
	struct timeval t;
	t.tv_sec=0;//设置为10秒
	t.tv_usec=0;
	int len1=sizeof(t);
	
	getsockopt(sock,SOL_SOCKET,SO_SNDTIMEO,(char *)&t,&len1);
	printf("SO_SNDTIMEO timeout:%d #########\n",t.tv_sec);
	
	getsockopt(sock,SOL_SOCKET,SO_RCVTIMEO,(char *)&t,&len1);
	printf("SO_RCVTIMEO timeout:%d #########\n",t.tv_sec);
	
    memset((void*) &server_addr, 0, sizeof(struct sockaddr_in));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = htons(INADDR_ANY);
    server_addr.sin_port = htons(PORT);

    //将地址结构绑定到套接字上
    ret = bind(sock, (struct sockaddr*) &server_addr, sizeof(server_addr));
    if (ret < 0)
    {
        perror("bind error");
        return -1;
    }

    /**
     * 循环等待客户端
     */
    while (1)
    {
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
				memset(buffer,0,256);
                count = recvfrom(sock, buffer, 256, 0,
                        (struct sockaddr*)&from_addr, &from_len); //接收客户端发送的数据

                //from_addr保存客户端的地址结构
                if (strstr(buffer, IP_FOUND))
                {
                    //响应客户端请求
                    //打印客户端的IP地址和端口号
                    printf("\nClient connection information:\n\t IP: %s, Port: %d\n",
                            (char *)inet_ntoa(from_addr.sin_addr),
                            ntohs(from_addr.sin_port));

                    //将数据发送给客户端
                    //memcpy(buffer, IP_FOUND_ACK, strlen(IP_FOUND_ACK) + 1);
                    //memcpy(buffer, BinGO_meg, strlen(BinGO_meg) + 1);
					/*
					memset(buffer,0,128);
					devFd = fopen("/usr/fs/usr/yuan/BINGO_MSG.txt","r+");
					if(devFd == NULL){
						printf("read deviceID.txt file error!!\n");
						return -2;
					}
					fread(buffer,128,1,devFd);
					fclose(devFd);
					*/
                    count = sendto(sock, BinGO_meg, strlen(BinGO_meg), 0,
                            (struct sockaddr*) &from_addr, from_len);
					printf("sendto meg : [%s] .. \n",BinGO_meg);
                }
            }
            break;
        }
    }
	
	close(sock);
	printf("close sock............\n");
    return 0;
}
