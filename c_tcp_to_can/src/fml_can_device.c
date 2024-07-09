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
 * @Date           : 2022-04-15 17:41:46
 * @LastEditors    : zirun
 * @LastEditTime   : 2022-04-15 17:41:50
 * @FilePath       : /lgcext/can_lib/apl_can_device.c
 */

#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <byteswap.h>
#include "can_msg.h"
#include "dev_can.h"
#include "ll_es_can_pro.h"
#include "my_alg.h"

 extern DEV_DCDC_CanDataT g_dc_can_data;
static pthread_t can_cycle_thread_id; 
void RemoteControlMsg_parse(uint8_t src_dev_type, uint8_t par_id,
                            uint8_t *datbuf) {
  uint16_t *pu16data = (uint16_t *)datbuf;
  if (src_dev_type == DEV_DCDC) {
    log_info("ACK U16 data:%04x", *pu16data);
  }
  return;
}
void RemoteSetupMsg_parse(uint8_t src_dev_type, uint8_t par_id,
                          uint8_t *datbuf) {
  uint16_t *pu16data = (uint16_t *)datbuf;
  if (src_dev_type == DEV_DCDC) {
    log_info("ACK U16 data:%04x", *pu16data);
  }
  return;
}

void RemoteSignalMsg_parse(uint8_t src_dev_type, uint8_t par_id,
                           uint8_t *datbuf) {
  // uint16_t *pu16data = (uint16_t *)datbuf;
  static int print_ticks = 0;
  print_ticks++;
  if (src_dev_type == DEV_DCDC) {
    switch (par_id) {
      case 0x02:
        g_dc_can_data.dcdc_state1 = (uint16_t)((datbuf[0] << 8) | datbuf[1]);
        if (print_ticks > 10) {
          print_ticks = 0;
          log_info("dcdc state1:0x%04x fault:%d warn:%d run:%d",
                      g_dc_can_data.dcdc_state1,
                      BIT_GET(g_dc_can_data.dcdc_state1, 0),
                      BIT_GET(g_dc_can_data.dcdc_state1, 1),
                      BIT_GET(g_dc_can_data.dcdc_state1, 2));
          if (g_dc_can_data.dcdc_state1 & 0x01) {
            log_info("dcdc fault1:0x%04x  fault2:0x%04x",
                        g_dc_can_data.dcdc_fault1, g_dc_can_data.dcdc_fault2);
          }
          if (g_dc_can_data.dcdc_state1 & 0x02) {
            log_info("dcdc alarm1:0x%04x  ", g_dc_can_data.dcdc_alarm1);
          }
        }

        break;
      case 0x03:
        g_dc_can_data.dcdc_alarm1 = (uint16_t)((datbuf[0] << 8) | datbuf[1]);

        break;
      case 0x04:
        g_dc_can_data.dcdc_fault1 = (uint16_t)((datbuf[0] << 8) | datbuf[1]);
        g_dc_can_data.dcdc_fault2 = (uint16_t)((datbuf[2] << 8) | datbuf[3]);

        break;
      default:
        break;
    }
  } else if (src_dev_type == DEV_BMS) {
    switch (par_id) {
      case 0x01:
        g_dc_can_data.bms_state1 = (uint16_t)((datbuf[0] << 8) | datbuf[1]);
        break;
      case 0x02:
        g_dc_can_data.bms_alarm1 = (uint16_t)((datbuf[0] << 8) | datbuf[1]);
        if (g_dc_can_data.bms_state1 & 0x02) {
          log_info("bms bms_alarm1:0x%04x   ", g_dc_can_data.bms_alarm1);
        }
        break;
      case 0x03:
        g_dc_can_data.bms_protect1 = (uint16_t)((datbuf[0] << 8) | datbuf[1]);

        break;
      case 0x04:
        g_dc_can_data.bms_fault1 = (uint16_t)((datbuf[0] << 8) | datbuf[1]);
        if (g_dc_can_data.bms_state1 & 0x01) {
          log_info("bms_fault1:0x%04x ", g_dc_can_data.bms_fault1);
        }
        break;
      default:
        break;
    }
  }
  return;
}

