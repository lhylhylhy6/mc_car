/*
 * Copyright (c) 2006-2021, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2022-09-20     10091       the first version
 */
#include "pid.h"

extern rt_uint32_t number;
extern rt_uint32_t period,pulse;
extern rt_mutex_t number_protect;

extern struct rt_device_pwm * pwm1 ;
extern struct rt_device_pwm * pwm2 ;

rt_int32_t pwm_l,pwm_r;
rt_int32_t speed;
float middle = 180.0;
float kp=650;
float ki = 0.0;
float kd = 1.7;
float dia=0;

rt_thread_t pid_thread = RT_NULL;

void pid_thread_entry(void *parameter)
{
    while(1)
    {
        rt_uint32_t num=0;
        num=0;
        speed = period*pulse;
        rt_mutex_take(number_protect, RT_WAITING_FOREVER);
        num = number;
        rt_mutex_release(number_protect);
        //pid_compute(num);
        rt_thread_mdelay(30);
    }
}

int pid_init(void)
{
    pid_thread = rt_thread_create("pid_thread", pid_thread_entry, RT_NULL, 1024, 9, 300);
    if(pid_thread)
    {
        rt_thread_startup(pid_thread);
    }
    else {
        rt_kprintf("create pid_thread error\r\n");
        return -RT_ERROR;
    }
    return RT_EOK;
}






