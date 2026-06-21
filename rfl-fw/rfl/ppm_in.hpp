/*
 * ppm_in.hpp
 *
 *  Created on: Jun 17, 2026
 *      Author: hannes
 */

#ifndef PPM_IN_HPP_
#define PPM_IN_HPP_

struct PPMInput;
PPMInput* ppminput_inst = nullptr;

struct PPMInput{

	PPMInput()
	{
		ppminput_inst = this;
	}

	void start()
	{
		LL_TIM_EnableIT_CC3(TIM1);
		LL_TIM_CC_EnableChannel(TIM1, LL_TIM_CHANNEL_CH3);
		LL_TIM_EnableCounter(TIM1);
	}
	void stop()
	{
		LL_TIM_DisableIT_CC3(TIM1);
		LL_TIM_CC_DisableChannel(TIM1, LL_TIM_CHANNEL_CH3);
		LL_TIM_DisableCounter(TIM1);
	}

	void on_int_cc()
	{
	    if(LL_TIM_IsActiveFlag_CC3(TIM1))
	    {
	        LL_TIM_ClearFlag_CC3(TIM1);

	        uint16_t capture = LL_TIM_IC_GetCaptureCH3(TIM1);

	        // ---- overflow-safe delta ----
	        pulseWidth = (uint16_t)(capture - lastCapture);
	        lastCapture = capture;

	        // ---- frame sync detection ----
	        if(pulseWidth >= PPM_SYNC_MIN_US)
	        {
	            ppmIndex = 0;
	            frameValid = false;
	            lastFrameTime = HAL_GetTick();
	            return;
	        }

	        // ---- noise rejection ----
	        if(pulseWidth < PPM_MIN_US || pulseWidth > PPM_MAX_US)
	        {
	            return;
	        }

	        // ---- channel capture ----
	        if(ppmIndex < PPM_CHANNELS)
	        {
	            uint16_t *ch = (uint16_t*)&channels[ppmIndex];

	            // light smoothing filter (optional but useful)
	            *ch = (*ch == 0)
	                ? pulseWidth
	                : (uint16_t)((*ch * 3 + pulseWidth) / 4);

	            ppmIndex++;
	        }

	        // ---- frame complete ----
	        if(ppmIndex == PPM_CHANNELS)
	        {
	            frameValid = true;
	            lastFrameTime = HAL_GetTick();
	        }
	    }
	}

	// ---- PPM decoder state ----
	volatile uint16_t channels[8] = {0};
	volatile uint8_t  ppmIndex = 0;

	volatile uint16_t lastCapture = 0;
	volatile uint16_t pulseWidth  = 0;

	volatile uint32_t lastFrameTime = 0;
	volatile bool frameValid = false;

	// tuning constants
	static constexpr uint16_t PPM_SYNC_MIN_US = 3000;  // sync gap
	static constexpr uint16_t PPM_MIN_US      = 500;   // reject noise low
	static constexpr uint16_t PPM_MAX_US      = 2500;  // reject noise high
	static constexpr uint8_t  PPM_CHANNELS    = 8;
};

extern "C" void TIM1_CC_IRQHandler(void)
{
	if(ppminput_inst) ppminput_inst->on_int_cc();
	LL_TIM_ClearFlag_CC3(TIM1);
}



#endif /* PPM_IN_HPP_ */
