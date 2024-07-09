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
 * @Date           : 2022-04-18 11:00:57
 * @LastEditors    : zirun
 * @LastEditTime   : 2022-04-18 11:01:01
 * @FilePath       : /lgcext/can_lib/ll_es_dcdc_prcess.c
 */
#include "ll_es_can_pro.h"

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <time.h>

#include "can_msg.h"
#include "dev_can.h"
#include "my_alg.h"

#define CMD_MODONOFF 0x01
#define CMD_CLEARFAILURE 0x02
#define CMD_ONOFFGRID 0x03
#define YT_CMD_SYNC 0x01
#define YT_CMD_OFFGRID_VSET 0x05
#define YT_CMD_ENABLE_DCDC 0x07
#define YT_CMD_BAT_CHG_VSET 0x70
#define YT_CMD_BAT_CHG_ISET 0x71
#define YT_CMD_BAT_DISCH_ISET 0x72
#define YT_CMD_BAT_LIMITED_UP_VSET 0x73
#define YT_CMD_BAT_LIMITED_DOWN_VSET 0x74
#define YT_CMD_DC_POWER 0x75
#define YT_CMD_EOD_SET 0x76
#define YT_CMD_ONGRID_SET 0x77
#define YT_CMD_OFFGRID_SET 0x78

#define YX_CMD_DC_STATUS 0x02
#define YX_CMD_DC_WARN 0x03
#define YX_CMD_DC_WARN1_3 0x04
#define YX_CMD_DC_WARN4_6 0x05

#define YC_CMD_PSV 0x02
#define YC_CMD_HEART 0x05
#define YC_CMD_BAT_VIP 0x10
#define YC_CMD_MLINE_VIP 0x11
#define YC_CMD_DC_CHG_ENERGY 0x14

extern DEV_DCDC_CanDataT g_dc_can_data;
extern int ll_socket_send(Clinet_tgcan *p_client, uint8_t msgclass,
                           uint8_t broadcast, uint8_t des_device,
                           uint8_t des_id, uint8_t par_addr, uint8_t *buf_in);
int ll_es_can_sync(Clinet_tgcan *p_client) {
  int ret = -1;
  uint8_t databuf[6];
  uint8_t dataout[16];

  time_t curTime = time(NULL);
  struct tm cur_tm;
  localtime_r(&curTime, &cur_tm);
  databuf[0] = (uint8_t)(cur_tm.tm_year & 0xff);
  databuf[1] = (uint8_t)(cur_tm.tm_mon & 0xff);
  databuf[2] = (uint8_t)(cur_tm.tm_mday & 0xff);
  databuf[3] = (uint8_t)(cur_tm.tm_hour & 0xff);
  databuf[4] = (uint8_t)(cur_tm.tm_min & 0xff);
  databuf[5] = (uint8_t)(cur_tm.tm_sec & 0xff);

  ret = ll_socket_send(p_client, RemoteSetupMsg, 1, DEV_ALL, 0, 0x01, databuf);

  return ret;
}

//
/**
 * @brief: ���ػ�����
 * @details: This is the detail description.
 * @param {uint16_t} mod_onoff  0���ػ� 1������  �㲥
 * @param out {*}
 * @return {*}
 */
int ll_es_can_power_onoff(Clinet_tgcan *p_client,uint16_t mod_onoff) {
  int ret = -1;
  uint8_t databuf[6];
  memset(databuf, 0, 6);
  log_info("can es power %s", mod_onoff ? "on" : "off");
  databuf[0] = (uint8_t)(mod_onoff >> 8);
  databuf[1] = (uint8_t)(mod_onoff & 0xff);
  ret =ll_socket_send(p_client,RemoteControlMsg, 0, DEV_DCDC, 1, 0x01, databuf);
  return ret;
}

/**
 * @brief: �����������
 * @details: (Para ID 0x02)
 * @param {uint16_t} flag  ;//0����Ч 1���������
 * @param out {*}
 * @return {*}success:0 fail:-1
 */
