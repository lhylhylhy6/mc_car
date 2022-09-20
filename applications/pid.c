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

#define DBG_TAG "pid"
#define DBG_LVL DBG_LOG
#include <rtdbg.h>

extern rt_uint32_t number;
extern rt_uint32_t period,pulse;
extern rt_mutex_t number_protect;

extern struct rt_device_pwm * pwm1 ;
extern struct rt_device_pwm * pwm2 ;

rt_int32_t pwm_l,pwm_r;
rt_int32_t speed;
int middle = 155;
float kp = 650;
float ki = 0.0;
float kd = 1.7;
float dia=0;

rt_thread_t pid_thread = RT_NULL;

void pwm_limit(rt_int32_t * pwm1,rt_int32_t * pwm2)
{
    if(*pwm1>1000000) *pwm1=1000000;
    else if(*pwm1<-1000000) *pwm1=-1000000;

    if(*pwm2>1000000) *pwm2=1000000;
    else if(*pwm2<-1000000) *pwm2=-1000000;
}

void pwm_abs(rt_int32_t pwm_1,rt_int32_t pwm_2)
{
    //rt_kprintf("-%d %d-\r\n",pwm_1,pwm_2);
    if(pwm_1<0)
        {
            rt_pin_write(AIN2_PIN, PIN_HIGH);
            rt_pin_write(AIN1_PIN, PIN_LOW);
            pwm_1 = -pwm_1;
        }
        else if(pwm_1>=0)
        {
            rt_pin_write(AIN1_PIN, PIN_HIGH);
            rt_pin_write(AIN2_PIN, PIN_LOW);
        }
        if(pwm_2<0)
        {
            rt_pin_write(BIN2_PIN, PIN_HIGH);
            rt_pin_write(BIN1_PIN, PIN_LOW);
            pwm_2 = -pwm_2;
        }
        else if(pwm_2>=0)
        {
            rt_pin_write(BIN1_PIN, PIN_HIGH);
            rt_pin_write(BIN2_PIN, PIN_LOW);
        }

        pwm_limit(&pwm_1, &pwm_2);
        static int iii=0;
        iii++;
        if(iii==50)
        {
            rt_kprintf("-%d %d-\r\n",pwm_1,pwm_2);
            iii = 0;
        }
        my_pwm_set_pulse(pwm1, pwm_1);
        my_pwm_set_pulse(pwm2, pwm_2);
}


void pid_compute(int val)
{
    static float error=0,ierror=0,derror=0,errorlast=0;
    error = middle*1.0 -val;
    ierror=ierror+error;
    derror=error-errorlast;
    errorlast=error;
    if(ierror>3000) ierror=3000;
    else if(ierror<-3000) ierror=-3000;
    dia = kp*error/100.0+ki*ierror+kd*derror/10.0;
    pwm_l = speed - dia;
    pwm_r = speed + dia;
    pwm_abs(pwm_l, pwm_r);

}

int pid_set(int argc,char **argv)
{
    if(argc<4)
    {
        rt_kprintf("the format is <kp> <ki> <kd>\r\n");
    }
    else {
        kp = atof(argv[1]);
        ki = atof(argv[2]);
        kd = atof(argv[3]);
        rt_kprintf("SET OK ! KP:%f KI:%f KD:%f",kp,ki,kd);
    }
    return RT_EOK;
}
MSH_CMD_EXPORT(pid_set,pid parameter set);

void pid_thread_entry(void *parameter)
{
    rt_uint32_t num=0;
    while(1)
    {
        speed = period*pulse/100;
        rt_mutex_take(number_protect, RT_WAITING_FOREVER);
        num = number;
        rt_mutex_release(number_protect);
        pid_compute(num);
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






