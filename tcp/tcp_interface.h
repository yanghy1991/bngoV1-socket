#ifndef __TCP_INTERFACE_H_
#define __TCP_INTERFACE_H_

#include "asr.h"

int getLocalIP(int inet_sock);
int get_bngo_results_json(char *buffer,sds_results_file bngo_results_msg);

#endif //#ifndef __TCP_INTERFACE_H_
