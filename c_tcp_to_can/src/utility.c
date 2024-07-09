/*
 * @Author: zouzirun zouzirun@163.com
 * @Date: 2024-07-03 17:15:01
 * @LastEditors: zouzirun zouzirun@163.com
 * @LastEditTime: 2024-07-05 16:45:09
 * @FilePath: \c_tcp_to_can\src\utility.c
 * @Description: 这是默认设置,请设置`customMade`, 打开koroFileHeader查看配置 进行设置: https://github.com/OBKoro1/koro1FileHeader/wiki/%E9%85%8D%E7%BD%AE
 */
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

  int istrToarray(char *str_buf, int *data, int *dlen)
{
    char *token;
     int num = 0;
    token = strtok(str_buf, " ");
    while (token != NULL)
    {
        *data++ = atoi(token);
        num++;
        token = strtok(NULL, " ");
    }
    printf("num is %d \r\n", num);
    *dlen = num;
    return 0;
}