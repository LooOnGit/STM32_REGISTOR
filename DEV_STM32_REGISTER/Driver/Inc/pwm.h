/*
 * pwm.h
 *
 *  Created on: Aug 14, 2023
 *      Author: maitr
 */

#ifndef INC_PWM_H_
#define INC_PWM_H_

/**
* @brief generate pwm with frequency = 1Hz, pulse width = 50%
* @param None
* @retval None
*/
void pwm_init();
void pwm_pulse_ctrl(uint8_t pulse);


#endif /* INC_PWM_H_ */