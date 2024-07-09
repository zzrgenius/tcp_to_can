/*
 * @Description:
 * @Version: 1.0
 * @Author: zirun
 * @Date: 2021-12-08 16:41:33
 * @LastEditors: zouzirun zouzirun@163.com
 * @LastEditTime: 2024-07-05 13:41:11
 */
#ifndef __MY_ALG_H
#define __MY_ALG_H
  #include <errno.h>
 #include <math.h>
#include <pthread.h>
#include <semaphore.h>
#include <signal.h>
#include <stdio.h>
#include <time.h>
#include "log.h"
#define USE_MASTER_COURT 1

#define DEV_ES_TYPE_ACA 0x01
#define DEV_ES_TYPE_ACB 0x02
#define DEV_ES_TYPE_DC_CAN 0x03

#define DEV_PV_TYPE_ACA 0x01
#define DEV_PV_TYPE_DC 0x02

#define TEST_DEBUG 1
#define __DEBUG_PRINTF__
 #ifdef __cplusplus
extern "C" {
#endif

#include <stdio.h> 
 

#define EPS 1e-5
#define LP_A (0.00523f)
#define Equ(a, b) ((fabs((a) - (b))) < (EPS))
#define More(a, b) (((a) - (b)) > (EPS))
#define Less(a, b) (((a) - (b)) < (-EPS))
#define MoreEqu(a, b) (((a) - (b)) > (-EPS))
#define LessEqu(a, b) (((a) - (b)) < (EPS))

#define READ_BIT(REG, BIT) ((REG) & (BIT))

#define GET_BIT(X, BIT) ((X & (1u << BIT)) >> BIT)
#define CLEAR_BIT(x, bit) (x &= ~(1 << bit)) /* �����bitλ */
#define SET_BIT(x, bit) (x |= (1 << bit)) /* ��λ��bitλ */

#define BIT_MASK(x) (1u << (x))
#define BIT_GET(x, y) (((x) >> (y)) & 0x01u)
#define BIT_SET(x, y) ((x) |= (1 << (y)))
#define BIT_CLR(x, y) ((x) &= (~(1 << (y))))
#define BIT_INVERT(x, y) ((x) ^= (1 << (y)))
#define TOSTRING(X) #X
 
#define OUTPOWER_PACK_SIZE 6 //��������
 
 
 
#define MAX_DEV_ES_NUMS 2
#define MAX_DEV_PV_NUMS 4
#define MAX_DEV_ACDC_NUMS 2
#define MAX_LOAD_NUMS 20

 #if 0
extern int my_sockFd;
int creat_tcp_client(void);
#endif

#ifdef __cplusplus
}
#endif
#endif