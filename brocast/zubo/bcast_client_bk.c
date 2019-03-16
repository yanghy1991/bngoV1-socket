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
    struct sockaddr_in peer;  
  
    sockfd = socket(AF_INET, SOCK_DGRAM, 0);  
    if (-1 == sockfd)  
    {  
        perror("socket");   
        goto err0;  
    }  
  
    memset(&peer, 0, sizeof(peer));  
    peer.sin_family = AF_INET;  
    peer.sin_port = htons(PORT);  
    inet_pton(AF_INET, "224.0.0.88", &peer.sin_addr);   
  
    printf("send data to UDP Server %s: %d\n", inet_ntoa(peer.sin_addr), ntohs(peer.sin_port));  
  
  
    //向多播组发送消息  
    while(1)  
    {  
        sprintf(buf, "hello world %d", i);   
        ret = sendto(sockfd, buf, strlen(buf), 0, (void*)&peer, sizeof(peer));  
        printf("ret: %d\n", ret);  
        sleep(1);  
        i++;   
    }  
  
  
  
  
    return 0;  
err0:  
    return -1;  
}  