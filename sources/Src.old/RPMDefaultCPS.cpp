#include "RPMDefaultCPS.h"
#include "RPMDefaultCPSInterrupt.h"
#include "stm32f3xx_hal.h"

/*
- TIM15 base frequency is set to 4Hz. (presclaler = 244)
- Timer is 16bit, so there are 4*65536 steps in a second.
*/



void RPMDefaultCps::init() {
	rpm_duration=0;
	rpm_overflow=1;	
}

unsigned int RPMDefaultCps::getLatestMeasure() {
	if (rpm_overflow)
		return 0;
	// 64 = timer divider

	// return ((unsigned long)(60*F_CPU/64/NUMBER_OF_CYLINDERS)/((unsigned long)rpmDuration)); 
	return (60*4*65536)/rpm_duration/NUMBER_OF_CYLINDERS;

}
unsigned int RPMDefaultCps::getLatestRawValue() {
	return rpm_duration;
}

int RPMDefaultCps::getInjectionTiming() {
	if (HAL_GetTick() - trigger_last_hit < 500)
		return rpm_injection_trigger;
	return 0;
}

