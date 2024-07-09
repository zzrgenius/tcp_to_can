/*
 * can_custom_lib.h
 *
 * Enhancement of the can utils lib for linux
 * *          
 * This is a lib done exclusively for the FSAE Eagle Racing Team, from the University of Trento.
 * Everything is built on top of the raw canbus.
 *
 * See the examples to understand its full usage
 * 
 * == Still developing ==
 */

#ifndef CAN_CUSTOM_LIB_H
#define CAN_CUSTOM_LIB_H

//basic function
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>

//sockets import
 
#include <net/if.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/uio.h>
#include "log.h"

#define CAN_MAX_DLEN 8
typedef u_int32_t canid_t;
/*
 * Controller Area Network Identifier structure
 *
 * bit 0-28 : CAN identifier (11/29 bit)
 * bit 29 : error message frame flag (0 = data frame, 1 = error message)
 * bit 30 : remote transmission request flag (1 = rtr frame)
 * bit 31 : frame format flag (0 = standard 11 bit, 1 = extended 29 bit)
 */
/* special address description flags for the CAN_ID */
#define CAN_EFF_FLAG 0x80000000U /* EFF/SFF is set in the MSB */
#define CAN_RTR_FLAG 0x40000000U /* remote transmission request */
#define CAN_ERR_FLAG 0x20000000U /* error message frame */

/* valid bits in CAN ID for frame formats */
#define CAN_SFF_MASK 0x000007FFU /* standard frame format (SFF) */
#define CAN_EFF_MASK 0x1FFFFFFFU /* extended frame format (EFF) */
#define CAN_ERR_MASK 0x1FFFFFFFU /* omit EFF, RTR, ERR flags */

 typedef struct   {
    uint8_t head;
 canid_t can_id;  /* 32 bit CAN_ID + EFF/RTR/ERR flags */
  u_int8_t    data[8] ;
} __attribute__ ((packed)) socket_frame_t ;
 typedef struct   {
  canid_t can_id;  /* 32 bit CAN_ID + EFF/RTR/ERR flags */
  u_int8_t    data[8] ;
} __attribute__ ((packed)) can_frame_t ;
// int open_can_socket(char* can_device, struct sockaddr_can* addr);
 int open_can_socket(void) ;

int simple_send_can(int socket, char* data);
int send_can(int socket, int id, char* data);

int simple_receive_can(int socket, char** data);
int receive_can(int socket, int* id,char** data);
// int receive_can_compact(int socket, int* id,int* data1, int* data2);
#endif
