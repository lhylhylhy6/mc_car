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

rt_sem_t rx_sem;
rt_device_t keyward_uart = RT_NULL;
rt_thread_t keyward_read_thread;

rt_err_t keyward_uart_rx_inter(rt_device_t dev,rt_size_t size)
{
    rt_sem_release(rx_sem);
    return RT_EOK;
}

void keyward_read_entry(void *parameter)
{
      char ch;

      while(1)
     {

         while(rt_device_read(keyward_uart, -1, &ch, 1)!=1)
         {
             rt_sem_take(rx_sem, RT_WAITING_FOREVER);
         }
         rt_kprintf("--%c--\r\n",ch);


     }

}

rt_err_t keyward_uart_init(void)
{
    rt_err_t ret = RT_EOK;
    rx_sem = rt_sem_create("keyward_rx", 0, RT_IPC_FLAG_PRIO);
    keyward_uart = rt_device_find(keyward_uart_name);
    if(keyward_uart)
    {
        ret = rt_device_open(keyward_uart, RT_DEVICE_FLAG_INT_RX);
    }
    else {
        rt_kprintf("error keyward_uart find\r\n");
        return -1;
    }
    rt_device_set_rx_indicate(keyward_uart, keyward_uart_rx_inter);
    keyward_read_thread = rt_thread_create("keyward_read_thread", keyward_read_entry, 0, 1024, 3, 300);
    if(keyward_read_thread)
    {
        rt_thread_startup(keyward_read_thread);
    }
    else {
        rt_kprintf("create keyward_read_thread error\r\n");
    }
    return ret;

}

