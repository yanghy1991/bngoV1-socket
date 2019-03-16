#include <stdio.h>  
#include <string.h>  
#include <stdlib.h>  
#include <sys/types.h>  
#include <sys/socket.h>  
#include <netinet/in.h>  
#include <arpa/inet.h>  
  
#define PORT 10086  
#define SIZE 128  
  
  
int main(void)  
{  
    int ret = -1;  
    int sockfd = -1;  
    int i = 0;  
    char buf[SIZE];  
    struct sockaddr_in addr;  
    struct sockaddr_in from;  
    //组播相关结构体  
    struct ip_mreq req;  
    socklen_t len = sizeof(from);  
  
    sockfd = socket(AF_INET, SOCK_DGRAM, 0);  
    if (-1 == sockfd)  
    {  
        perror("socket");   
        goto err0;  
    }  
  
    memset(&addr, 0, sizeof(addr));  
    addr.sin_family = AF_INET;  
    addr.sin_port = htons(PORT);  
    addr.sin_addr.s_addr = INADDR_ANY;  
    //inet_pton(AF_INET, "172.16.1.88", &addr.sin_addr);   
  
    ret = bind(sockfd, (void*)&addr, sizeof(addr));  
    if (-1 == ret)  
    {  
        perror("bind");   
        goto err1;  
    }  
  
    printf("UDP Server %s: %d\n", inet_ntoa(addr.sin_addr), ntohs(addr.sin_port));  
  
    //加入多播组  
    req.imr_multiaddr.s_addr = inet_addr("224.0.0.88");  
    //将本机加入多播组  
    req.imr_interface.s_addr = INADDR_ANY;  
  
    //加入多播组  
    ret = setsockopt(sockfd, IPPROTO_IP, IP_ADD_MEMBERSHIP, &req, sizeof(req));  
    if (ret < 0)  
    {  
        perror("setsockopt");   
        goto err0;  
    }  
  
    while(1)  
    {  
        memset(buf, 0, SIZE);  
  
        ret = recvfrom(sockfd, buf, SIZE, 0, (void*)&from, &len);  
        buf[ret] = 0;  
  
        printf("recv from: %s:%d  %s\n", inet_ntoa(from.sin_addr), ntohs(from.sin_port), buf);  
        i++;  
        //退出循环  
        if (10 == i)  
            break;  
    }  
  
    ret = setsockopt(sockfd, IPPROTO_IP, IP_DROP_MEMBERSHIP, &req, sizeof(req));  
    if (ret < 0)  
    {  
        perror("setsockopt");   
        goto err0;  
    }  
          
    //退出组播组之后 测试是否可以收到组播包  
    while(1)  
    {  
        memset(buf, 0, SIZE);  
  
        ret = recvfrom(sockfd, buf, SIZE, 0, (void*)&from, &len);  
        buf[ret] = 0;  
  
        printf("recv from: %s:%d  %s\n", inet_ntoa(from.sin_addr), ntohs(from.sin_port), buf);  
        i++;  
        //退出循环  
        if (10 == i)  
            break;  
    }  
  
  
      
  
    close(sockfd);  
    return 0;  
err1:  
    close(sockfd);  
err0:  
    return -1;  
}  