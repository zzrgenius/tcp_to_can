
#ifndef _CAN_MSG_H
#define _CAN_MSG_H

#include "can_msg_list.h"
#include "dev_can.h"
#include <pthread.h>
#include <semaphore.h>
#include <stdint.h>
#include <sys/_timeval.h>
//#define CAN_USE_DEBUG_EN_MSG
//#define CAN_USE_DEBUG_EN_IN
//#define CAN_USE_DEBUG_EN_OUT
//#define CAN_USE_DEBUG_EN

#define RemoteControlMsg 0x01      //遥控命令
#define RemoteSetupMsg 0x02        //遥调命令
#define RemoteSignalMsg 0x03       // 遥信命令
#define RemoteMeterMsg 0x04        //遥测命令
#define CorrectionCoefficient 0x05 //校正系数
// Source Device 定义
#define DEV_ALL 0x00
#define DEV_MONITOR 0x01
#define DEV_DCDC 0x04
#define DEV_MPPT 0x05
#define DEV_BMS 0x08

typedef union {

  struct {
    uint32_t des_id : 6;
    uint32_t des_device : 4;
    uint32_t group_num : 4;
    uint32_t src_id : 6;
    uint32_t src_device : 4;
    uint32_t broadcast : 1;
    uint32_t msg_class : 7;
  };
  uint32_t id;
} _msg_canid;

typedef struct {
  uint8_t par_addr;
  uint8_t scr_des_frame;
  uint8_t data[6];
} _msg_candata;
typedef struct {
  _msg_candata msg_data;
  _msg_canid msg_id;

} CAN_UserMsg_t;

#define CMD_TYPE_CHECK 0
#define CMD_CHECK_ID 1
#define CMD_CHECK_ACK_ID 2
#define CMD_SELF_CHECK_ID 3
#define CMD_SELF_CHECK_ACK_ID 4

/*-----------------*/

#define CMD_TYPE_CTRL 1

#define CMD_DI_ID 1
#define CMD_DI_ACK_ID 2
#define CMD_DO_ID 3
#define CMD_DO_ACK_ID 4
#define CMD_AD_ID 5
#define CMD_AD_ACK_ID 6

#define CMD_DO_ALL_ID 7
#define CMD_DO_ALL_ACK_ID 8

#define CMD_DI_ALL_ID 9
#define CMD_DI_ALL_ACK_ID 10

/*-----------------*/
#define CMD_TYPE_CMP 2

#define CMD_STEPS_SET_ID 1
#define CMD_STEPS_SET_ACK_ID 2

#define CMD_STEP_DO_SET_ID 3
#define CMD_STEP_DO_SET_ACK_ID 4

#define CMD_STEP_TIME_SET_ID 5
#define CMD_STEP_TIME_SET_ACK_ID 6

#define CMD_LIST_DO_ID 7
#define CMD_LIST_DO_ACK_ID 8

#define CMD_LIST_STATE_ID 9
#define CMD_LIST_STATE_ACK_ID 10

// msg type max count
#define CAN_MSG_TYPE_MAX (2 + 1)
// msg command max count
#define CAN_MSG_CMD_MAX 10

/*--------------------------------------*/
// CAN msg type
typedef union {
  struct {
    uint8_t dst;
    uint8_t src;
    uint8_t type;
    uint8_t cmd;
    uint8_t data[4];
  };
  uint8_t data_m[8];
} CAN_Msg_t;

 
// can messgae inital
 //打包数据包
int can_pack_msg(CAN_UserMsg_t *p, ...);
//解数据包
int can_msg_parse(CAN_UserMsg_t *msg, ...);

// write an wait for result by point to ponit
  // can use print message
void can_print_msg(char *head_name, long count, CAN_UserMsg_t msg);

#endif
/*------------------------------end file-----------------------------------*/
