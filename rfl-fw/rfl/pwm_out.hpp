/*
 * pwm_out.hpp
 *
 *  Created on: Jun 17, 2026
 *      Author: hannes
 */

#ifndef PWM_OUT_HPP_
#define PWM_OUT_HPP_

struct PwmOut{
	PwmOut()
	{

	}
	void start()
	{
		LL_APB2_GRP1_ForceReset(LL_APB2_GRP1_PERIPH_TIM1);
		HAL_Delay(1);
		LL_APB2_GRP1_ReleaseReset(LL_APB2_GRP1_PERIPH_TIM1);
		HAL_Delay(1);

		LL_TIM_InitTypeDef TIM_InitStruct = {0};
		LL_TIM_OC_InitTypeDef TIM_OC_InitStruct = {0};
		LL_TIM_BDTR_InitTypeDef TIM_BDTRInitStruct = {0};

		LL_GPIO_InitTypeDef GPIO_InitStruct = {0};

		LL_RCC_SetTIMClockSource(LL_RCC_TIM1_CLKSOURCE_PCLK1);

		/* Peripheral clock enable */
		LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_TIM1);

		/* TIM1 interrupt Init */
		NVIC_SetPriority(TIM1_CC_IRQn, 0);
		NVIC_EnableIRQ(TIM1_CC_IRQn);

		TIM_InitStruct.Prescaler = 31;
		TIM_InitStruct.CounterMode = LL_TIM_COUNTERMODE_UP;
		TIM_InitStruct.Autoreload = 20000;
		TIM_InitStruct.ClockDivision = LL_TIM_CLOCKDIVISION_DIV1;
		TIM_InitStruct.RepetitionCounter = 0;
		LL_TIM_Init(TIM1, &TIM_InitStruct);
		LL_TIM_DisableARRPreload(TIM1);
		LL_TIM_SetClockSource(TIM1, LL_TIM_CLOCKSOURCE_INTERNAL);
		LL_TIM_OC_EnablePreload(TIM1, LL_TIM_CHANNEL_CH3);
		TIM_OC_InitStruct.OCMode = LL_TIM_OCMODE_PWM1;
		TIM_OC_InitStruct.OCState = LL_TIM_OCSTATE_DISABLE;
		TIM_OC_InitStruct.OCNState = LL_TIM_OCSTATE_DISABLE;
		TIM_OC_InitStruct.CompareValue = 0;
		TIM_OC_InitStruct.OCPolarity = LL_TIM_OCPOLARITY_HIGH;
		TIM_OC_InitStruct.OCNPolarity = LL_TIM_OCPOLARITY_HIGH;
		TIM_OC_InitStruct.OCIdleState = LL_TIM_OCIDLESTATE_LOW;
		TIM_OC_InitStruct.OCNIdleState = LL_TIM_OCIDLESTATE_LOW;
		LL_TIM_OC_Init(TIM1, LL_TIM_CHANNEL_CH3, &TIM_OC_InitStruct);
		LL_TIM_OC_DisableFast(TIM1, LL_TIM_CHANNEL_CH3);
		LL_TIM_SetTriggerOutput(TIM1, LL_TIM_TRGO_RESET);
		LL_TIM_SetTriggerOutput2(TIM1, LL_TIM_TRGO2_RESET);
		LL_TIM_DisableMasterSlaveMode(TIM1);
		TIM_BDTRInitStruct.OSSRState = LL_TIM_OSSR_DISABLE;
		TIM_BDTRInitStruct.OSSIState = LL_TIM_OSSI_DISABLE;
		TIM_BDTRInitStruct.LockLevel = LL_TIM_LOCKLEVEL_OFF;
		TIM_BDTRInitStruct.DeadTime = 0;
		TIM_BDTRInitStruct.BreakState = LL_TIM_BREAK_DISABLE;
		TIM_BDTRInitStruct.BreakPolarity = LL_TIM_BREAK_POLARITY_HIGH;
		TIM_BDTRInitStruct.BreakFilter = LL_TIM_BREAK_FILTER_FDIV1;
		TIM_BDTRInitStruct.BreakAFMode = LL_TIM_BREAK_AFMODE_INPUT;
		TIM_BDTRInitStruct.Break2State = LL_TIM_BREAK2_DISABLE;
		TIM_BDTRInitStruct.Break2Polarity = LL_TIM_BREAK2_POLARITY_HIGH;
		TIM_BDTRInitStruct.Break2Filter = LL_TIM_BREAK2_FILTER_FDIV1;
		TIM_BDTRInitStruct.Break2AFMode = LL_TIM_BREAK_AFMODE_INPUT;
		TIM_BDTRInitStruct.AutomaticOutput = LL_TIM_AUTOMATICOUTPUT_ENABLE;
		LL_TIM_BDTR_Init(TIM1, &TIM_BDTRInitStruct);
		/* USER CODE BEGIN TIM1_Init 2 */

		/* USER CODE END TIM1_Init 2 */
		LL_IOP_GRP1_EnableClock(LL_IOP_GRP1_PERIPH_GPIOB);
		/**TIM1 GPIO Configuration
		PB6   ------> TIM1_CH3
		*/
		GPIO_InitStruct.Pin = d3_Pin;
		GPIO_InitStruct.Mode = LL_GPIO_MODE_ALTERNATE;
		GPIO_InitStruct.Speed = LL_GPIO_SPEED_FREQ_LOW;
		GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
		GPIO_InitStruct.Pull = LL_GPIO_PULL_NO;
		GPIO_InitStruct.Alternate = LL_GPIO_AF_1;
		LL_GPIO_Init(d3_GPIO_Port, &GPIO_InitStruct);

		HAL_Delay(1);


		LL_TIM_CC_EnableChannel(TIM1, LL_TIM_CHANNEL_CH3);
		LL_TIM_CC_EnableChannel(TIM4, LL_TIM_CHANNEL_CH2|LL_TIM_CHANNEL_CH3);
		LL_TIM_CC_EnableChannel(TIM16, LL_TIM_CHANNEL_CH1);
		LL_TIM_EnableCounter(TIM1);
		LL_TIM_EnableCounter(TIM4);
		LL_TIM_EnableCounter(TIM16);
		LL_TIM_OC_SetCompareCH3(TIM1, 1500);
		LL_TIM_OC_SetCompareCH2(TIM4, 1500);
		LL_TIM_OC_SetCompareCH3(TIM4, 1500);
		LL_TIM_OC_SetCompareCH1(TIM16, 1500);
	}

	void set(int c1, int c2, int c3, int c4)
	{
		LL_TIM_OC_SetCompareCH3(TIM1, c3);
		LL_TIM_OC_SetCompareCH2(TIM4, c2);
		LL_TIM_OC_SetCompareCH3(TIM4, c1);
		LL_TIM_OC_SetCompareCH1(TIM16, c4);
	}
};

#endif /* PWM_OUT_HPP_ */
