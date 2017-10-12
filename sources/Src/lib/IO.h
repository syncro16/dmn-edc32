#include <stdint.h>
#include "main.h"
#include "stm32f3xx_hal.h"

extern TIM_HandleTypeDef htim1;
extern TIM_HandleTypeDef htim2;
extern TIM_HandleTypeDef htim3;
extern TIM_HandleTypeDef htim15;
extern TIM_HandleTypeDef htim17;

class IO {
public:
	IO();
	void outputAux(bool state);
	void outputFuelSolenoid(bool state);
	void outputGlowPlugs(bool state);
	void outputFanIntake(bool state);
	void outputMainPowerRelay(bool state);
	void pulseOutEngineSpeedSignal(int rpm);
	void pwmOutQuantityAdjuster_12b(uint16_t a);
	void pwmOutTiming_8b(uint8_t a);
	void pwmOutBoostSolenoid_8b(uint8_t a);
	void pwmOutAux_8(uint8_t a);
	
	bool inputTPSWot();
	bool inputTPSIdle();
	bool inputBrake();
	bool inputCruiseSet();
	bool inputCruiseDecel();
	bool inputAux();
	uint16_t analogInBatteryVoltage();
	uint16_t analogInManifoldPressre();
	uint16_t analogInTPS();
	uint16_t analogInFuelTemp();
	uint16_t analogInCoolantTemp();
	uint16_t analogInAuxTemp();
	uint16_t analogInAuxIn1();
	// QA handled by own routine
	// DAC out handled by own routine	
	
	
	
};
extern IO io;	