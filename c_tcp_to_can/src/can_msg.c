
#include "can_msg.h"

#include <errno.h>
#include <pthread.h>
#include <semaphore.h>
#include <stdarg.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <time.h>
#include <unistd.h>

#include "dev_can.h"

// message write wait result out time
#define CAN_WAIT_ACK_TIMEDOUT 2000  // ms
// message valid time
#define CAN_RCV_MSG_TIMEDOUT 4000  // ms


// printf can msg data
//  uint32_t msg_class : 7;
//     uint32_t broadcast : 1;
//     uint32_t src_device : 4;
//     uint32_t src_id : 6;
//     uint32_t group_num : 4;
//     uint32_t des_device : 4;
//     uint32_t des_id : 6;

static int insert_can_frame_send_list(Client_t *client, socket_frame_t frame);

int CAN_MsgPrint(CAN_UserMsg_t msg) {
  int i;
  char buf[512] = {'\0'};
  int ncnt = 0;
  memset(buf, 0, 512);
  ncnt =
      sprintf(buf,
              "CAN MSG ID:%x: msg_class(%02x) broadcast(%02x) src_device(%02x) "
              "src_id(%02x) group_num(%02x) des_device(%02x) des_id(%02x) ",
              msg.msg_id.id, msg.msg_id.msg_class, msg.msg_id.broadcast,
              msg.msg_id.src_device, msg.msg_id.src_id, msg.msg_id.group_num,
              msg.msg_id.des_device, msg.msg_id.des_id);
  ncnt += sprintf(buf + ncnt, " par_addr=%02x sd frame:%02x ",
                  msg.msg_data.par_addr, msg.msg_data.scr_des_frame);

  for (i = 0; i < 6; ++i) {
    ncnt += sprintf(buf + ncnt, " %02x ", msg.msg_data.data[i]);
  }

  printf("%s\r\n", buf);
  printf("end \r\n");

  return 0;
}
// //打包数据包
// int can_pack_msg(CAN_UserMsg_t *p, ...) {
//   int ret = 0;

//   return ret;
// }

// //解数据包
// int can_msg_parse(CAN_UserMsg_t *msg, ...) {
//   int ret = 0;

//   return ret;
// }
 
 

// can use print message
void can_print_msg(char *head_name, long count, CAN_UserMsg_t msg) {
  int i;
  int ncnt = 0;
  char buf[128] = {'\0'};
  ncnt += sprintf(buf + ncnt, "can id 0x%x class: 0x%x src:0x%x par_id: 0x%x",
                  msg.msg_id.id, msg.msg_id.src_device, msg.msg_id.msg_class,
                  msg.msg_data.par_addr);
  for (i = 0; i < 6; i++) {
    ncnt += sprintf(buf + ncnt, " [%02x]", msg.msg_data.data[i]);
  }

  printf("%s(%ld):%s\n", head_name, count, buf);
}

 

static long time_subtrack(struct timeval *diff, struct timeval x,
                          struct timeval y) {
  long tim;
  struct timeval rel;
  if (x.tv_sec > y.tv_sec) {
    return -1;
  }

  if ((x.tv_sec == y.tv_sec) && (x.tv_usec > y.tv_usec)) {
    return -1;
  }

  if (y.tv_usec >= x.tv_usec) {
    rel.tv_usec = y.tv_usec - x.tv_usec;
    rel.tv_sec = y.tv_sec - x.tv_sec;
  } else {
    rel.tv_usec = 1 * 1000 * 1000 + x.tv_usec - y.tv_usec;
    rel.tv_sec = y.tv_sec - x.tv_sec - 1;
  }

  tim = (rel.tv_usec / 1000);
  if ((rel.tv_usec % 1000) > 500) {
    tim += 1;
  }

  tim += rel.tv_sec * 1000;
  if (diff != NULL) {
    diff->tv_sec = rel.tv_sec;
    diff->tv_usec = rel.tv_usec;
  }

  return tim;
}
 
