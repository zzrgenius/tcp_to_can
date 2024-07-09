/*
 * @Description:
 * @Version: 1.0
 * @Author: zirun
 * @Date: 2021-12-08 16:24:15
 * @LastEditors: zouzirun zouzirun@163.com
 * @LastEditTime: 2024-07-05 13:40:54
 */
#include "my_alg.h"

  #if 0
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
int my_sockFd;
int creat_tcp_client(void)
{
    my_sockFd = socket(PF_INET,SOCK_STREAM,0);
    if(my_sockFd < 0)
    {
        perror("craet socket error!\r");
        return -1;
    }
    struct  sockaddr_in servAddr = {0};
    servAddr.sin_family = PF_INET;
    servAddr.sin_port = htons(8765);
    servAddr.sin_addr.s_addr = inet_addr("192.168.1.3");
    int ret = connect(my_sockFd,(struct sockaddr*)&servAddr,sizeof(servAddr));
    if(ret < 0)
    {
        perror("connect error!\r");
        close(my_sockFd);
        return -2;
    }
    log_info("connect ok!\r\n");
    return 0;
    
}
#endif