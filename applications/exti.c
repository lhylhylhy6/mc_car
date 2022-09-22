#include <exti.h>
/*
 * Copyright (c) 2006-2021, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2022-09-21     lhylh       the first version
 */
#include "car.h"
#define DBG_TAG "exti"
#define DBG_LVL DBG_LOG
#include <rtdbg.h>

struct rt_completion turn_flag;

rt_thread_t  car_stop_thread = RT_NULL;
rt_thread_t  thing_scan_thread = RT_NULL;
rt_sem_t thing_scan_sem =RT_NULL;

ALIGN(RT_ALIGN_SIZE)

extern int path[5];
int a = 0,stop_flag = 0;
int thing_flag=0;

#define EXT_PIN  GET_PIN(C, 8)
#define STA_PIN  GET_PIN(C, 9)

void irq_callback_test(void *args)
{
        a = 1;
}

void irq_sta_callback(void *args)
{
    rt_sem_release(thing_scan_sem);
}

void thing_scan_entry(void *parameter)
{
    static int thing_times=0;
    while(1)
    {
        rt_sem_take(thing_scan_sem, RT_WAITING_FOREVER);
        thing_times++;
        thing_times %= 2;
        if (thing_times==0)
        {
            rt_thread_delay(500);
            car_turn();
        }
        else
        {
            rt_thread_delay(500);
            car_forward();
        }
        rt_sem_release(thing_scan_sem);
    }
}

void car_stop_entry(void *parameter)
{
    if((a == 1)&&(stop_flag == 0))
    {
        car_stop();
        a=0;
        stop_flag = 1;
        rt_kprintf("stop\r\n");
    int temp[4]={0};
    for(int i=0;i<4;i++)
    {
        if(path[i]==1)
        {
            path[i]=2;
        }
        else if(path[i]==2)
        {
            path[i]=1;
        }
        temp[i] = path[i];
    }
    for(int i=0;i<4;i++)
    {
        path[i] = temp[3-i];
        LOG_D("%d, ",path[i]);
    }
    LOG_D("\r\n");
    }
}

int extern_interrupt_init(void)
{
    rt_err_t err=RT_EOK;

    rt_pin_mode(EXT_PIN, PIN_MODE_INPUT);
    rt_pin_mode(STA_PIN, PIN_MODE_INPUT);

    err = rt_pin_attach_irq(EXT_PIN, PIN_IRQ_MODE_RISING, irq_callback_test, RT_NULL);
    err = rt_pin_attach_irq(STA_PIN, PIN_IRQ_MODE_RISING_FALLING, irq_sta_callback, RT_NULL);

    thing_scan_sem = rt_sem_create("thing_scan", 0, RT_IPC_FLAG_PRIO);

    car_stop_thread = rt_thread_create("car_stop_thread", car_stop_entry, RT_NULL, 512, 0, 20);
    thing_scan_thread = rt_thread_create("thing_scan", thing_scan_entry, RT_NULL, 512, 0, 20);

    if(car_stop_thread != RT_NULL)
        rt_thread_startup(car_stop_thread);
    else
    {
        LOG_D("car_stop_thread error\r\n");
    }

    if(car_stop_thread != RT_NULL)
        rt_thread_startup(thing_scan_thread);
    else
    {
        LOG_D("thing_scan_thread error\r\n");
    }

    if(RT_EOK != err)
    {
        LOG_D("\n attach irq failed. \n");
    }

    err = rt_pin_irq_enable(EXT_PIN, PIN_IRQ_ENABLE);
    err = rt_pin_irq_enable(STA_PIN, PIN_IRQ_ENABLE);

    if(RT_EOK != err)
    {
        LOG_D("\n enable irq failed. \n");
    }
    return err;
}