int ll_es_can_faultclear(Clinet_tgcan *p_client,uint16_t flag) {
  int ret = -1;
  uint8_t databuf[6];
  memset(databuf, 0, 6);
  databuf[0] = (uint8_t)(flag >> 8);
  databuf[1] = (uint8_t)(flag & 0xff);
  ret =ll_socket_send(p_client,RemoteControlMsg, 0, DEV_DCDC, 0, 0x02, databuf);
  return ret;
}
// 0������ 1������ �㲥
/**
 * @brief: ң�� ����������.
 * @details: ���� pq  ����VF
 * @param {uint16_t} flag //0������ 1������
 * @param out {*}
 * @return {*}
 */
int ll_es_can_onoffgrid(Clinet_tgcan *p_client,uint16_t flag) {
  int ret = -1;
  uint8_t databuf[6];
  memset(databuf, 0, 6);
  log_info("can es %s", flag ? "offgrid" : "ongrid");

  databuf[0] = (uint8_t)(flag >> 8);
  databuf[1] = (uint8_t)(flag & 0xff);
  ret =ll_socket_send(p_client,RemoteControlMsg, 0, DEV_ALL, 0, 0x03, databuf);
  return ret;
}
/**
 * @brief: This is a brief description.
 * @details: This is the detail description.
 * @param {uint16_t} vol
 * @param out {*}
 * @return {*}
 */
int ll_es_can_offgird_vset(Clinet_tgcan *p_client,uint16_t vol) {
  // ĸ�ߵ�ѹ���ã���Χ 250V~900V��*10 �·�
  int ret = -1;
  uint8_t databuf[6];
  memset(databuf, 0, 6);
  databuf[0] = (uint8_t)(vol >> 8);
  databuf[1] = (uint8_t)(vol & 0xff);
  ret =ll_socket_send(p_client,RemoteSetupMsg, 0, DEV_DCDC, 1, 0x05, databuf);
  return ret;
}
int ll_es_can_enable_dcmodule(Clinet_tgcan *p_client,uint16_t flag) {
  int ret = -1;
  uint8_t databuf[6];
  memset(databuf, 0, 6);
  databuf[0] = (uint8_t)(flag >> 8);
  databuf[1] = (uint8_t)(flag & 0xff);
  ret =ll_socket_send(p_client,RemoteSetupMsg, 0, DEV_DCDC, 1, 0x07, databuf);
  return ret;
}

int ll_es_can_enable_bmsmodule(Clinet_tgcan *p_client,uint16_t flag) {
  int ret = -1;
  uint8_t databuf[6];
  memset(databuf, 0, 6);
  databuf[0] = (uint8_t)(flag >> 8);
  databuf[1] = (uint8_t)(flag & 0xff);
  ret =ll_socket_send(p_client,RemoteSetupMsg, 0, DEV_DCDC, 1, 0x0b, databuf);
  return ret;
}
int ll_es_can_bat_CHG_vset(Clinet_tgcan *p_client,int16_t vol) {
  // ��ؾ����ѹ 200~850V��*10 �·�
  int ret = -1;
  uint8_t databuf[6];
  memset(databuf, 0, 6);
  databuf[0] = (uint8_t)(vol >> 8);
  databuf[1] = (uint8_t)(vol & 0xff);
  ret =ll_socket_send(p_client,RemoteSetupMsg, 0, DEV_DCDC, 1, 0x70, databuf);

  //  ret =ll_socket_send(p_client,RemoteSetupMsg, 0,
  //  DEV_DCDC, 1, 0x70, databuf);

  return ret;
}

int ll_es_can_max_CHG_current(Clinet_tgcan *p_client,uint16_t current) {
  int ret = -1;
  uint8_t databuf[6];
  memset(databuf, 0, 6);
  databuf[0] = (uint8_t)(current >> 8);
  databuf[1] = (uint8_t)(current & 0xff);
  ret =ll_socket_send(p_client,RemoteSetupMsg, 0, DEV_DCDC, 1, 0x71, databuf);
  return ret;
}

