
#include "dev_can.h"

#include <net/if.h>
#include <pthread.h>
#include <semaphore.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/uio.h>
#include <unistd.h>

#include "can_custom_lib.h"
#include "can_msg_list.h"
#if defined CAN_BUS_DEBUG_EN_FRAME
#include <sys/time.h>
#endif

/*------------end include----------------------*/
#if defined CAN_BUS_DEBUG_EN
// static void print_can_frame(char *head_name, long count, struct can_frame
// frame);
#endif
static can_frame_t *malloc_can_frame(void);
// open can socket
// insert can recive massage frame to massage recive list
static int insert_can_frame_recv_list(Client_t *client, socket_frame_t frame);
// can recive msg thread
void *thread_can_frame_rcv(void *arg);
/*---------------------------------------------*/
void print_can_frame_t(char *head_name, long count, socket_frame_t frame) {
  char buf[218] = {'\0'};
  int ncnt = 0;
  int i;

  ncnt = sprintf(buf, "%s(%ld): <%03x> [%d]", head_name, count, frame.can_id);
  for (i = 0; i < 8; i++) ncnt += sprintf(buf + ncnt, " [%02x]", frame.data[i]);
  printf("%s\n", buf);
}

/*--------------------------------------------*/
/*
 @ description: malloc a can frame
 @ return:		frame memory pointer
                                NULL: malloc fail
*/
/*--------------------------------------------*/
static can_frame_t *malloc_can_frame(void) {
  can_frame_t *p;
  p = malloc(sizeof(can_frame_t));
  if (p == NULL) {
    return NULL;
  }
  p->can_id = 0;
  memset(p->data, 0, 8);
  return p;
}

/*--------------------------------------------*/
/*
 @ description: write can frame
 @ input para:
                                can: can type
                                msg: can frame massage
 @ return:		write to can TR data count(byte)
                                <0: fail
*/
/*--------------------------------------------*/
int write_can_frame(Client_t *client, socket_frame_t *frame) {
  socket_frame_t *pframe = frame;
  int i;
  int ret = 1;

  log_info("client address:%ld\r\n", client);
  if (client == NULL) {
    log_error("client IS NULL\n");
  }
  pframe->can_id =
      (pframe->can_id & CAN_EFF_MASK) | CAN_EFF_FLAG;  // extend frame

  ret = write(client->socket_fd, (uint8_t *)pframe, sizeof(socket_frame_t));
  if (ret == -1) {
    log_error("socket write fail\n");
    //   // wait buffer free
    usleep(200 * 1000);
    ret = -2;
  } else {
    client->snd_frame_cnt++;
  }

  return ret;
}

int get_send_frame(Client_t *client, socket_frame_t *frame_out) {
  socket_frame_t *frame_temp;
  ListNode *get_list_node = NULL;
  int i;

  if (client == NULL) {
    log_debug("client is null\r\n");

    return 0;
  }
  pthread_mutex_lock(&client->frame_list_send_mutex);

  if (client->frame_recv_list->len == 0) {
    // can no massage
    log_debug("can frame list is not have message\n");

    pthread_mutex_unlock(&client->frame_list_send_mutex);

    return -1;
  }

  get_list_node = list_lpop(client->frame_send_list);
  if (get_list_node != NULL) {
    frame_temp = (socket_frame_t *)(get_list_node->val);
    frame_out->can_id = frame_temp->can_id;
    frame_out->head = frame_temp->head;

    for (i = 0; i < 8; i++) {
      frame_out->data[i] = frame_temp->data[i];
    }
  } else {
    return -1;
  }
  free(frame_temp);
  free(get_list_node);
  pthread_mutex_unlock(&client->frame_list_send_mutex);
  return 8;
}

/*--------------------------------------------*/
/*
 @ description: read can frame from recive msg list
 @ input para:
                                can: can type
                                msg: can frame out
 @ return:		data count
                                <0: fail
*/
/*--------------------------------------------*/
int read_can_frame(Client_t *client, socket_frame_t *frame_out) {
  socket_frame_t *frame_temp;
  ListNode *get_list_node = NULL;
  int i;

  if (client == NULL) {
    log_debug("client is null\r\n");

    return -1;
  }
  log_debug("debug\r\n");

  pthread_mutex_lock(&client->frame_list_recv_mutex);

  if (client->frame_recv_list->len == 0) {
    // can no massage
    log_debug("can frame list is not have message\n");

    pthread_mutex_unlock(&client->frame_list_recv_mutex);

    return -2;
  }

  get_list_node = list_lpop(client->frame_recv_list);
  if (get_list_node != NULL) {
    frame_temp = (socket_frame_t *)(get_list_node->val);
    frame_out->can_id = frame_temp->can_id;
    frame_out->head = frame_temp->head;

    for (i = 0; i < 8; i++) {
      frame_out->data[i] = frame_temp->data[i];
    }
  } else {
    return -1;
  }
  free(frame_temp);
  free(get_list_node);
  pthread_mutex_unlock(&client->frame_list_recv_mutex);
  return 8;
}

