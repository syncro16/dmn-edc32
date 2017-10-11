#include <stdint.h>

class IO {
public:
	void outputAux(bool state);
	void outputFuelSolenoid(bool state);
	void outputGlowPlugs(bool state);
	void outputFanIntake(bool state);
	void outputMainPowerRelay(bool state);
	void pulseOutEngineSpeedSignal(int rpm);
	void pwmOutQuantityAdjuster_12b(uint_16 a);
	void pwmOutTiming_8b(uint_8 a);
	void pwmOutBoostSolenoid_8b(uint_8 a);
	void pwmOutAux_8(uint_8 a);
	
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
	
	
	
}
extern IO io;	