#if 0
// can write and read message
int can_write_and_read_msg(msg_client_t *can, CAN_UserMsg_t *msg) {
  socket_frame_t  frame;
  // CAN_UserMsg_t msg_tmp;
  struct timespec ts;
  struct timeval tt;
  int ret = 0;
  int sem_row = 0;
  int sem_col = 0;

#if defined CAN_USE_DEBUG_EN_MSG
  static long i = 0;
#endif

  struct timeval start, stop;
  long diff_time = 0;
  gettimeofday(&start, NULL);

   // msg to frame
  can_msg_to_frame(&frame, *msg, 0, 8);
  // send
  ret = write_can_frame(can->myclient, frame); // have blocking
  if (ret < 0) {
    log_error( "can write msg fail\n");
     return ret;
  }

  // wait ack

  gettimeofday(&tt, NULL);
  ts.tv_sec = tt.tv_sec;
  ts.tv_nsec = tt.tv_usec * 1000 + (CAN_WAIT_ACK_TIMEDOUT % 1000) * 1000 * 1000;
  ts.tv_sec += ts.tv_nsec / (1000 * 1000 * 1000) + CAN_WAIT_ACK_TIMEDOUT / 1000;
  ts.tv_nsec %= 1000 * 1000 * 1000;

  while (1) {

    if (ret == -1) {
      if (errno == ETIMEDOUT) {
        log_warn("can message wait read ack time out\n");
      } else {
        printf("%s\n", strerror(errno));
        log_error( "can message wait read ack error(%d)\n", errno);
      }
    }
    gettimeofday(&stop, NULL);
    diff_time = time_subtrack(NULL, start, stop);
    if ((ret >= 0) && (diff_time == 0)) {

    } else {
      break;
    }
  }
#if defined CAN_USE_DEBUG_EN
  printf("diff ret time = %ld ms\n", time_subtrack(NULL, start, stop));
#endif
  // find massage ack in can recive massage list
  // ret = can_find_ack_msg(can, msg);
  if (ret < 0) {
    log_warn("can write msg can't find ack %d\n", ret);
     return -1;
  }

   return ret;
}
#endif
 

// /* Get a float from 4 bytes (Modbus) without any conversion (ABCD) */
// float modbus_get_float_abcd(const uint16_t *src) {
//   float f;
//   uint32_t i;

//   i = ntohl(((uint32_t)src[0] << 16) + src[1]);
//   memcpy(&f, &i, sizeof(float));

//   return f;
// }

// /* Get a float from 4 bytes (Modbus) in inversed format (DCBA) */
// float modbus_get_float_dcba(const uint16_t *src) {
//   float f;
//   uint32_t i;

//   i = ntohl(__bswap_32((((uint32_t)src[0]) << 16) + src[1]));
//   memcpy(&f, &i, sizeof(float));

//   return f;
// }

// /* Get a float from 4 bytes (Modbus) with swapped bytes (BADC) */
// float modbus_get_float_badc(const uint16_t *src) {
//   float f;
//   uint32_t i;

//   i = ntohl((uint32_t)(__bswap_16(src[0]) << 16) + __bswap_16(src[1]));
//   memcpy(&f, &i, sizeof(float));

//   return f;
// }

// /* Get a float from 4 bytes (Modbus) with swapped words (CDAB) */
// float modbus_get_float_cdab(const uint16_t *src) {
//   float f;
//   uint32_t i;

//   i = ntohl((((uint32_t)src[1]) << 16) + src[0]);
//   memcpy(&f, &i, sizeof(float));

//   return f;
// }

// /* DEPRECATED - Get a float from 4 bytes in sort of Modbus format */
// float modbus_get_float(const uint16_t *src) {
//   float f;
//   uint32_t i;

//   i = (((uint32_t)src[1]) << 16) + src[0];
//   memcpy(&f, &i, sizeof(float));

//   return f;
// }

// /* Set a float to 4 bytes for Modbus w/o any conversion (ABCD) */
// void modbus_set_float_abcd(float f, uint16_t *dest) {
//   uint32_t i;

//   memcpy(&i, &f, sizeof(uint32_t));
//   i = htonl(i);
//   dest[0] = (uint16_t)(i >> 16);
//   dest[1] = (uint16_t)i;
// }

// /* Set a float to 4 bytes for Modbus with byte and word swap conversion
// (DCBA)
//  */
// void modbus_set_float_dcba(float f, uint16_t *dest) {
//   uint32_t i;

//   memcpy(&i, &f, sizeof(uint32_t));
//   i = __bswap_32(htonl(i));
//   dest[0] = (uint16_t)(i >> 16);
//   dest[1] = (uint16_t)i;
// }

// /* Set a float to 4 bytes for Modbus with byte swap conversion (BADC) */
// void modbus_set_float_badc(float f, uint16_t *dest) {
//   uint32_t i;

//   memcpy(&i, &f, sizeof(uint32_t));
//   i = htonl(i);
//   dest[0] = (uint16_t)__bswap_16(i >> 16);
//   dest[1] = (uint16_t)__bswap_16(i & 0xFFFF);
// }

// /* Set a float to 4 bytes for Modbus with word swap conversion (CDAB) */
// void modbus_set_float_cdab(float f, uint16_t *dest) {
//   uint32_t i;

//   memcpy(&i, &f, sizeof(uint32_t));
//   i = htonl(i);
//   dest[0] = (uint16_t)i;
//   dest[1] = (uint16_t)(i >> 16);
// }

// /* DEPRECATED - Set a float to 4 bytes in a sort of Modbus format! */
// void modbus_set_float(float f, uint16_t *dest) {
//   uint32_t i;

//   memcpy(&i, &f, sizeof(uint32_t));
//   dest[0] = (uint16_t)i;
//   dest[1] = (uint16_t)(i >> 16);
// }
/*------------------------------end file-----------------------------------*/