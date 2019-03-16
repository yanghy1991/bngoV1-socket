#include <unistd.h>  
#include <string.h>  
#include <stdlib.h>  
#include <fcntl.h>
#include <net/if.h>
#include <stdio.h>
#include <sys/types.h>  
#include <sys/socket.h>
#include <netinet/in.h>  
#include <arpa/inet.h>  
#include <sys/shm.h> 
#include <sys/ioctl.h> 
 
#include "cJSON.h"
#include "bngo_info.h"

typedef struct sds_results_file{
	int ret;
	/**
	 * @brief input 输入文本.
	 */
	char input[1024];
	/**
	 * @brief output BNGO输出.
	 */
	char output[1024];
	/**   
	 * @brief src 操作意图
	 */  
	char domain[128];
}sds_results_file;

int get_bngo_results_json(char *buffer,sds_results_file bngo_results_msg)
{
//生成json格式
	cJSON *root = NULL;
	root = cJSON_CreateObject(); // 创建根
	if (!root)  
    {  
        printf("Error before: [%s]\n",cJSON_GetErrorPtr());  
        return -1;  
    } 
	
	cJSON_AddItemToObject(root, "input", cJSON_CreateString(bngo_results_msg.input)); 
	cJSON_AddItemToObject(root, "output", cJSON_CreateString(bngo_results_msg.output)); 
	cJSON_AddItemToObject(root, "domain", cJSON_CreateString(bngo_results_msg.domain));
	cJSON_AddItemToObject(root, "asr_ret", cJSON_CreateNumber(bngo_results_msg.ret)); 
	
	memcpy(buffer,cJSON_PrintUnformatted(root),strlen(cJSON_PrintUnformatted(root)));

	if(root)
		cJSON_Delete(root);	
	
	return 0;
}

//获取设备id
int get_bngoNAME(char *bngoName){
	//获取设备id
	char buffer[256];
	FILE *devFd;
	
	if (access("/usr/data/BINGO_MSG.bngo",F_OK) == 0){
		memset(buffer,0,256);
		devFd = fopen("/usr/data/BINGO_MSG.bngo","r+");
		if(devFd == NULL){
			printf("read deviceID.txt file error!!\n");
			return -1;
		}
		fread(buffer,256,1,devFd);
		fclose(devFd);
		
		//解析
		char *result_json = NULL;
		result_json = strstr(buffer,"{");
		
		cJSON * root = NULL;
		cJSON * bngoname = NULL;
		root = cJSON_Parse(result_json);
		if (!root) 
		{
			//printf("Error before: [%s]\n",cJSON_GetErrorPtr());
			printf("%s : parse json error.\n",__func__);  
			return -1;
		}

		bngoname = cJSON_GetObjectItem(root, "NAME");
		if(bngoname == NULL)
		{
			printf("%s : get err_msg failed! error:[%s]\n",__func__);
			return -1;//获取err_msg失败
		}
		
		memcpy(bngoName,bngoname->valuestring,strlen(bngoname->valuestring)+1);
		cJSON_Delete(root);
	} else {
		return -1;
	}
	
	return 0;
}

int getLocalIP(int inet_sock){
    int port_sock;  
    struct ifreq ifr;  
	char ip[32] = {0};  
	struct sockaddr_in local_addr;
	int sin_size = sizeof(struct sockaddr_in);
	
    port_sock = socket(AF_INET, SOCK_DGRAM, 0);  
    strcpy(ifr.ifr_name, "wlan0");  
    if(ioctl(port_sock, SIOCGIFADDR, &ifr) < 0){
		perror("ioctl-conf:");
        return -1;
	}
	close(port_sock);
		
    strcpy(ip, inet_ntoa(((struct sockaddr_in *)&ifr.ifr_addr)->sin_addr)); 
	
	getsockname(inet_sock, (struct sockaddr*)&local_addr, &sin_size);
	
	/*
	printf("\nserver information:\n\t IP: %s, Port: %d\n",
									ip,
									ntohs(local_addr.sin_port));
	*/

	char bngoID[100] = {0};
	if(get_bngoID(bngoID) < 0){
		printf("get bngoid failed \n");
		return -1;		
	}
	
	
	char bngoName[50] = {0};
	if(get_bngoNAME(bngoName) < 0){
		printf("get bngoName failed \n");
		return -1;		
	}
	
	//生成json格式
	cJSON *root = NULL;
	root = cJSON_CreateObject(); // 创建根
	if (!root)  
    {  
        printf("Error before: [%s]\n",cJSON_GetErrorPtr());  
        return -1;  
    } 
	cJSON_AddItemToObject(root, "BINGO_ID", cJSON_CreateString(bngoID)); 
	cJSON_AddItemToObject(root, "IP", cJSON_CreateString(ip)); 
	cJSON_AddItemToObject(root, "PORT", cJSON_CreateNumber(ntohs(local_addr.sin_port))); 
	cJSON_AddItemToObject(root, "NAME", cJSON_CreateString(bngoName)); 
	
	FILE *devFd;
	devFd = fopen("/usr/data/tcp_info.txt","w+");
	if(devFd == NULL){
		printf("create deviceID.txt file error!!\n");
		return -2;
	}
	//memcpy(ASR_COMMAND,cJSON_PrintUnformatted(root),strlen(cJSON_PrintUnformatted(root)));
	fwrite(cJSON_PrintUnformatted(root),strlen(cJSON_PrintUnformatted(root)),1,devFd);
	fflush(devFd);
	fclose(devFd);

	
	if(root)
		cJSON_Delete(root);
	
    return 0;
} 