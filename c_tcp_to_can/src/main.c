/*
 * @Author: zouzirun zouzirun@163.com
 * @Date: 2024-07-04 09:56:26
 * @LastEditors: zouzirun zouzirun@163.com
 * @LastEditTime: 2024-07-09 14:52:06
 * @FilePath: \c_tcp_to_can\src\main.c
 * @Description: 这是默认设置,请设置`customMade`, 打开koroFileHeader查看配置
 * 进行设置: https://github.com/OBKoro1/koro1FileHeader/wiki/%E9%85%8D%E7%BD%AE
 */
#include <arpa/inet.h>  // inet_addr()
#include <fcntl.h>
#include <netdb.h>
#include <pthread.h>
#include <semaphore.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>  // bzero()
#include <sys/errno.h>
#include <sys/socket.h>
#include <unistd.h>  // read(), write(), close()

#include "can_custom_lib.h"
#include "can_msg.h"
#include "dev_can.h"
 #include "ll_es_can_pro.h"
#include "my_alg.h"
#include "ringbuffer.h"

// #define SERVER_IP "127.0.0.1"  // 服务器IP[地址]
#define SERVER_IP "192.168.10.10"  // 服务器IP[地址]
#define SERVER_PORT 4001           // 服务器端口号
#define SOCKET_TXBUF_SIZE 2048
static unsigned char txbuf[SOCKET_TXBUF_SIZE]; /*发送缓冲区 ------------*/

#if 1
static pthread_t socket_rcv_id;
static pthread_t loop_id;
DEV_DCDC_CanDataT g_dc_can_data;

// void func(int sockfd) {
//   char buff[MAX];
//   int n;
//   for (;;) {
//     bzero(buff, sizeof(buff));
//     printf("Enter the string : ");
//     n = 0;
//     while ((buff[n++] = getchar()) != '\n');
//     write(sockfd, buff, sizeof(buff));
//     bzero(buff, sizeof(buff));
//     read(sockfd, buff, sizeof(buff));
//     printf("From Server : %s", buff);
//     if ((strncmp(buff, "exit", 4)) == 0) {
//       printf("Client Exit...\n");
//       break;
//     }
//   }
// }
uint8_t buff[1024];

// can message to can frame
static void pack_msg_to_frame(socket_frame_t *frame, CAN_UserMsg_t msg) {
  memset((uint8_t *)frame, 0, sizeof(socket_frame_t));
  frame->can_id = (u_int32_t)msg.msg_id.id;
  frame->head = 0x88;
  frame->data[0] = msg.msg_data.par_addr;
  frame->data[1] = msg.msg_data.scr_des_frame;
  frame->data[2] = msg.msg_data.data[0];
  frame->data[3] = msg.msg_data.data[1];
  frame->data[4] = msg.msg_data.data[2];
  frame->data[5] = msg.msg_data.data[3];
  frame->data[6] = msg.msg_data.data[4];
  frame->data[7] = msg.msg_data.data[5];
}
int ll_socket_send(Clinet_tgcan *p_client, uint8_t msgclass, uint8_t broadcast,
                   uint8_t des_device, uint8_t des_id, uint8_t par_addr,
                   uint8_t *buf_in) {
  int ret = -1;
  CAN_UserMsg_t usr_msg;
  socket_frame_t sframe;

  memset((uint8_t *)&usr_msg, 0, sizeof(CAN_UserMsg_t));
  usr_msg.msg_id.msg_class = msgclass;
  usr_msg.msg_id.broadcast = broadcast;
  usr_msg.msg_id.src_device = DEV_MONITOR;
  usr_msg.msg_id.src_id = 0;
  usr_msg.msg_id.group_num = 0;
  usr_msg.msg_id.des_device = des_device;
  usr_msg.msg_id.des_id = des_id;
  usr_msg.msg_data.par_addr = par_addr;
  usr_msg.msg_data.scr_des_frame = 0;
  memcpy(usr_msg.msg_data.data, buf_in, 6);
  pack_msg_to_frame(&sframe, usr_msg);

  ret = ring_buf_put(&p_client->rbsend, (unsigned char *)&sframe,
                     sizeof(socket_frame_t));
  return ret;
}

// int ll_es_can_sync(Clinet_tgcan *p_client) {
//   int ret = -1;
//   uint8_t databuf[6];
//   uint8_t dataout[16];

//   time_t curTime = time(NULL);
//   struct tm cur_tm;
//   localtime_r(&curTime, &cur_tm);
//   databuf[0] = (uint8_t)(cur_tm.tm_year & 0xff);
//   databuf[1] = (uint8_t)(cur_tm.tm_mon & 0xff);
//   databuf[2] = (uint8_t)(cur_tm.tm_mday & 0xff);
//   databuf[3] = (uint8_t)(cur_tm.tm_hour & 0xff);
//   databuf[4] = (uint8_t)(cur_tm.tm_min & 0xff);
//   databuf[5] = (uint8_t)(cur_tm.tm_sec & 0xff);

//   ret = hal_socket_send(p_client, RemoteSetupMsg, 1, DEV_ALL, 0, 0x01,
//   databuf);

//   return ret;
// }

