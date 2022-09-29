/*
 * Copyright (c) 2006-2021, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2022-09-25     lhylh       the first version
 */
#include "uart2.h"

#define keyward_uart_name "uart3"

rt_sem_t rxx_sem;
rt_device_t keyward_uart = RT_NULL;
rt_thread_t keyward_read_thread;

rt_uint8_t temp_flag=0;
rt_uint8_t temp_info[2];

rt_uint8_t cross_num=1;
rt_uint8_t info_num=0;

rt_uint8_t first_num=0;
rt_uint8_t map_info[5][4][2];

rt_err_t keyward_uart_rx_inter(rt_device_t dev,rt_size_t size)
{
    rt_sem_release(rxx_sem);
    return RT_EOK;
}

//检查是否重复以及把数据插入map_info中
void check_and_insert(void)
{
    rt_uint8_t fflag=0;
    int i=0;
    for(i=0;i<4;i++)
    {
        if(map_info[cross_num][i][0]==0)
        {
            break;
        }
        if(map_info[cross_num][i][0]==temp_info[0])
        {
            fflag=1;
            break;
        }
    }
    if(fflag!=1)
    {
        map_info[cross_num][i][0]=temp_info[0];
        map_info[cross_num][i][1]=temp_info[1];
    }
}

extern rt_uint8_t cross_num;
rt_uint8_t first_flag=0;
rt_uint8_t cross_enable=0;
void keyward_read_entry(void *parameter)
{
      char ch;
      static rt_uint32_t temp_number=0;

      while(1)
     {

          while(rt_device_read(keyward_uart, -1, &ch, 1)!=1)
          {
             rt_sem_take(rxx_sem, RT_WAITING_FOREVER);
          }
          rt_kprintf("%c",ch);
          if(ch=='[')
          {
              temp_flag=0;
              temp_number=0;
          }
          else if(ch=='!')
          {
              first_flag=1;
              cross_enable=1;
          }
          else if(ch==']')
          {
              if(first_flag==1)
              {
                  first_num = temp_number;
                  rt_kprintf("-------------first_num is %d\r\n",first_num);
                  cross_num=0;
                  first_flag=0;
              }
              else
              {
                  temp_info[1]=temp_number;
                  temp_number=0;
                  //check_and_insert();
              }
          }
          else if(ch==',')
          {
              temp_info[0]=temp_number;
              temp_number=0;
          }
          else if(ch>='0'&&ch<='9')
          {
              temp_number=ch-'0';
          }
          ch=0;

     }

}

rt_err_t keyward_uart_init(void)
{
    rt_err_t ret = RT_EOK;
    char start_info[2]="a";
    rxx_sem = rt_sem_create("keyward_rx", 0, RT_IPC_FLAG_PRIO);
    keyward_uart = rt_device_find(keyward_uart_name);
    rt_device_write(keyward_uart, 0, start_info, 2);
    if(keyward_uart)
    {
        ret = rt_device_open(keyward_uart, RT_DEVICE_FLAG_INT_RX);
    }
    else {
        rt_kprintf("error keyward_uart find\r\n");
        return -1;
    }
    rt_device_set_rx_indicate(keyward_uart, keyward_uart_rx_inter);
    keyward_read_thread = rt_thread_create("keyward_read_thread", keyward_read_entry, 0, 1024, 5, 300);
    if(keyward_read_thread)
    {
        rt_thread_startup(keyward_read_thread);
    }
    else {
        rt_kprintf("create keyward_read_thread error\r\n");
    }
    return ret;

}

