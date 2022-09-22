/*
 * Copyright (c) 2006-2021, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2022-09-20     lhylh       the first version
 */
#include "car.h"
#include "pid.h"
#include <rtthread.h>
#include <rtdevice.h>
#include "drivers\include\drv_common.h"

#define DBG_TAG "car"
#define DBG_LVL DBG_LOG
#include <rtdbg.h>

#define CAR_MSH_ENABLE 1

extern int pulse;
int arrive_flag=0;

//设置车的基本速度
int car_set_percent(int argc,char **argv)
{
    if(argc != 2)
    {
        rt_kprintf("please input car_set_percent <percent>\r\n");
        return -RT_ERROR;
    }
    else {
        pulse = atoi(argv[1]);
    }
    return RT_EOK;
}


int car_init(void)
{
    rt_err_t ret = RT_EOK;

    rt_pin_mode(AIN1_PIN, PIN_MODE_OUTPUT);
    rt_pin_mode(AIN2_PIN, PIN_MODE_OUTPUT);
    rt_pin_mode(BIN1_PIN, PIN_MODE_OUTPUT);
    rt_pin_mode(BIN2_PIN, PIN_MODE_OUTPUT);

    rt_pin_write(AIN1_PIN, PIN_LOW);
    rt_pin_write(AIN2_PIN, PIN_LOW);
    rt_pin_write(BIN1_PIN, PIN_LOW);
    rt_pin_write(BIN2_PIN, PIN_LOW);

    ret = pwm_init();
    pid_init();

    return ret;
}

int car_forward(void)
{
    rt_pin_write(AIN1_PIN, PIN_HIGH);
    rt_pin_write(AIN2_PIN, PIN_LOW);
    rt_pin_write(BIN1_PIN, PIN_HIGH);
    rt_pin_write(BIN2_PIN, PIN_LOW);
    my_pwm_enable();
    return RT_EOK;
}
extern int arrive_flag;
int car_stop(void)
{
    LOG_D("now car is stop\r\n");
    rt_kprintf("now car is going to stop\r\n");
    rt_pin_write(AIN1_PIN, PIN_LOW);
    rt_pin_write(AIN2_PIN, PIN_LOW);
    rt_pin_write(BIN1_PIN, PIN_LOW);
    rt_pin_write(BIN2_PIN, PIN_LOW);
    my_pwm_disable();
    rt_kprintf("now car is stop\r\n");
    arrive_flag = 1;
    return RT_EOK;
}

extern struct rt_completion pid_completion;
rt_uint16_t turn_num=600;


int car_turn_ex(int argc,char **argv)
{
    if(argc == 2)
    {
        turn_num = atoi(argv[1]);
    }
    else {
        rt_kprintf("please input car_turn_ex <delaytimes>\r\n");
    }
    return RT_EOK;
}


int car_left(void)
{
    extern struct rt_device_pwm * pwm1;
    extern struct rt_device_pwm * pwm2;
    extern rt_uint32_t period;

    rt_completion_wait((struct rt_completion *)(&pid_completion), RT_WAITING_FOREVER);
    rt_uint32_t level = rt_hw_interrupt_disable();
    rt_pwm_set(pwm1, PWM_CHANNEL1, period, 200000); //left
    rt_pwm_set(pwm2, PWM_CHANNEL2, period, 500000); //right
    for(int i=0;i<3;i++)
    {
        for(int ii=0;ii<1150;ii++)
        {
            for(int iii=0;iii<1000;iii++)
            {

            }
        }
    }

    rt_completion_done((struct rt_completion *)(&pid_completion));
    pid_clear();
    rt_hw_interrupt_enable(level);
    rt_kprintf("turn left ok\r\n");
    return RT_EOK;
}

extern struct rt_completion turn_flag;
int car_right(void)
{

    extern struct rt_device_pwm * pwm1;
    extern struct rt_device_pwm * pwm2;
    extern rt_uint32_t period;

    rt_completion_wait((struct rt_completion *)(&pid_completion), RT_WAITING_FOREVER);
    rt_uint32_t level = rt_hw_interrupt_disable();
    rt_pwm_set(pwm1, PWM_CHANNEL1, period, 500000); //left
    rt_pwm_set(pwm2, PWM_CHANNEL2, period, 200000); //right
    for(int i=0;i<3;i++)
    {
        for(int ii=0;ii<1250;ii++)
        {
            for(int iii=0;iii<1000;iii++)
            {

            }
        }
    }
    rt_completion_done((struct rt_completion *)(&pid_completion));
    pid_clear();

    rt_hw_interrupt_enable(level);
    rt_kprintf("turn right ok\r\n");
    return RT_EOK;
}


int car_turn(void)
{
    extern struct rt_device_pwm * pwm1;
    extern struct rt_device_pwm * pwm2;
    extern rt_uint32_t period;
    my_pwm_enable();
    rt_completion_wait((struct rt_completion *)(&pid_completion), RT_WAITING_FOREVER);
    rt_uint32_t level = rt_hw_interrupt_disable();
    rt_pin_write(AIN1_PIN, PIN_LOW);
    rt_pin_write(AIN2_PIN, PIN_HIGH);
    rt_pin_write(BIN1_PIN, PIN_HIGH);
    rt_pin_write(BIN2_PIN, PIN_LOW);
    rt_pwm_set(pwm1, PWM_CHANNEL1, period, 300000); //left
    rt_pwm_set(pwm2, PWM_CHANNEL2, period, 300000); //right
    for(int i=0;i<4;i++)
    {
        for(int ii=0;ii<1150;ii++)
        {
            for(int iii=0;iii<turn_num;iii++)
            {

            }
        }
    }
    rt_completion_done((struct rt_completion *)(&pid_completion));

    pid_clear();
    rt_pin_write(AIN1_PIN, PIN_HIGH);
    rt_pin_write(AIN2_PIN, PIN_LOW);
    rt_pin_write(BIN1_PIN, PIN_HIGH);
    rt_pin_write(BIN2_PIN, PIN_LOW);
    rt_hw_interrupt_enable(level);
    rt_kprintf("turn ok\r\n");
    return RT_EOK;
}

#if CAR_MSH_ENABLE

MSH_CMD_EXPORT(car_set_percent , car set percent);
MSH_CMD_EXPORT(car_stop,car stop);
MSH_CMD_EXPORT(car_forward,car forward);
MSH_CMD_EXPORT(car_left, car left);
MSH_CMD_EXPORT(car_right, car right);
MSH_CMD_EXPORT(car_turn, car turn);
MSH_CMD_EXPORT(car_turn_ex,set turn delay times);

#endif
