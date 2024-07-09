#include "can_custom_lib.h"
#include "my_alg.h"
#include <unistd.h>  
#include <arpa/inet.h>
#include <stdio.h> 
#include <netdb.h> 
// #include <netinet/in.h> 
 #include <stdlib.h> 
#include <string.h> 
#include <sys/socket.h> 
#include <sys/types.h> 
 //	signal(SIGTERM, sigterm);
//	signal(SIGHUP, sigterm);

/*--------------------------------------------*/
/*
 @ description: open can socket
 @ input para:  can bus name
 @ return:		socket file decription
                                <0: fail
*/
/*--------------------------------------------*/

//  my_sockFd = socket(PF_INET,SOCK_STREAM,0);
//     if(my_sockFd < 0)
//     {
//         perror("craet socket error!\r");
//         return -1;
//     }
//     
#define SERVER_IP "127.0.0.1"  // 服务器IP[地址] 
// #define SERVER_IP "192.168.10.10"  // 服务器IP[地址] 
#define SERVER_PORT 4001       // 服务器端口号

int open_can_socket(void) {
  struct sockaddr_in    server_addr;

  int sockfd = 0;
     // 创建套接字 
  if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
    log_error( "  socket error\n");
    return -1;
  }
    bzero(&server_addr, sizeof(server_addr));
    server_addr.sin_family = AF_INET;           // IPv4[协议]  
    server_addr.sin_addr.s_addr = inet_addr(SERVER_IP);  // 服务器IP[地址] 
    server_addr.sin_port = htons(SERVER_PORT);  // 服务器端口号
    // 连接服务器

 // connect the client socket to server socket
    if (connect(sockfd, (struct sockaddr *)&server_addr, sizeof(server_addr)) != 0) {
        printf("connection with the server failed...\n");
       return -2;
    }
    else
    {
        printf("connected to the server..\n");
    }
    log_debug("return socket fd:%d",sockfd);
   return sockfd;
}
 

int simple_send_can(int socket, char *data) {
  int len = strlen(data);

  if (len > 8) {
    // Data are too long. Not sending.
    return -1;
  }

  // setting the frame to send
  socket_frame_t frame;

  frame.can_id = 0x7FF;
 
  for (int i = 0; i < len; ++i) {
    frame.data[i] = data[i];
  }

  // invio del frame + salvataggio dei byte inviati
  return write(socket, &frame, sizeof(socket_frame_t));
}

int send_can(int socket, int id, char *data) {
  int len = strlen(data);

  if (len > 8) {
    // Data are too long. Not sending.
    return -1;
  }

  // setting the frame to send
  socket_frame_t frame;

  frame.can_id = id;
 
  for (int i = 0; i < 8; ++i) {
    frame.data[i] = data[i];
  }

  // invio del frame + salvataggio dei byte inviati
  return write(socket, &frame, sizeof(socket_frame_t));
}

int simple_receive_can(int socket, char **data) {
  socket_frame_t frame_read;
  int ret = read(socket, &frame_read, sizeof(socket_frame_t));

  *data = (char *)malloc(8 * sizeof(char));

  for (int i = 0; i < 8; ++i) {
    (*data)[i] = frame_read.data[i];
  }

  return ret;
}

int receive_can(int socket, int *id, char **data) {
  socket_frame_t frame_read;
  int ret = read(socket, (uint8_t*)&frame_read, sizeof(socket_frame_t));

  *data = (char *)malloc(8 * sizeof(char));
  *id = frame_read.can_id;

  for (int i = 0; i < 8; ++i) {
    (*data)[i] = frame_read.data[i];
  }

  return ret;
}
 