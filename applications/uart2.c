/*
 * Copyright (c) 2006-2021, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2022-09-20     10091       the first version
 */
#include "uart2.h"
#include "car.h"

rt_sem_t rx_sem;
rt_device_t pid_uart = RT_NULL;
rt_mutex_t number_protect = RT_NULL;
rt_uint32_t number = 0;
rt_thread_t pid_read_thread;

extern rt_uint8_t return_flag;

extern rt_uint8_t cross_num;
extern rt_uint8_t first_num;
extern rt_uint8_t map_info[5][4][2];

int o_path_num=0;
int o_path[4];

int path[8][4]={
        {1,1,1,1},
        {2,2,2,2},
        {0,1,0,1},
        {0,2,0,2},
        {0,0,1,1},
        {0,0,1,2},
        {0,0,2,1},
        {0,0,2,2}
};
int path_num=0;
int path_switch=0;

int select_side(void)
{
    int renum=0;
    for(int i=0;i<4;i++)
    {
        if(map_info[cross_num-1][i][0]==first_num)
        {
            renum = map_info[cross_num-1][i][1];
            break;
        }
    }
    return renum;
}
rt_err_t pid_uart_rx_inter(rt_device_t dev,rt_size_t size)
{
    rt_sem_release(rx_sem);
    return RT_EOK;
}

void pid_read_entry(void *parameter)
{
      char ch;
      static rt_uint32_t temp_number=0;
      while(1)
     {

         while(rt_device_read(pid_uart, -1, &ch, 1)!=1)
         {
             rt_sem_take(rx_sem, RT_WAITING_FOREVER);
         }

         if(ch=='[')
         {
             //temp_number = 0 ;
         }
         else if(ch==']')
         {
             rt_mutex_take(number_protect, RT_WAITING_FOREVER);
             number = temp_number;
             temp_number = 0 ;
             //rt_kprintf("%d",number);
             rt_mutex_release(number_protect);

         }
         else if(ch=='c')
         {
             rt_kprintf("*********is cross *********\r\n");
             if(return_flag==0)
             {
                 if(first_num==1||first_num==2)
                 {
                     switch(first_num)
                   {
                       case 1:car_left();break;
                       case 2:car_right();break;
                   }
                     rt_kprintf("%d\r\n",first_num);
                 }
                 else
                 {
                      cross_num++;
                      int side = select_side();
                      if(cross_num==4)
                      {
                          if(side==0)
                          {
                              int ii=0,l_num=0;
                              for(ii=0;ii<4;ii++)
                              {
                                  if(map_info[cross_num-1][ii][1]==1)
                                          l_num++;
                              }
                              if(l_num==1)
                                  side=1;
                              else
                                  side=2;
                          }
                      }
                      o_path[o_path_num]=side;
                      o_path_num++;
                      switch(side)
                      {
                          case 0:break;
                          case 1:car_left();break;
                          case 2:car_right();break;
                      }
                      rt_kprintf("%d\r\n",side);
                 }
             }
             else
             {
                 switch(o_path[o_path_num])
               {
                   case 0:break;
                   case 1:car_left();break;
                   case 2:car_right();break;
               }
                 o_path_num++;
             }
         }
         else if(ch>='0'&&ch<='9')
         {
             temp_number=temp_number *10 + ch-'0';
         }
         ch=0;
     }

}

rt_err_t pid_uart_init(void)
{
    rt_err_t ret = RT_EOK;
    rx_sem = rt_sem_create("pid_rx", 0, RT_IPC_FLAG_PRIO);
    number_protect = rt_mutex_create("number", RT_IPC_FLAG_PRIO);
    pid_uart = rt_device_find(pid_uart_name);
    if(pid_uart)
    {
        ret = rt_device_open(pid_uart, RT_DEVICE_FLAG_INT_RX);
    }
    else {
        rt_kprintf("error pid_uart find\r\n");
        return -1;
    }
    rt_device_set_rx_indicate(pid_uart, pid_uart_rx_inter);
    pid_read_thread = rt_thread_create("pid_read_thread", pid_read_entry, 0, 1024, 5, 300);
    if(pid_read_thread)
    {
        rt_thread_startup(pid_read_thread);
    }
    else {
        rt_kprintf("create pid_read_thread error\r\n");
    }
    return ret;

}
