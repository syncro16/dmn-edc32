/*

*/
#include <math.h>

#include "Dac.h"
#include "Serial.h"
#include "stm32f3xx_hal.h"


Dac dac;

extern DAC_HandleTypeDef hdac1;
extern DAC_HandleTypeDef hdac2;

Dac::Dac() {
	for (int i=0;i<DAC_WAVETABLE_SIZE;i++) {
		baseLut[i] = cos((-5+i)*((3.1415*2)/DAC_WAVETABLE_SIZE)); // -5 adjust table a bit off to make interrupt handler to read exact top position of waveform

	}
}

void Dac::startDmaSineGenerator() {
	for (int i=0;i<DAC_WAVETABLE_SIZE;i++) {
		refWave[i] = DAC_OFS+0x7FF+baseLut[i]*(DAC_MAX_LINEAR_RANGE_R);
/*		if (i<62) {
			refWave[i] = 0xfff;
		} else {
			refWave[i] = 0;
		}	*/	
//		serial.printf("%i ",refWave[i]);
	}
	for (int i=0;i<DAC_WAVETABLE_SIZE;i++) {
		customWave[i] = DAC_OFS+0x7FF+-baseLut[i]*(DAC_MAX_LINEAR_RANGE_C);
	}	

	// check that DMA is configured to HALFWORD and Config.DAC_Trigger = DAC_TRIGGER_T6_TRGO;
	
	HAL_DAC_Start_DMA(&hdac1, DAC_CHANNEL_1, (uint32_t*)refWave, DAC_WAVETABLE_SIZE, DAC_ALIGN_12B_R);
	HAL_DAC_Start_DMA(&hdac1, DAC_CHANNEL_2, (uint32_t*)customWave, DAC_WAVETABLE_SIZE, DAC_ALIGN_12B_R);	
}

void Dac::setCustomWaveAmplitude(float amplitude) {
	dac_buf_updating=1;
	if (amplitude<0) amplitude=0;
	if (amplitude>1) amplitude=1;
	for (int i=0;i<DAC_WAVETABLE_SIZE;i++) {
		// +4 for correct phase shift (or+3)
		customWave[i] = DAC_OFS+0x7FF+-baseLut[(3+i)%DAC_WAVETABLE_SIZE]*((float)DAC_MAX_LINEAR_RANGE_C*amplitude);
	}	
	dac_buf_updating=0;	
	// about 0,08ms
}


void Dac::analogOut(int v) {
	HAL_DAC_SetValue(&hdac2,DAC_CHANNEL_1,DAC_ALIGN_12B_R,v);
	HAL_DAC_Start(&hdac2, DAC_CHANNEL_1); 
}