int ll_es_can_max_DISCH_current(Clinet_tgcan *p_client,uint16_t current) {
  int ret = -1;
  uint8_t databuf[6];
  memset(databuf, 0, 6);
  databuf[0] = (uint8_t)(current >> 8);
  databuf[1] = (uint8_t)(current & 0xff);
  ret =ll_socket_send(p_client,RemoteSetupMsg, 0, DEV_DCDC, 1, 0x72, databuf);
  return ret;
}

int ll_es_can_uplimited_vset(Clinet_tgcan *p_client,int16_t vol) {
  int ret = -1;
  uint8_t databuf[6];
  memset(databuf, 0, 6);
  databuf[0] = (uint8_t)(vol >> 8);
  databuf[1] = (uint8_t)(vol & 0xff);
  ret =ll_socket_send(p_client,RemoteSetupMsg, 0, DEV_DCDC, 1, 0x73, databuf);
  return ret;
}

int ll_es_can_downlimited_vset(Clinet_tgcan *p_client,int16_t vol) {
  int ret = -1;
  uint8_t databuf[6];
  memset(databuf, 0, 6);
  databuf[0] = (uint8_t)(vol >> 8);
  databuf[1] = (uint8_t)(vol & 0xff);
  ret =ll_socket_send(p_client,RemoteSetupMsg, 0, DEV_DCDC, 1, 0x74, databuf);
  return ret;
}

int ll_es_can_dc_power(Clinet_tgcan *p_client,int pw_set) {
  int ret = -1;
  int16_t power;
  uint8_t databuf[6];
  memset(databuf, 0, 6);
  power = (int16_t)(pw_set / 10);
  databuf[0] = (uint8_t)(power >> 8);
  databuf[1] = (uint8_t)(power & 0xff);
  log_info("can es power set:%d", pw_set);

  ret =ll_socket_send(p_client,RemoteSetupMsg, 0, DEV_DCDC, 1, 0x75, databuf);
  return ret;
}

int ll_es_can_eod(Clinet_tgcan *p_client,uint16_t vol) {
  int ret = -1;
  uint8_t databuf[6];
  memset(databuf, 0, 6);
  databuf[0] = (uint8_t)(vol >> 8);
  databuf[1] = (uint8_t)(vol & 0xff);
  ret =ll_socket_send(p_client,RemoteSetupMsg, 0, DEV_DCDC, 1, 0x76, databuf);
  return ret;
}

int ll_es_can_ongrid_dod(Clinet_tgcan *p_client,uint16_t vol) {
  int ret = -1;
  uint8_t databuf[6];
  memset(databuf, 0, 6);
  databuf[0] = (uint8_t)(vol >> 8);
  databuf[1] = (uint8_t)(vol & 0xff);
  ret =ll_socket_send(p_client,RemoteSetupMsg, 0, DEV_DCDC, 1, 0x77, databuf);
  return ret;
}

int ll_es_can_offgrid_dod(Clinet_tgcan *p_client,uint16_t vol) {
  int ret = -1;
  uint8_t databuf[6];
  memset(databuf, 0, 6);
  databuf[0] = (uint8_t)(vol >> 8);
  databuf[1] = (uint8_t)(vol & 0xff);
 ll_socket_send(p_client,RemoteSetupMsg, 0, DEV_DCDC, 1, 0x78, databuf);
  return ret;
}
int ll_es_can_set_sys_state(Clinet_tgcan *p_client,uint16_t hgcmd) {
  // 0 ��ĸ�� 4 ����ģʽ 8 �ȶ˿�
  int ret = -1;
  uint8_t databuf[6];
  memset(databuf, 0, 6);
  databuf[0] = (uint8_t)(hgcmd >> 8);
  databuf[1] = (uint8_t)(hgcmd & 0xff);
  log_info("can es state:%d", hgcmd);

  ret =ll_socket_send(p_client,RemoteSetupMsg, 0, DEV_DCDC, 1, 0x83, databuf);
  return ret;
}
int ll_es_can_get_runningflag(void) {
  int flag = false;
   if (BIT_GET(g_dc_can_data.dcdc_state1, 2)) {
    flag = true;
  } else {
    flag = false;
  }
   log_info("dcdc state :%x flag:%d", g_dc_can_data.dcdc_state1, flag);
  return flag;
}