static void *pthread_loop(void *arg) {
  Clinet_tgcan *p_client = (Clinet_tgcan *)arg;
  static int init_flag = 0;
  /* 线程pthread开始运行 */
  printf("pthread start!\n");

  while (1) {
    sleep(5);
    if (p_client->is_connected == 1) {
      if (init_flag == 1) {
        ll_es_can_sync(p_client);

      } else {
        ll_es_can_power_onoff(p_client, 0);
        ll_es_can_enable_dcmodule(p_client, 1);
        ll_es_can_dc_power(p_client, 100);
        ll_es_can_faultclear(p_client, 1);
        init_flag = 1;
      }
    }
  }
}
int reconnect(Clinet_tgcan *pclient) {
  static int retrys = 0;

  struct sockaddr_in servaddr;
  bzero(&servaddr, sizeof(servaddr));

  // assign IP, PORT
  servaddr.sin_family = AF_INET;
  servaddr.sin_addr.s_addr = inet_addr(SERVER_IP);
  servaddr.sin_port = htons(SERVER_PORT);

  if (connect(pclient->socketfd, (struct sockaddr *)&servaddr,
              sizeof(servaddr)) != 0) {
    printf("connection with the server failed...\n");
    return -1;
  } else {
    printf("connected to the server..\n");
    return 0;
  }
}

static void *thread_client(void *arg) {
  Clinet_tgcan *p_client = (Clinet_tgcan *)arg;
  int nbytes;
  int sbytes;
  socket_frame_t recv_frame;
  socket_frame_t send_frame;
  CAN_UserMsg_t recv_msg;
  nbytes = 0;
  log_info("enter into thread sockfd:%d ", p_client->socketfd);
  while (1) {
    if (p_client->is_connected == 1) {
      nbytes = read(p_client->socketfd, (uint8_t *)&recv_frame,
                    sizeof(socket_frame_t));
      if (nbytes == 0) {
        /* code */
        log_error("client closed\r\n");
        p_client->is_connected = 0;

      } else if (nbytes > 0) {
        /* code */
        log_info("nbytes:%d", nbytes);
        uint8_t *p_char = (uint8_t *)&recv_frame;
        // for (size_t i = 0; i < sizeof(socket_frame_t); i++) {
        //   printf("%02x ", *(p_char++));
        // }
        // printf("\r\n");
        recv_msg.msg_id.id = recv_frame.can_id;
        memcpy((uint8_t *)&recv_msg.msg_data, recv_frame.data, 8);

        can_device_msg_parse(&recv_msg);

      } else {
        if (errno == EAGAIN || errno == EWOULDBLOCK) {
        } else {
          perror("read");
        }
      }
    }
    if (p_client->is_connected == 0) {
      if (reconnect(p_client) == 0) {
        p_client->is_connected = 1;
      } else {
        log_info("retry to connect sever!\r\n");
        sleep(3);
      }
    }
    if (p_client->is_connected == 1) {
      sbytes = ring_buf_get(&p_client->rbsend, (uint8_t *)&send_frame,
                            sizeof(socket_frame_t));
      if (sbytes > 0) {
        log_info("write sbytes:%d\r\n", sbytes);
        send(p_client->socketfd, (uint8_t *)&send_frame, sbytes, MSG_DONTWAIT);
      }
    }
    usleep(10000);
  }
}

int main() {
  int sockfd;
  struct sockaddr_in servaddr;
  Clinet_tgcan *g_client = NULL;
  g_client = malloc(sizeof(Clinet_tgcan));
  g_client->rx_buf = malloc(1024 * sizeof(uint8_t));
  g_client->tx_buf = malloc(1024 * sizeof(uint8_t));
  g_client->is_connected = 0;
  g_client->socketfd = -1;
  ring_buf_init(&g_client->rbsend, txbuf, sizeof(txbuf)); /*初始化环形缓冲区 */
  pthread_mutex_init(&g_client->usr_data_mutex, NULL);

  log_debug("g_client addr:-%ld", g_client);

  // socket create and verification
  sockfd = socket(AF_INET, SOCK_STREAM, 0);
  if (sockfd == -1) {
    printf("socket creation failed...\n");
    exit(0);
  } else {
    printf("Socket[%d] successfully created..\n", sockfd);
  }
  g_client->socketfd = sockfd;

  bzero(&servaddr, sizeof(servaddr));

  // assign IP, PORT
  servaddr.sin_family = AF_INET;
  servaddr.sin_addr.s_addr = inet_addr(SERVER_IP);
  servaddr.sin_port = htons(SERVER_PORT);

  // connect the client socket to server socket
  if (connect(sockfd, (struct sockaddr *)&servaddr, sizeof(servaddr)) != 0) {
    printf("connection with the server failed...\n");
    exit(0);
  } else
    printf("connected to the server..\n");
  g_client->is_connected = 1;
  int flag = fcntl(sockfd, F_GETFL, 0);  // 获取文件fd当前的状态
  if (flag < 0) {
    perror("fcntl F_GETFL fail");
    close(sockfd);
    return -1;
  }
  if (fcntl(sockfd, F_SETFL, flag | O_NONBLOCK) < 0)  // 设置为非阻塞态
  {
    perror("fcntl F_SETFL fail");
    close(sockfd);
    return -1;
  }
  // pthread_attr_t attr;
  // pthread_attr_init(&attr);
  // pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
  if (pthread_create(&socket_rcv_id, NULL, thread_client, (void *)g_client) !=
      0) {
  }
  if (pthread_create(&loop_id, NULL, pthread_loop, (void *)g_client) != 0) {
    log_error("Create  recive thread fail\n");
  }
  // function for chat
  // func(sockfd);
  while (1) {
    sleep(1);
  }

  // close the socket
  close(sockfd);
}
#endif
 
