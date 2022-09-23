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
extern int arrive_flag;

rt_thread_t  car_stop_thread = RT_NULL;
rt_thread_t  thing_scan_thread = RT_NULL;

ALIGN(RT_ALIGN_SIZE)

extern int path[5];
int a = 0;
int thing_flag=0;

#define EXT_PIN  GET_PIN(C, 8)
#define STA_PIN  GET_PIN(C, 7)

void overthemap(void)
{
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
    rt_kprintf("over map ok!\r\n");
}

void irq_callback_test(void *args)
{
        a = 1;
    //car_stop();
}
//1：正常启动 2：转弯
void thing_scan_entry(void *parameter)
{
    while(1)
    {

        if(arrive_flag==0)
        {
            rt_kprintf("---now arrive flag is %d ---\r\n",arrive_flag);
            LOG_D("a\r\n");
            static int nums=0;
            for(int i=0;i<10;i++)
            {
                if(rt_pin_read(STA_PIN)==PIN_HIGH)
                {
                    nums++;
                    if(nums>=9)
                    {
                        nums=0;
                        arrive_flag=2;
                        LOG_D("now is forward\r\n");
                        rt_kprintf("now is forward\r\n");
                        car_forward();
                    }
                }
                else {
                    rt_thread_delay(200);
                }
            }
            nums = 0;
        }
        else if(arrive_flag ==2)
        {
            rt_kprintf("---now arrive flag is %d ---\r\n",arrive_flag);
            LOG_D("now is running\r\n");
            rt_kprintf("now is running\r\n");
            rt_thread_delay(1000);
        }
        else if(arrive_flag ==1)
        {
            rt_kprintf("---now arrive flag is %d ---\r\n",arrive_flag);
            LOG_D("c\r\n");
            static int nums=0;
            for(int i=0;i<10;i++)
            {
                if(rt_pin_read(STA_PIN)==PIN_LOW)
                {
                    nums++;
                    if(nums>=9)
                    {
                        nums=0;
                        LOG_D("now is turn\r\n");
                        rt_kprintf("now is turn\r\n");
                        arrive_flag=2;
                        if(arrive_flag==2)
                        {
                           overthemap();
                           car_turn();
                        }
                        a=0;

                    }
                }
                else {
                    rt_thread_delay(200);
                }
            }
            nums = 0;
        }
    }
}
int stop_numbers = 0;
void car_stop_entry(void *parameter)
{
    while(1)
    {
        if(a==1)
        {
            car_stop();
            static int nums=0;
            stop_numbers ++;
            if(stop_numbers == 2)
            {
                arrive_flag = 0;
                stop_numbers = 0;
            }
            //rt_kprintf("should stop\r\n");
            if(rt_pin_read(EXT_PIN)==PIN_LOW)
            {
                nums++;
                if(nums==7)
                {
                    a=0;
                    nums=0;
                    if(arrive_flag == 2)
                    arrive_flag=1;
                    LOG_D("now is stop\r\n");
                    rt_kprintf("now is stop\r\n");
                    car_stop();

                }
            }
            else
            {
                rt_thread_delay(10 );
            }
        }
        else {
            rt_thread_delay(500);
        }
    }
}

int extern_interrupt_init(void)
{
    rt_err_t err=RT_EOK;

    rt_pin_mode(EXT_PIN, PIN_MODE_INPUT);
    rt_pin_mode(STA_PIN, PIN_MODE_INPUT);

    err = rt_pin_attach_irq(EXT_PIN, PIN_IRQ_MODE_RISING, irq_callback_test, RT_NULL);
    err = rt_pin_irq_enable(EXT_PIN, PIN_IRQ_ENABLE);

    car_stop_thread = rt_thread_create("car_stop_thread", car_stop_entry, RT_NULL, 512, 0, 200);
    thing_scan_thread = rt_thread_create("thing_scan", thing_scan_entry, RT_NULL, 512, 5, 200);

    if(car_stop_thread != RT_NULL)
        rt_thread_startup(car_stop_thread);

    if(thing_scan_thread != RT_NULL)
        rt_thread_startup(thing_scan_thread);

    LOG_D("extern_interrupt_init ok");
    return err;
}


