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

extern int path[8][4];
int status = 0;
int stop_pin = 0,scan_pin = 0,stop_compare = 0,scan_compare = 0;

int thing_flag=0;

#define EXT_PIN  GET_PIN(C, 8)
#define STA_PIN  GET_PIN(C, 7)
extern int path_num;
extern int path_switch;

extern int o_path_num;
extern int o_path[4];
void overthemap(void) //0022 1100
{
    path_num = 0;
    int temp[4]={0};
    for(int i=0;i<4;i++)
    {
        if(o_path[i]==1)
        {
            o_path[i]=2;
        }
        else if(o_path[i]==2)
        {
            o_path[i]=1;
        }
        temp[i] = o_path[i];
    }
    for(int i=0;i<4;i++)
    {
        o_path[i] = temp[3-i];
        rt_kprintf("%d, ",o_path[i]);
    }
    rt_kprintf("\r\n");
    rt_kprintf("over map ok!\r\n");
}


int stop_num = 0,take_num = 0,stop_flag = 0;
void irq_callback_test(void *args)
{
     stop_num++;
     if(stop_num == 1)
     {
        car_stop();
        overthemap();
        stop_pin = rt_pin_read(EXT_PIN);
        stop_flag = 1;
     }

}
int nums = 0,nums2 = 0,nums3 = 0;
void car_stop_entry(void *parameter)
{
    while(1)
    {
        for(int i=0;i<10;i++)
        {
             stop_compare = rt_pin_read(EXT_PIN);
             if((stop_pin!=stop_compare))
                 nums2++;
             if(nums2>=4)
             {
                 stop_flag = 0;
                 stop_num = 0;
                 nums2 = 0;
                 break;
             }
        }
        nums2 = 0;
        nums3 = 0;
        rt_thread_delay(200);
    }
}

void thing_scan_irq(void *args)
{
    take_num++;
    if(take_num == 1)
    {
        scan_pin = rt_pin_read(STA_PIN);
        status = 1;
    }
}

void  thing_scan_entry(void *parameter)
{
    while(1)
    {
        scan_compare = rt_pin_read(STA_PIN);
        if(scan_pin!=scan_compare)
            nums3++;
        if(nums3>=4)
        {
            status = 0;
            nums = 0;
            nums3 = 0;
            continue;
        }
        if((status == 1)&&(rt_pin_read(STA_PIN) == PIN_LOW))
        {
            for(int i=0;i<10;i++)
            {
                if((rt_pin_read(STA_PIN) == PIN_LOW))
                {
                    nums++;
                    if(nums>=9)
                    {
                         status = 2;
                         nums = 0;
                    }
                }

            }
            nums = 0;
            nums2 = 0;
            nums3 = 0;
        }
        if(status == 1 && stop_flag==0)
        {
            car_forward();
        }
        if(status == 2)
        {
            car_turn();
            stop_num = 0;
            take_num = 0;
            status = 0;
            stop_flag = 0;
        }
        rt_thread_delay(200);
    }
}
////1：正常启动 2：转弯
//void thing_scan_entry(void *parameter)
//{
//    while(1)
//    {
//
//        if(arrive_flag==0)
//        {
//            rt_kprintf("---now arrive flag is %d ---\r\n",arrive_flag);
//            LOG_D("a\r\n");
//            static int nums=0;
//            for(int i=0;i<10;i++)
//            {
//                if(rt_pin_read(STA_PIN)==PIN_HIGH)
//                {
//                    nums++;
//                    if(nums>=9)
//                    {
//                        nums=0;
//                        arrive_flag=2;
//                        LOG_D("now is forward\r\n");
//                        rt_kprintf("now is forward\r\n");
//                        car_forward();
//                    }
//                }
//                else {
//                    rt_thread_delay(200);
//                }
//            }
//            nums = 0;
//        }
//        else if(arrive_flag ==2)
//        {
//            rt_kprintf("---now arrive flag is %d ---\r\n",arrive_flag);
//            LOG_D("now is running\r\n");
//            rt_kprintf("now is running\r\n");
//            rt_thread_delay(1000);
//        }
//        else if(arrive_flag ==1)
//        {
//            rt_kprintf("---now arrive flag is %d ---\r\n",arrive_flag);
//            LOG_D("c\r\n");
//            static int nums=0;
//            for(int i=0;i<10;i++)
//            {
//                if(rt_pin_read(STA_PIN)==PIN_LOW)
//                {
//                    nums++;
//                    if(nums>=9)
//                    {
//                        nums=0;
//                        LOG_D("now is turn\r\n");
//                        rt_kprintf("now is turn\r\n");
//                        arrive_flag=2;
//                        if(arrive_flag==2)
//                        {
//                           overthemap();
//                           car_turn();
//                        }
//                        a=0;
//
//                    }
//                }
//                else {
//                    rt_thread_delay(200);
//                }
//            }
//            nums = 0;
//        }
//    }
//}
//int stop_numbers = 0;
//void car_stop_entry(void *parameter)
//{
//    while(1)
//    {
//        if(a==1)
//        {
//            car_stop();
//            static int nums=0;
//            stop_numbers ++;
//            if(stop_numbers == 2)
//            {
//                arrive_flag = 0;
//                stop_numbers = 0;
//            }
//            //rt_kprintf("should stop\r\n");
//            if(rt_pin_read(EXT_PIN)==PIN_LOW)
//            {
//                nums++;
//                if(nums==7)
//                {
//                    a=0;
//                    nums=0;
//                    if(arrive_flag == 2)
//                    arrive_flag=1;
//                    LOG_D("now is stop\r\n");
//                    rt_kprintf("now is stop\r\n");
//                    car_stop();
//
//                }
//            }
//            else
//            {
//                rt_thread_delay(10 );
//            }
//        }
//        else {
//            rt_thread_delay(500);
//        }
//    }
//}

int extern_interrupt_init(void)
{
    rt_err_t err=RT_EOK;

    rt_pin_mode(EXT_PIN, PIN_MODE_INPUT);
    rt_pin_mode(STA_PIN, PIN_MODE_INPUT);

    err = rt_pin_attach_irq(EXT_PIN, PIN_IRQ_MODE_RISING, irq_callback_test, RT_NULL);
    err = rt_pin_irq_enable(EXT_PIN, PIN_IRQ_ENABLE);

    err = rt_pin_attach_irq(STA_PIN, PIN_IRQ_MODE_RISING_FALLING, thing_scan_irq, RT_NULL);
    err = rt_pin_irq_enable(STA_PIN, PIN_IRQ_ENABLE);
    car_stop_thread = rt_thread_create("car_stop_thread", car_stop_entry, RT_NULL, 512, 0, 200);
    thing_scan_thread = rt_thread_create("thing_scan", thing_scan_entry, RT_NULL, 512, 7, 200);

    if(car_stop_thread != RT_NULL)
        rt_thread_startup(car_stop_thread);

    if(thing_scan_thread != RT_NULL)
        rt_thread_startup(thing_scan_thread);

    LOG_D("extern_interrupt_init ok");
    return err;
}


