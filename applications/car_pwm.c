/*
 * Copyright (c) 2006-2021, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2022-09-20     10091       the first version
 */
#include "car_pwm.h"

#define DBG_TAG "pwm"
#define DBG_LVL DBG_LOG
#include <rtdbg.h>

struct rt_device_pwm * pwm1 = RT_NULL;
struct rt_device_pwm * pwm2 = RT_NULL;

rt_uint32_t period = 1000000 ;
float pulse_percent = 50;

int pwm_init(void)
{
    rt_err_t ret;
    pwm1 = (struct rt_device_pwm *)rt_device_find(PWM_DEVICE_NAME1);
    pwm2 = (struct rt_device_pwm *)rt_device_find(PWM_DEVICE_NAME2);
    if(pwm1)
    {
        ret = rt_pwm_set(pwm1, PWM_CHANNEL1, period, (rt_uint32_t)((period*pulse_percent)/100));
        if(ret<0)
            LOG_D("ret1: %d\r\n",ret);
        rt_pwm_enable(pwm1, PWM_CHANNEL1);
    }
    ret = RT_EOK;
    if(pwm2)
    {
        ret = rt_pwm_set(pwm2, PWM_CHANNEL2, period, (rt_uint32_t)((period*pulse_percent)/100));
        if(ret<0)
            LOG_D("ret2: %d\r\n",ret);
        rt_pwm_enable(pwm2, PWM_CHANNEL2);
    }
    ret = rt_pwm_disable(pwm1, PWM_CHANNEL1);
    ret = rt_pwm_disable(pwm2, PWM_CHANNEL2);
    return ret;
}

int my_pwm_set(int argc,char **argv)
{
      rt_uint32_t new_pulse = 0;
      rt_err_t err = RT_EOK;
      if(argc == 1)
      {
        LOG_D("please input :my_pwm_set_pulse <pwm_dev> <percent>\r\n");
        err = - RT_ERROR;
        return err;
      }
      else if(argc == 2)
      {
        LOG_D("please input :my_pwm_set_pulse <pwm_dev> <percent>\r\n");
        err = - RT_ERROR;
        return err;
      }
      else
      {
          rt_uint8_t percent = 0;
          percent = atoi(argv[2]);
          if(strcmp(argv[1],"pwm1")==0)
          {
              new_pulse = period * percent / 100;
              rt_pwm_set(pwm1, PWM_CHANNEL1, period, new_pulse);
              LOG_D("set %s %d%% ok\r\n",pwm1,percent);
          }
          else if(strcmp(argv[1],"pwm2")==0)
          {
              new_pulse = period * percent / 100;
              rt_pwm_set(pwm2, PWM_CHANNEL2, period, new_pulse);
              LOG_D("set %s %d%% ok\r\n",pwm2,percent);
          }
          else {
              LOG_D("please input correct device name\r\n");
              LOG_D("pwm1 or pwm2\r\n");
              err = - RT_ERROR;
              return err;
          }
      }
      return err;
}
MSH_CMD_EXPORT(my_pwm_set , <input percent(%)>to set the pwm pulse);

rt_err_t my_pwm_set_pulse(struct rt_device_pwm * pwm_dev,rt_uint32_t pulse_val)
{
    rt_err_t ret=RT_EOK;

    RT_ASSERT(pwm_dev);
    if(pwm_dev == pwm1)
        ret = rt_pwm_set(pwm_dev, PWM_CHANNEL1, period, pulse_val);
    if(pwm_dev == pwm2)
        ret = rt_pwm_set(pwm_dev, PWM_CHANNEL2, period, pulse_val);
    return ret;
}

rt_err_t my_pwm_enable(void)
{
    rt_err_t ret = RT_EOK;
    ret = rt_pwm_enable(pwm1, PWM_CHANNEL1);
    ret = rt_pwm_enable(pwm2, PWM_CHANNEL2);
    return ret;
}

rt_err_t my_pwm_disable(void)
{
    rt_err_t ret = RT_EOK;
    ret = rt_pwm_disable(pwm1, PWM_CHANNEL1);
    ret = rt_pwm_disable(pwm2, PWM_CHANNEL2);
    return ret;
}





