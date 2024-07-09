/*
 * @Author: zouzirun zouzirun@163.com
 * @Date: 2024-07-09 14:08:47
 * @LastEditors: zouzirun zouzirun@163.com
 * @LastEditTime: 2024-07-09 14:17:41
 * @FilePath: \c_tcp_to_can_r\inc\ll_es_can_pro.h
 * @Description: 这是默认设置,请设置`customMade`, 打开koroFileHeader查看配置 进行设置: https://github.com/OBKoro1/koro1FileHeader/wiki/%E9%85%8D%E7%BD%AE
 */

#ifndef _LL_ES_CAN_PRO_H_
#define _LL_ES_CAN_PRO_H_

#include <stdint.h>
#include "dev_can.h"

#ifdef __cplusplus
extern "C" {
#endif


typedef struct {
  uint16_t dev_online;
  // yx
  uint16_t dcdc_state1;
  uint16_t dcdc_alarm1;
  uint16_t dcdc_alarm2;
  uint16_t dcdc_alram3;
  uint16_t dcdc_fault1;
  uint16_t dcdc_fault2;
  uint16_t bms_state1;
  uint16_t bms_alarm1;
  uint16_t bms_protect1;
  uint16_t bms_fault1;

  // yc
  uint16_t u16Pversion;
  uint16_t u16Sversion;
  uint16_t u16Vversion;
  uint16_t u16Aversion;
  uint16_t u16Dversion;
  uint16_t u16HardWare_Version;
  uint16_t heartbeat;
  int16_t bat_voltage;
  int16_t bat_current;
  int16_t bat_power;
  int16_t bus_voltage;
  int16_t bus_current;
  int16_t bus_power;
  int16_t bak1;
  int32_t energr_for_CHG;
  int32_t energr_for_DISCH;

  int16_t env_temp;
  int16_t fanSpeed;
  int16_t heatsinkTemp1;
  int16_t heatsinkTemp2;
  int16_t heatsinkTemp3;
  int16_t heatsinkTemp4;
  int16_t heatsinkTemp5;
  int16_t heatsinkTemp6;
  int16_t heatsinkTemp7;
  int16_t heatsinkTemp8;
  int16_t heatsinkTemp9;
  int16_t bms_bat_totalvol;

  int32_t bms_bat_totalcur;

  int16_t bms_soc;
  int16_t bms_soh;
  int16_t bms_env_temp;
  int16_t bms_sigleMinVol;
  int16_t bms_sigleMinVolNum;
  int16_t bms_sigleMaxVol;
  int16_t bms_sigleMaxVolNum;
  int16_t bms_sigleMinTemp;
  int16_t bms_sigleMinTempNum;
  int16_t bms_sigleMaxTemp;       // ������ߵ���¶�
  int16_t bms_sigleMaxTempNum;    // ������ߵ���¶ȱ��
  int16_t bms_voldiff;            // ���ѹ��
  int16_t bms_tempdiff;           // ����²�
  int16_t bms_chgVolLimit;        // ������Ƶ�ѹ
  int16_t bms_dischgVolLimit;     // �ŵ����Ƶ�ѹ
  int16_t bms_chgcurrentLimit;    // ������Ƶ���
  int16_t bms_dischgcurrentLimit; // �ŵ����Ƶ���

} DEV_DCDC_CanDataT;
// msg_class
 int ll_es_can_power_onoff (Clinet_tgcan *p_client,uint16_t mod_onoff);
 int ll_es_can_faultclear (Clinet_tgcan *p_client,uint16_t flag);
 int ll_es_can_onoffgrid (Clinet_tgcan *p_client,uint16_t flag);
int ll_es_can_sync (Clinet_tgcan *p_client);
int ll_es_can_offgird_vset (Clinet_tgcan *p_client,uint16_t vol);
int ll_es_can_enable_dcmodule (Clinet_tgcan *p_client,uint16_t flag);
int ll_es_can_enable_bmsmodule (Clinet_tgcan *p_client,uint16_t flag);
int ll_es_can_bat_CHG_vset (Clinet_tgcan *p_client,int16_t vol);
int ll_es_can_max_CHG_current (Clinet_tgcan *p_client,uint16_t current);
int ll_es_can_max_DISCH_current (Clinet_tgcan *p_client,uint16_t current);
int ll_es_can_uplimited_vset (Clinet_tgcan *p_client,int16_t vol);
int ll_es_can_downlimited_vset (Clinet_tgcan *p_client,int16_t vol);
int ll_es_can_dc_power (Clinet_tgcan *p_client,int pw_set);
int ll_es_can_eod (Clinet_tgcan *p_client,uint16_t vol);
int ll_es_can_ongrid_dod (Clinet_tgcan *p_client,uint16_t vol);
int ll_es_can_offgrid_dod (Clinet_tgcan *p_client,uint16_t vol);
int ll_es_can_set_sys_state (Clinet_tgcan *p_client,uint16_t hgcmd);

#ifdef __cplusplus
}
#endif

#endif