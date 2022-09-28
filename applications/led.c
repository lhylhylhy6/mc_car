/*
 * Copyright (c) 2006-2021, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2022-09-28     lhylh       the first version
 */
#include <rtthread.h>
#include <rtdevice.h>
#include "board.h"
#include "led.h"

//#define DBG_TAG "led"
//#define DBG_LVL DBG_LOG
//#include <rtdbg.h>


void led_init(void)
{
    rt_pin_mode(LED1, PIN_MODE_OUTPUT);
    rt_pin_mode(LED2, PIN_MODE_OUTPUT);
    rt_pin_mode(LED3, PIN_MODE_OUTPUT);
    LED_DOWN(1);
    LED_DOWN(2);
    LED_DOWN(3);
}




