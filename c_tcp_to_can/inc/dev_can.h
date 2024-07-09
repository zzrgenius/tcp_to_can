/*
 * @Author: zouzirun zouzirun@163.com
 * @Date: 2024-07-04 10:11:55
 * @LastEditors: zouzirun zouzirun@163.com
 * @LastEditTime: 2024-07-09 14:35:04
 * @FilePath: \tcp_Client_test\c_tcp_to_can\src\dev_can.h
 * @Description: 这是默认设置,请设置`customMade`, 打开koroFileHeader查看配置
 * 进行设置: https://github.com/OBKoro1/koro1FileHeader/wiki/%E9%85%8D%E7%BD%AE
 */
/*
 *
 * ������������������+ +
 * �������ߩ��������ߩ� + +
 * �������������������� ��
 * �������������������� ++ + + +
 *  ������������������ ��+
 * �������������������� +
 * �����������ߡ�������
 * �������������������� + +
 * ��������������������
 * ��������������������������������������
 * ���������������� + + + +
 * ����������������
 * ���������������� +  ���ޱ���
 * ����������������    ������bug����
 * ��������������������+������������������
 * ���������� �������������� + +
 * �������� ���������������ǩ�
 * �������� ������������������
 * �����������������ש����� + + + +
 * �����������ϩϡ����ϩ�
 * �����������ߩ������ߩ�+ + + +
 *
 *
 * @file           :
 * @brief          : This is a brief description.
 * @details        :  This is the detail description.
 * @Author         : zirun
 * @version        : v1.0
 * @Date           : 2022-04-15 13:54:03
 * @LastEditors    : zirun
 * @LastEditTime   : 2022-04-15 13:54:07
 * @FilePath       : /lgcext/dev_can.h
 */
#ifndef DEV_CAN_H
#define DEV_CAN_H
#include <semaphore.h>
#include <stdint.h>
#include <stdio.h>

#include "can_custom_lib.h"
#include "can_msg_list.h"
#include "log.h"
#include "my_alg.h"
#include "ringbuffer.h"
#define CAN_BUS_DEBUG_EN

// can type
typedef struct {
  int socket_fd;      //   socket file descriptor
  int rcv_frame_cnt;  // socket recive frame count(test use)
  int snd_frame_cnt;  // socket send frame count(test use)

  List *frame_recv_list;
  List *frame_send_list;

  int frame_cnt;  // CAN recive msg not deal frame count(test use)
  pthread_mutex_t frame_list_recv_mutex;  // CAN recive message list
  pthread_mutex_t frame_list_send_mutex;  // CAN recive message list

  sem_t frame_sem;  // recive a frame need to deal sem init 0 not
} Client_t;

typedef struct {
  int socketfd;
  int is_connected;
  uint8_t *rx_buf;
  uint8_t *tx_buf;
  ring_buf_t rbsend;
    pthread_mutex_t usr_data_mutex; 
} Clinet_tgcan;

// open client
// close can
void close_can(Client_t *client);

// can write frame
int write_can_frame(Client_t *client, socket_frame_t *frame);

// read can frame from recive msg list
int read_can_frame(Client_t *client, socket_frame_t *msg);
int get_send_frame(Client_t *client, socket_frame_t *frame_out);

void print_can_frame_t(char *head_name, long count, socket_frame_t frame);
#endif