// insert can recive massage frame to massage recive list
static int insert_can_frame_recv_list(Client_t *client, socket_frame_t frame) {
  can_frame_t *msg;

  int i;

  // malloc a node
  msg = malloc_can_frame();
  if (msg == NULL) {
    log_error("can msg malloc fail\n");
    return -1;
  }

  // standar or extend frame
  // msg->type = 0;
  if (frame.can_id & CAN_EFF_FLAG) {
    msg->can_id = frame.can_id & CAN_EFF_MASK;
    // msg->type |= 0x01;
  } else {
    msg->can_id = frame.can_id & CAN_SFF_MASK;
    // msg->type &= (~0x01);
  }

  if (frame.can_id & CAN_RTR_FLAG) {
    // msg->type |= 0x20;
  }

  for (i = 0; i < 8; i++) {
    msg->data[i] = frame.data[i];
  }
  // find recive massage list tail
  pthread_mutex_lock(&client->frame_list_recv_mutex);
  ListNode *node = list_node_new(msg);
  if (node != NULL) {
    list_rpush(client->frame_recv_list, node);
  }
  client->frame_cnt += 1;
  pthread_mutex_unlock(&client->frame_list_recv_mutex);

  sem_post(&client->frame_sem);
  return 8;
}

void *thread_can_frame_rcv(void *arg) {
  int fd;
  socket_frame_t frame;
  int nbytes;
  int ret;
  Client_t *client = NULL;

#if defined CAN_BUS_DEBUG_EN_FRAME
  struct timeval start, end;
#endif

  client = (Client_t *)arg;
  fd = client->socket_fd;

  log_debug("%s FD:%d sizeof socket_frame_t -- %d", __FUNCTION__, fd,
            sizeof(socket_frame_t));
  while (1) {
    if ((nbytes = read(fd, &frame, sizeof(socket_frame_t))) <= 0) {
      continue;
    }
    if (nbytes > 0) {
      log_debug("nbytes :%d", nbytes);
      client->rcv_frame_cnt++;  // test use

      // log_debug("recive_frame:%d\r\n", can->rcv_frame_cnt);
      ret = insert_can_frame_recv_list(client, frame);
      if (ret < 0)
        log_debug("insert to frame list fail\n");
      else {
        log_debug("insert to frame list success\n");
      }
    }
    usleep(100);
    // if (get_send_frame(client, &frame) > 0) {
    //   ret = write_can_frame( client, &frame);
    //   if (ret < 0) {
    //     log_error("can write msg fail\n");
    //     return ret;
    //   }
    // }
  }

  log_debug("can massge recive thread is away\n");
  return NULL;
}

// open can
Client_t *open_client(void) {
  int ret = 0;
  Client_t *myclient = NULL;
  pthread_t rcv_id;

  // CAN molloc
  myclient = (Client_t *)malloc(sizeof(Client_t));
  if (myclient == NULL) {
    log_error("CAN molloc fail\n");
    return NULL;
  }

  log_debug("debug1");

  myclient->frame_recv_list = list_new();
  if (myclient->frame_recv_list == NULL) {
    log_error("can bus message frame_list malloc fail\n");
    free(myclient);
    return NULL;
  }
  myclient->frame_recv_list->free = free;
  myclient->frame_cnt = 0;
  myclient->frame_recv_list->len = 0;

  myclient->frame_send_list = list_new();
  if (myclient->frame_send_list == NULL) {
    log_error("can bus message frame_list malloc fail\n");
    free(myclient);
    return NULL;
  }
  myclient->frame_send_list->free = free;
  myclient->frame_send_list->len = 0;

  log_debug("frame_list len：%d", myclient->frame_recv_list->len);

  pthread_mutex_init(&myclient->frame_list_recv_mutex, NULL);
  pthread_mutex_init(&myclient->frame_list_send_mutex, NULL);

  ret = sem_init(&myclient->frame_sem, 0, 0);
  if (ret != 0) {
    log_error("CAN msg sem init fail\n");
    goto open_can_out;
  }

  myclient->socket_fd = open_can_socket();
  log_debug("socket id is %d", myclient->socket_fd);

  if (myclient->socket_fd < 0) {
    log_error("CAN socket open fail\n");
    goto open_can_out;
  }

  // open can massage frame recive thread
  pthread_attr_t attr;
  pthread_attr_init(&attr);
  pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
  if (pthread_create(&rcv_id, &attr, thread_can_frame_rcv, myclient) != 0) {
    log_error("Create  recive thread fail\n");
    goto open_can_out;
  }

  usleep(100);
  return myclient;
// error out
open_can_out:
  log_debug("OPEN OUT");
  pthread_mutex_destroy(&myclient->frame_list_recv_mutex);
  pthread_mutex_destroy(&myclient->frame_list_send_mutex);

  list_destroy(myclient->frame_recv_list);
  list_destroy(myclient->frame_send_list);

  free(myclient);
  return NULL;
}
// free can
static void free_can(Client_t **can) {
  Client_t *can_tmp = NULL;
  can_tmp = *can;
  list_destroy(can_tmp->frame_recv_list);
  list_destroy(can_tmp->frame_send_list);

  free(can_tmp);
  *can = NULL;
}

// close can
void close_can(Client_t *client) {
  // can recive frame thread free
  // close can socket
  close(client->socket_fd);
  sleep(1);
  pthread_mutex_destroy(&client->frame_list_recv_mutex);
  pthread_mutex_destroy(&client->frame_list_send_mutex);

  // can free
  free_can(&client);
}

/*------------------------------end file-----------------------------------*/