void RemoteMeterMsg_parse(uint8_t src_dev_type, uint8_t par_id,
                          uint8_t *datbuf) {
  uint16_t *pu16data = (uint16_t *)datbuf;
  static int print_ticks = 0;
  // log_info("SRC TYPE:0x%02x id:0x%02x", src_dev_type, par_id);
  print_ticks++;
  if (src_dev_type == DEV_DCDC) {
    if (print_ticks > 30) {
      print_ticks = 0;
      log_info("dcdc bat v:%d c:%d p:%d", g_dc_can_data.bat_voltage,
                  g_dc_can_data.bat_current, g_dc_can_data.bat_power);
    }
    switch (par_id) {
      case 0x02:
        g_dc_can_data.u16Pversion = bswap_16(*(pu16data));
        g_dc_can_data.u16Sversion = bswap_16(*(pu16data + 1));
        g_dc_can_data.u16Vversion = bswap_16(*(pu16data + 2));
        break;
      case 0x03:
        g_dc_can_data.u16Aversion = bswap_16(*(pu16data));
        g_dc_can_data.u16Dversion = bswap_16(*(pu16data + 1));
        g_dc_can_data.u16HardWare_Version = bswap_16(*(pu16data + 2));
        break;
      case 0x05:
        g_dc_can_data.heartbeat = bswap_16(*(pu16data));

        break;
      case 0x10:
        g_dc_can_data.bat_voltage = bswap_16(*(pu16data));
        g_dc_can_data.bat_current = bswap_16(*(pu16data + 1));

        g_dc_can_data.bat_power = (uint16_t)(datbuf[4] << 8 | datbuf[5]);

        break;
      case 0x11:
        g_dc_can_data.bus_voltage = bswap_16(*(pu16data));
        g_dc_can_data.bus_current = bswap_16(*(pu16data + 1));
        g_dc_can_data.bus_power = bswap_16(*(pu16data + 2));

        log_info("dcdc bus v:%d c:%d p:%d  ", g_dc_can_data.bus_voltage,
                    g_dc_can_data.bus_current, g_dc_can_data.bus_power);
        log_info("dcdc hex-- %02x %02x %02x %02x %02x %02x", datbuf[0],
                    datbuf[1], datbuf[2], datbuf[3], datbuf[4], datbuf[5]);

        break;
      case 0x14:
        g_dc_can_data.energr_for_CHG =
            (bswap_16(*(pu16data)) << 16) | (bswap_16(*(pu16data + 1)));
        // log_info("dcdc charge 0x%x 0x%x", *(pu16data), *(pu16data +
        // 1));
        break;
      case 0x15:
        g_dc_can_data.energr_for_DISCH =
            (bswap_16(*(pu16data)) << 16) | (bswap_16(*(pu16data + 1)));
        // log_info("dcdc discharge 0x%x 0x%x", *(pu16data),
        // *(pu16data + 1));
        break;
      case 0x16:
        g_dc_can_data.env_temp = bswap_16(*(pu16data));
        g_dc_can_data.fanSpeed = bswap_16(*(pu16data + 1));

        break;
      case 0x17:
        g_dc_can_data.heatsinkTemp1 = bswap_16(*(pu16data));
        g_dc_can_data.heatsinkTemp2 = bswap_16(*(pu16data + 1));
        g_dc_can_data.heatsinkTemp3 = bswap_16(*(pu16data + 2));

        break;
      case 0x18:
        g_dc_can_data.heatsinkTemp4 = bswap_16(*(pu16data));
        g_dc_can_data.heatsinkTemp5 = bswap_16(*(pu16data + 1));
        g_dc_can_data.heatsinkTemp6 = bswap_16(*(pu16data + 2));
        break;
      case 0x19:
        g_dc_can_data.heatsinkTemp7 = bswap_16(*(pu16data));
        g_dc_can_data.heatsinkTemp8 = bswap_16(*(pu16data + 1));
        g_dc_can_data.heatsinkTemp9 = bswap_16(*(pu16data + 2));
        break;
      default:

        break;
    }
  } else if (src_dev_type == DEV_BMS) {
    switch (par_id) {
      case 0x03:
        g_dc_can_data.bms_bat_totalvol = bswap_16(*(pu16data));
        g_dc_can_data.bms_bat_totalcur =
            (bswap_16(*(pu16data + 1)) << 16) | (bswap_16(*(pu16data + 2)));
        break;
      case 0x04:
        g_dc_can_data.bms_soc = bswap_16(*(pu16data));
        g_dc_can_data.bms_soh = bswap_16(*(pu16data + 1));
        g_dc_can_data.bms_env_temp = bswap_16(*(pu16data + 2));
        break;
      case 0x05:
        g_dc_can_data.bms_sigleMinVol = bswap_16(*(pu16data));
        g_dc_can_data.bms_sigleMinVolNum = bswap_16(*(pu16data + 1));
        break;
      case 0x06:
        g_dc_can_data.bms_sigleMaxVol = bswap_16(*(pu16data));
        g_dc_can_data.bms_sigleMaxVolNum = bswap_16(*(pu16data + 1));
        break;
      case 0x07:
        g_dc_can_data.bms_sigleMinTemp = bswap_16(*(pu16data));
        g_dc_can_data.bms_sigleMinTempNum = bswap_16(*(pu16data + 1));
        break;
      case 0x08:
        g_dc_can_data.bms_sigleMaxTemp =
            bswap_16(*(pu16data));  // ������ߵ���¶�
        g_dc_can_data.bms_sigleMaxTempNum = bswap_16(*(pu16data + 1));
        // ������ߵ���¶ȱ��
        break;
      case 0x0B:

        g_dc_can_data.bms_voldiff = bswap_16(*(pu16data));  // ���ѹ��
        g_dc_can_data.bms_tempdiff = bswap_16(*(pu16data + 1));  // ����²�
        break;
      case 0x0C:
        g_dc_can_data.bms_chgVolLimit = bswap_16(*(pu16data));  // ������Ƶ�ѹ
        g_dc_can_data.bms_dischgVolLimit = bswap_16(*(pu16data + 1));
        // �ŵ����Ƶ�ѹ
      case 0x0D:
        g_dc_can_data.bms_chgcurrentLimit =
            bswap_16(*(pu16data));  // ������Ƶ���
        g_dc_can_data.bms_dischgcurrentLimit = bswap_16(*(pu16data + 1));
        // �ŵ����Ƶ���
        break;
      default:
        break;
    }
  }
  return;
}
 int can_device_msg_parse(CAN_UserMsg_t *msg) {
  log_info("src_device %02x,MSG CLASS:%02x MSG  par:%02x",
              msg->msg_id.src_device, msg->msg_id.msg_class,
              msg->msg_data.par_addr);
  switch (msg->msg_id.msg_class) {
    case RemoteControlMsg:
      // 0x01      //ң������
      RemoteControlMsg_parse(msg->msg_id.src_device, msg->msg_data.par_addr,
                             msg->msg_data.data);
      break;
    case RemoteSetupMsg:
      // 0x02        //ң������
      RemoteSetupMsg_parse(msg->msg_id.src_device, msg->msg_data.par_addr,
                           msg->msg_data.data);
      break;
    case RemoteSignalMsg:
      // 0x03       // ң������
      RemoteSignalMsg_parse(msg->msg_id.src_device, msg->msg_data.par_addr,
                            msg->msg_data.data);
      break;
    case RemoteMeterMsg:
      // 0x04        //ң������
      RemoteMeterMsg_parse(msg->msg_id.src_device, msg->msg_data.par_addr,
                           msg->msg_data.data);
      break;
    // case CorrectionCoefficient:

    //   break;
    default:
      break;
  }
  return 0;
} 
