/*
 * Copyright (c) 2006-2021, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2022-09-28     lhylh       the first version
 */
#ifndef APPLICATIONS_LED_H_
#define APPLICATIONS_LED_H_

#define LED1 GET_PIN(F,1) //red
#define LED2 GET_PIN(F,2) //yellow
#define LED3 GET_PIN(F,3) //green

#define LED_LIGHT(x) rt_pin_write(LED##x,PIN_HIGH)
#define LED_DOWN(x)  rt_pin_write(LED##x,PIN_LOW)

void led_init(void);

#endif /* APPLICATIONS_LED_H_ */
