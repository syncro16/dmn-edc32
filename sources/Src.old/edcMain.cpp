#include <stdarg.h>
#include <stdlib.h>

#include "RPMBase.h"
#include "edcMain.h"
#include "Dac.h"
#include "ConfEditor.h"
#include "IO.h"
#include "Serial.h"
#include "main.h"
#include "RPMDefaultCPSInterrupt.h"

#include "stm32f3xx_hal.h"
#include "string.h"
#include "utils.h"
#include "defines.h"
#include "DTC.h"
#include "PID.h"
#include "RPMBase.h"
#include "RPMDefaultCPS.h"
#include "QuantityAdjuster.h"
#include "stm32f3xx_hal.h"
#include "stm32f3xx_hal_tim.h"



/* phew! finally the main program rather than HAL crap */

// RPM input module
static RPMDefaultCps rpm;

// VP37 Quantity adjuster module
static QuantityAdjuster adjuster;

extern ADC_HandleTypeDef hadc1;
extern ADC_HandleTypeDef hadc2;

extern DMA_HandleTypeDef hdma_adc1;
extern volatile int QApos1;
extern volatile int QApos2;
extern volatile int QAref;
extern volatile int QAcustom;
extern volatile int freq;
extern volatile uint16_t adc1buf[16];
extern volatile uint16_t adc2buf[1];

extern ADC_HandleTypeDef hadc1;
extern __IO uint32_t uwTick;

//uint16_t buf[1024];

void setup() {
	serial.enableDmaRx(); // DMA Receiver routine	
	serial.printf("     BOOT\r\n");
	serial.ansiReset();
	serial.ansiClearScreen();
	core.load();
	serial.printf("Press any key ...");
	serial.ansiHideCursor(true);
	
	dac.startDmaSineGenerator(); 
	float a=0.58;

	dac.setCustomWaveAmplitude(a);	
	dac.analogOut(2200);	
}

void handleUserInterface() {
	static char lastKey;		
	if (serial.available()) {
		char c = serial.read();
		if (c == 27) {
			c = serial.read();
			if (c == '[') {
				c = serial.read();
				// Cursor to "vi" nodes
				switch (c) { 
						case 'A':
						lastKey = KEY_UP;
						break;
						case 'B':
						lastKey = KEY_DOWN;
						break;
						case 'C':
						lastKey = KEY_RIGHT;
						break;
						case 'D':
						lastKey = KEY_LEFT;
						break;
				}
			}
			confeditor.handleInput(lastKey);
		} else {
			// Special commands
			switch (c) {
			default:
				confeditor.handleInput(c);
			}				
		}					
	}
	confeditor.refresh();	
}

void refreshFastSensors() {
	int fuelAmount;
	int rpmIdle;
	static unsigned int runCount;
	static bool safetyStop = false;
	static int idleLastCalculatedFuelAmount;
	
	/* call rpm measurement function which converts internal timer state to rotation per minute (and stores it to control structure) */
	rpm.measure();
	core.controls[Core::valueEngineRPMFiltered] = 1250;
	
	if (core.controls[Core::valueEngineRPMFiltered] == 0 && core.node[Core::nodeFuelCutAtStall].value == 1) {
		if (runCount > FAST_START_DELAY)
		safetyStop = true;
		if (runCount<255)
		runCount++;
		core.controls[Core::valueRunMode]=0; // Stopped  		
	}

	if (safetyStop) {
		// When engine is not running, do not inject anything to prevent runaway situation when RPM signal is missing and engine is running
		fuelAmount = 0;
		core.controls[Core::valueRunMode]=0; // Stopped  
	} else { 
		safetyStop = false;
		unsigned int rpm;

		rpm = core.controls[Core::valueEngineRPMFiltered];
		int rpmCorrected = mapValues(rpm,0,core.node[Core::nodeControlMapScaleRPM].value);   

		core.controls[Core::valueRPM8bit] = rpmCorrected;
/*
		if (rpm<350) {
			core.controls[Core::valueRunMode] = 1; // starting 
		} else {
			if (core.controls[Core::valueRunMode]<98)
				core.controls[Core::valueRunMode]++;
				}*/
/*
				if (core.controls[Core::valueRunMode] == 0) {
			// Saturate PID control before start
			// core.controls[Core::valueIdlePIDCorrection] = core.node[Core::nodeIdleMinFuel].value*2;
			}
			*/

		// Use or switch to PID idle control if gas is not pressed and RPM below threshold. When PID is activated, it is switched of only when gas is pressed (no RPM threshold check)
		if (/*(core.controls[Core::valueRunMode] == 2 ||*/  
				/* rpm<(core.node[Core::nodeIdleSpeedTarget].value+400)) && */
			core.controls[Core::valueTPSActual] == 0) {

			rpmIdle = mapValues(rpm,0,1024); //TODO 

			if (core.node[Core::nodeIdleAdjusting].value && rpm>270) {
				// TODO fix valueRunMode!!
				fuelAmount = mapLookUp(core.maps[Core::mapIdxIdleMap],
					rpmIdle,
					core.controls[Core::valueTPSActual]);	
	//			idleMinFuel = core.node[Core::nodeIdleMinFuel].value; // JOOSE
	//			idleMaxFuel = core.node[Core::nodeIdleMaxFuel].value;
				core.controls[Core::valueRunMode] = 2; // pid idle 
	//			idlePidControl.setPosition(core.node[Core::nodeIdleSpeedTarget].value);
	//			idlePidControl.calculate();
				fuelAmount += core.controls[Core::valueIdlePIDCorrection];
				idleLastCalculatedFuelAmount = fuelAmount;
			} else {
				// Use idle & cold start map.
				fuelAmount = mapLookUp(core.maps[Core::mapIdxIdleMap],
							rpmIdle,
							core.controls[Core::valueTPSActual]);
				
				idleLastCalculatedFuelAmount = fuelAmount;
				core.controls[Core::valueRunMode] = 1; // starting 

				// Sets up the integral part of PID calculation
				// idlePidControl.setPositionHint(fuelAmount);
			}					 

		} else {
			// Return upscaled interpolated value of range 0..1023 according to fuel map data (0..255)
			core.controls[Core::valueRunMode]=100; // Engine running 

			// Base injection quantity 
			// TODO::: generate fuelMapLookup which uses upscaled fuel trim values before final interpolation

			core.controls[Core::valueFuelBaseAmount] = mapLookUp(core.maps[Core::mapIdxFuelMap],
				rpmCorrected,
				core.controls[Core::valueTPSActual]);

			/* Limit enrichment amount to by REQUESTED boost level (turbo control table), not the actual */
			unsigned char boostRequlated = core.controls[Core::valueBoostPressure];

			if (boostRequlated > core.controls[Core::valueBoostTarget])
				boostRequlated = core.controls[Core::valueBoostTarget];

			// Enrichment based on boost, amount is TPS% * fuel enrichment map value to smooth apply of enrichment
			core.controls[Core::valueFuelEnrichmentAmount] = 
			((unsigned long)(mapLookUp(core.maps[Core::mapIdxBoostMap],
					rpmCorrected,
					boostRequlated)) 
				*(unsigned  long)(core.controls[Core::valueTPSActual])
				/ (unsigned long)256);

			// Smoke limit map (TODO: add wide band lambda support and auto-map feature)
			//core.controls[Core::valueFuelLimitAmount] = mapLookUp10bit(core.maps[Core::mapIdxMaxFuelMap],
			//		rpmCorrected,
			//		core.controls[Core::valueBoostPressure]);	

			fuelAmount = core.controls[Core::valueFuelBaseAmount];
		
			// Smooth transtion from idle
			if (idleLastCalculatedFuelAmount>fuelAmount) 
				fuelAmount = idleLastCalculatedFuelAmount;

			if (fuelAmount) {
				// Enrichment amount is TPS% * fuel enrichment map value to smooth apply of enrichment
				fuelAmount += core.controls[Core::valueFuelEnrichmentAmount];
			}

			if (fuelAmount>core.controls[Core::valueFuelLimitAmount])	
				fuelAmount = core.controls[Core::valueFuelLimitAmount];		

		}
	}
	core.controls[Core::valueFuelAmount] = fuelAmount;	
	core.controls[Core::valueFuelAmount8bit] = fuelAmount/4;
	adjuster.setPosition(fuelAmount);
}


/* Read sensor values here which are not tied into interrupt handlers (see refreshFastSensors) 
   If changing over 8bit values which are used by interrupt controller (refreshFastSensor), remember to disable intterrupts while assigning parameter
*/

void refreshSlowSensors() {
	// read slow changing sensors
	int value;
	int scaledValue;

 	// Engine TEMP
	value = 42; //analogRead(PIN_ANALOG_TEMP_COOLANT);
	scaledValue = mapLookUp(core.maps[Core::mapIdxEngineTempSensorMap], value / 4, 0);		
	if (value > ANALOG_INPUT_HIGH_STATE_LIMIT) {
		// generate error only if map is configured and sensor reading is not present
		if (scaledValue > 0)
		dtc.setError(DTC_ENGINE_TEMP_UNCONNECTED);
		scaledValue = core.node[Core::nodeTempEngine].value; // use configuration setpoint value sensor's failback substitute value
	} 
	core.controls[Core::valueTempEngine]=scaledValue;
	
	// Fuel TEMP
	value = 42; //analogRead(PIN_ANALOG_TEMP_FUEL);
	scaledValue = mapLookUp(core.maps[Core::mapIdxFuelTempSensorMap], value / 4, 0);		
	if (value > ANALOG_INPUT_HIGH_STATE_LIMIT) {
		// generate error only if map is configured and sensor reading is not present
		if (scaledValue > 0)
		dtc.setError(DTC_FUEL_TEMP_UNCONNECTED);
		scaledValue = core.node[Core::nodeTempFuel].value; // use configuration setpoint value sensor's failback substitute value
	}
	core.controls[Core::valueTempFuel]=scaledValue;

	// Air TEMP
	value = 42; //analogRead(PIN_ANALOG_TEMP_INTAKE);
	scaledValue = mapLookUp(core.maps[Core::mapIdxAirTempSensorMap], value / 4, 0);		
	if (value > ANALOG_INPUT_HIGH_STATE_LIMIT) {
		// generate error only if map is configured and sensor reading is not present
		if (scaledValue > 0)
		dtc.setError(DTC_AIR_TEMP_UNCONNECTED);
		scaledValue = core.node[Core::nodeTempAir].value; // use configuration setpoint value sensor's failback substitute value
	}
	core.controls[Core::valueTempAir]=scaledValue;

	// Gearbox TEMP
	value = 42; // analogRead(PIN_ANALOG_TEMP_GEARBOX);
/*	scaledValue = mapLookUp(core.maps[Core::mapIdxAirTempSensorMap], value / 4, 0);		
	if (value > ANALOG_INPUT_HIGH_STATE_LIMIT) {
		// generate error only if map is configured and sensor reading is not present
		if (scaledValue > 0)
		dtc.setError(DTC_AIR_TEMP_UNCONNECTED);
		scaledValue = core.node[Core::nodeTempAir].value; // use configuration setpoint value sensor's failback substitute value
	}
	core.controls[Core::valueTempAir]=scaledValue;*/

	// Throttle position sensor(s) - Common VW sensor type
	// swiches for idle and full gas (later one not used)
	value = 842; // analogRead(PIN_ANALOG_TPS_POS);
	core.controls[Core::valueTPSRaw]=value;

	// Check TPS it is connected, otherwise apply limp mode amount (about 15%) 
	if (value > ANALOG_INPUT_HIGH_STATE_LIMIT) {
		dtc.setError(DTC_TPS_UNCONNECTED);
		core.controls[Core::valueTPSActual] = TPS_LIMP_MODE_AMOUNT; 
	} else {
		scaledValue = mapValues(core.controls[Core::valueTPSRaw],
				core.node[Core::nodeTPSMin].value,
				core.node[Core::nodeTPSMax].value);
		core.controls[Core::valueTPSActual] = scaledValue; 

	}

	/* override for gas pedal idle position TODO!*/
//	if ((core.node[Core::nodeTPSSafetyBits].value & TPS_SAFETY_BITS_IDLESW) == TPS_SAFETY_BITS_IDLESW) {
//		value = digitalRead(PIN_TPS_IDLE);
//		if (!value) {
//			core.controls[Core::valueTPSActual] = 0;
////		}
//	}

	value = 42; //analogRead(PIN_ANALOG_MAP);
	core.controls[Core::valueMAPRaw] = value; 
	static int mapFailCount = 0;
	if (value > ANALOG_INPUT_HIGH_STATE_LIMIT) {
		if (mapFailCount > SENSOR_FAIL_COUNT) {
			dtc.setError(DTC_MAP_UNCONNECTED);
		 	// Map failback is zero kPa
		 	core.controls[Core::valueBoostPressure] = 0; 
	 	} else {
	 		mapFailCount++;	
	 	}
	} else {
	 	core.controls[Core::valueBoostPressure] = mapValues(core.controls[Core::valueMAPRaw],
	 		core.node[Core::nodeMAPMin].value,
	 		core.node[Core::nodeMAPMax].value);
	 	mapFailCount = 0;
	}

 	core.controls[Core::valueBatteryVoltage] = 42; //analogRead(PIN_ANALOG_BATTERY_VOLTAGE);  

	// Log adjuster accuracy for debugging
	core.controls[Core::valueQAJitter] = adjuster.accuracy; 
	core.node[Core::nodeQADebugJitter].value = adjuster.setPointMax - adjuster.setPointMin;
	adjuster.setPointMax=0;
	adjuster.setPointMin=2000;
	
	// Register current injection timing (*10 fixed point)
	core.controls[Core::valueEngineTimingActual] = rpm.getInjectionTiming();
}

/*
	Calculates injection pump advance, controls N108 duty cycle which affects pump internal pressure and thus advance piston position
	- Operating modes 
		0 = off
		1 = open loop control, duty cycle is read from table (injFuel x RPM)
		2 = closed loop control, PID control from target table

*/
void doTimingControl() {
	if (core.controls[Core::valueOutputTestMode] != 0 || core.node[Core::nodeTimingMethod].value == 0) {	
		//analogWrite(PIN_PWM_TIMING_SOLENOID,core.controls[Core::valueEngineTimingDutyCycle]);	

		return;
	}

	if (core.node[Core::nodeTimingMethod].value == 2) {
		/* closed loop mode */


		core.controls[Core::valueEngineTimingTarget] = mapLookUp(core.maps[Core::mapIdxClosedLoopAdvanceMap],
			core.controls[Core::valueRPM8bit],
			core.controls[Core::valueFuelAmount8bit]);	

		static int timingKd = 0, timingSpeed=16, timingMin=N108_MIN_DUTY_CYCLE, timingMax=N108_MAX_DUTY_CYCLE;
		static PID timingPidControl(
			(int*)&core.node[Core::nodeTimingKp].value,
			(int*)&core.node[Core::nodeTimingKi].value,
			&timingKd,
			&timingMin,
			&timingMax,
			&timingSpeed,
			NULL,
			(int*)&core.controls[Core::valueEngineTimingActual], // Current timing
			(int*)&core.controls[Core::valueTimingPIDAmount] // output DC
			);

		timingPidControl.setPosition(core.controls[Core::valueEngineTimingTarget]);
		timingPidControl.calculate();

		core.controls[Core::valueEngineTimingDutyCycle] = core.controls[Core::valueTimingPIDAmount];

		// for ui
		core.node[Core::nodeEngineTiming].value = core.controls[Core::valueEngineTimingTarget];
	} else {
		/* open loop mode */
		core.controls[Core::valueEngineTimingDutyCycle] = mapLookUp(core.maps[Core::mapIdxOpenLoopAdvanceMap],
				core.controls[Core::valueRPM8bit],
				core.controls[Core::valueFuelAmount8bit]);			
	}

	//analogWrite(PIN_PWM_TIMING_SOLENOID,core.controls[Core::valueEngineTimingDutyCycle]);	
}

/*
	Calculates boost control parameters, controls N75 duty cycle which actuates Turbo VNT vanes
	- Operating modes 
		0 = off
		1 = open loop control, N75 duty cycle is read from table (injFuel x RPM)
		2 = closed loop control, N75 duty cycle is read from table + small amount of PID is applied
		*/

void doBoostControl() {
/*	static SimpleRotatingActuator SRA((int*)&core.node[Core::nodeSRAMinPos].value,
		(int*)&core.node[Core::nodeSRAMaxPos].value,
		(int*)&core.node[Core::nodeSRAInverseOperation].value);
*/
	if (core.controls[Core::valueOutputTestMode] != 0 || core.node[Core::nodeBoostAdjusting].value == 0) {	
	//		SRA.setPosition(core.controls[Core::valueN75DutyCycle]);
	//		SRA.calculate();
		//analogWrite(PIN_PWM_BOOST_SOLENOID,core.controls[Core::valueN75DutyCycle]);
		return;
	}
	static int boostMin=0, boostMax=255;
	static PID boostPidControl(
		(int*)&core.node[Core::nodeBoostKp].value,
		(int*)&core.node[Core::nodeBoostKi].value,
		(int*)&core.node[Core::nodeBoostKd].value,
		&boostMin,
		&boostMax,
		(int*)&core.node[Core::nodeBoostSpeed].value,
		(int*)&core.node[Core::nodeBoostBias].value,						
		(int*)&core.controls[Core::valueBoostPressure], // input
		(int*)&core.controls[Core::valueBoostPIDCorrection] // output
		);


	/* Open loop control */


	/* Look up for requested boost level (RPM x TPS) */
	core.controls[Core::valueBoostTarget] = mapLookUp(core.maps[Core::mapIdxTurboTargetPressureMap],
		core.controls[Core::valueRPM8bit],
		core.controls[Core::valueTPSActual]);	

	/* TODO: add check for overboost situations and set up DTC and safe mode */

	core.controls[Core::valueN75DutyCycleBaseForPid] =  mapLookUp(core.maps[Core::mapIdxTurboControlMap],
		core.controls[Core::valueRPM8bit],
		core.controls[Core::valueFuelAmount8bit]);	


	// for ui
	core.node[Core::nodePressure].value = core.controls[Core::valueBoostTarget];

	long int idx;
	unsigned char out;
	core.controls[Core::valueBoostActuatorClipReason] = BOOST_OK;

	static char boostRunCount;
	int joo;
	
	boostRunCount++;

	switch (core.node[Core::nodeBoostAdjusting].value) {
		case 2:
			/* Closed loop control 
			- Duty cycle table predefines max opening of vanes
			- Small amount of pid correction is subtracted of max opening position
			*/

			// Saturate PID control min/max according to fixed duty cycle table	
			// boostMin = 0;//-core.node[Core::nodeBoostPIDRange].value;
			// Fixed DC is the maximum opening
			// boostMax = core.node[Core::nodeBoostPIDRange].value;

			if (boostRunCount % 4 == 0) {
				boostMin = -core.node[Core::nodeBoostPIDRange].value; 
				boostMax = core.node[Core::nodeBoostPIDRange].value; 

				boostPidControl.setPosition(core.controls[Core::valueBoostTarget]);
				boostPidControl.calculate();
				core.controls[Core::valueBoostPIDCorrection] = -core.controls[Core::valueBoostPIDCorrection];
			}

			idx = core.controls[Core::valueN75DutyCycleBaseForPid] + core.controls[Core::valueBoostPIDCorrection];
			if (idx<0) {
				idx = 0;
			}
			if (idx>255) {
				idx = 255;
			}
			out = mapLookUp(core.maps[Core::mapIdxActuatorTension],idx,0); 

			core.controls[Core::valueN75DutyCycle] = out;
			break;
		case 3:			
			// Fixed openin based on control table
			idx = (((long int)core.controls[Core::valueBoostPressure]*100/(long int)core.controls[Core::valueBoostTarget])*256)/100;

			if (idx<0) {
				idx = 0;
			}
			if (idx>255) {
				idx = 255;
			}
			idx = idx;
	   	
		   	core.controls[Core::valueN75DutyCycle] =mapLookUp(core.maps[Core::mapIdxActuatorTension],idx,0); 
		   	break;
		  	case 4:
			// Classic vnt lda style + small PID correction
			idx = idx;

		    idx = (((long int)core.controls[Core::valueBoostPressure]*100/(long int)core.controls[Core::valueBoostTarget])*256)/100;

		    if (boostRunCount % 4 == 0) {
		    	boostMin = -core.node[Core::nodeBoostPIDRange].value; 
		    	boostMax = core.node[Core::nodeBoostPIDRange].value; 

		    	boostPidControl.setPosition(core.controls[Core::valueBoostTarget]);
		    	boostPidControl.calculate();
		    	core.controls[Core::valueBoostPIDCorrection] = -core.controls[Core::valueBoostPIDCorrection];
		    }

		    if (idx<0) {
		    	idx = 0;
		    }
		    if (idx>255) {
		    	idx = 255;
		    }

		    out = 255-mapLookUp(core.maps[Core::mapIdxActuatorTension],idx,0); 

/*
			out = 255-mapLookUp(core.maps[Core::mapIdxActuatorTension],idx,0); 

		   	// scale value
		   	joo = map(out,0,255,0,core.controls[Core::valueN75DutyCycleBaseForPid]);   	
		   	*/
		   	// scale value
		   	joo = map(out,0,255,0,core.controls[Core::valueN75DutyCycleBaseForPid]+core.controls[Core::valueBoostPIDCorrection]);   		   	
		   	core.controls[Core::valueN75DutyCycle] = joo;
		   	break;
		   }

		static unsigned int idleRunCycleCount = 0;


		/* Open vanes when idling */
		if (core.controls[Core::valueTPSActual] == 0 && core.controls[Core::valueEngineRPMFiltered]<1200) {
		   	idleRunCycleCount++;
		   	/* quick test: VNT autoclean */ 
		   	if (VNT_OPEN_DELAY_ON_IDLE != 0 && idleRunCycleCount > VNT_OPEN_DELAY_ON_IDLE ) {
		   		core.controls[Core::valueN75DutyCycle] = 255;
		   	}
		   	if (VNT_OPEN_DELAY_ON_IDLE != 0 && idleRunCycleCount > VNT_OPEN_DELAY_ON_IDLE*10 ) {
		   		core.controls[Core::valueN75DutyCycle] = 0;
		   	}
		   	if (VNT_OPEN_DELAY_ON_IDLE != 0 && idleRunCycleCount > VNT_OPEN_DELAY_ON_IDLE*11 ) {
		   		idleRunCycleCount = 0;
		   	}

		   } 
		   if (core.controls[Core::valueEngineRPMFiltered]>1200 || core.controls[Core::valueTPSActual] > 0) {
		   	idleRunCycleCount = 0;
		   }

/*
	SRA.calculate();	
	SRA.setPosition(core.controls[Core::valueN75DutyCycle]);
	*/

	//analogWrite(PIN_PWM_BOOST_SOLENOID,core.controls[Core::valueN75DutyCycle]);

}

static int rpmIdle;
volatile static char calls=0;

// Last idle calculated loop fuel amount. This decreases slowly on non-idle run mode, providing smooth transition between maps
// baseFuelAmount is set to idleLastCalculatedFuelAmount if it is greater than value read from control lookup table (map)

static int idleLastCalculatedFuelAmount; 
void doIdlePidControl() {
	/* PID */ 
	if (idleLastCalculatedFuelAmount>0)
		idleLastCalculatedFuelAmount--;

	static int idleMinFuel,idleMaxFuel,pidP;

	/* Use Idle PID p-parameter lookup table if static value of P is below 2*/
	if (core.node[Core::nodeIdleKp].value<2) {
		pidP = core.node[Core::nodeIdleKp].value;
	} else {
		pidP = mapLookUp(core.maps[Core::mapIdxIdlePidP],rpmIdle,0);	
	}

	static PID idlePidControl(
		(int*)&pidP,
//		(int*)&core.node[Core::nodeIdleKp].value,
		(int*)&core.node[Core::nodeIdleKi].value,
		(int*)&core.node[Core::nodeIdleKd].value,
			(int*)&idleMinFuel, //(int*)&core.node[Core::nodeIdleMinFuel].value,
			(int*)&idleMaxFuel, // (int*)&core.node[Core::nodeIdleMaxFuel].value,
			(int*)&core.node[Core::nodeIdlePIDSpeed].value,
			(int*)&core.node[Core::nodeIdlePIDBias].value,			
			(int*)&core.controls[Core::valueEngineRPMFiltered],
			(int*)&core.controls[Core::valueIdlePIDCorrection]
			);


	idleMinFuel = core.node[Core::nodeIdleMinFuel].value; // JOOSE
	idleMaxFuel = core.node[Core::nodeIdleMaxFuel].value;

	idlePidControl.setPosition(core.node[Core::nodeIdleSpeedTarget].value);
	idlePidControl.calculate(); // interrupt safe!
}


void doRelayControl() {
	
}
	
void __probe() {
	static char loop;
/*	if (!loop) 
		serial.ansiClearScreen();
	serial.ansiGotoXy(1,1);*/
	loop++;
	bool out = (loop/64)%2;

//	serial.printf("RPM:%4d    Timing:%5d  \r\n",rpm.getLatestMeasure(),rpm.getInjectionTiming());

/*	serial.printf("RPM:%4d    Timing:%5d  \r\n",rpm.getLatestMeasure(),rpm.getInjectionTiming());
	serial.printf("OUT: aux:%d  fuel:%d  glow:%d  fan:%d   relay:%d\r\n",out,out,out,out,out);
*/
	io.outputAux(out);
	io.outputFuelSolenoid(out);
	io.outputGlowPlugs(out);
	io.outputFanIntake(out);
	io.outputMainPowerRelay(out);
	
	if (out) {
//		serial.printf("PWM: QA:50%% tim:50%% bst:50%% aux:50%% speed_pulse:0\r\n");	
		io.pwmOutQuantityAdjuster_12b(2048);
		io.pwmOutBoostSolenoid_8b(128);
		io.pwmOutTiming_8b(128);
		io.pwmOutAux_8(128);
		io.pulseOutEngineSpeedSignal(0);		
	} else {
//		serial.printf("PWM: QA:0%%  tim:0%%  bst:0%%  aux:0%%  speed_pulse:1000rpm\r\n");			
		io.pwmOutQuantityAdjuster_12b(0);
		io.pwmOutBoostSolenoid_8b(0);
		io.pwmOutTiming_8b(0);
		io.pwmOutAux_8(0);
		io.pulseOutEngineSpeedSignal(1000);
	}
/*	serial.printf("IN:  aux:%d  wot:%d   idle:%d  brake:%d cSet:%d   cDecel:%d          \r\n",
		io.inputAux(),io.inputTPSWot(),io.inputTPSIdle(),io.inputBrake(),io.inputCruiseSet(),io.inputCruiseDecel());
	
	serial.printf("ADC: batv:%4d press:%4d tps:%4d   ftmp:%4d  ctmp:%4d   atmp:%4d  aux:%4d \r\n     itmp:%4d qaPos:%4d                      \r\n",
		io.analogInBatteryVoltage(),io.analogInManifoldPressre(),io.analogInTPS(),io.analogInFuelTemp(),io.analogInCoolantTemp(),
		io.analogInAuxTemp(),io.analogInAuxIn1(),io.analogInIntTemp(),io.analogQAPosition());
*/		
	//HAL_ADC_Start_DMA(&hadc1,(uint32_t*)adc1buf,8);
		
}

/* Called @ 1000hz */ 
volatile char edcRunning=0;
void edcScheluder() {
	if (!edcRunning)
		return;
	if (uwTick%2 == 0) {
		// 500hz
	}
	if (uwTick%20 == 0) {
		// 50hz
	}
	HAL_ADC_Start_DMA(&hadc1,(uint32_t*)adc1buf,8);
	HAL_ADC_Start_DMA(&hadc2,(uint32_t*)adc2buf,1);
	
}

void edcHandleSystemEvent(const char*name,const char *data) {
//	serial.printf("System event:%s, data:%s\n",name,data);
	if (strcmp(name,"_INI") == 0) {
		serial.printf("%c_RDY:DMN-EDC32 ready. version:20171026%c",0x02,0x03);					
		return;
	}
	// if we are here, event is unhandled, log it
	serial.printf("Unsupported event:%s\n",name);
}

void edcMain() {
	const int cmdBufSize = 512;
	char *cmdBuf = (char*)malloc(cmdBufSize);
	setup();
	serial.print("edcMain()");

	edcRunning = 1;
	static char loop=0;
	static char commandState=0;
	unsigned int cmdIdx=0;
	// main loop
	while (true) {
		if (serial.available()) {
			char c=serial.read();
			if (c==0x02) {
				memset(cmdBuf,0,cmdBufSize);
				commandState=1;
				cmdIdx=0;
				continue;
			}
			if (c==0x03) {
				commandState=0;
				cmdBuf[cmdIdx] = 0;
				edcHandleSystemEvent(cmdBuf,(char*)(&cmdBuf+commandState));
				continue;
			}
			if (commandState) {
				// TODO commandState also acts as key:value separator (:), check length
				if (cmdIdx < cmdBufSize-1) {
					if (c == ':') {
						cmdBuf[cmdIdx] = 0;
						commandState = cmdIdx+1;
					} else {
						cmdBuf[cmdIdx] = c;
					}
					cmdIdx++;
				}
			} else {
				// error
			}

		}
		loop++;
		if ((loop % 32) == 0) {
//			serial.printf("RPM:%4d    Timing:%5d  \r\n",rpm.getLatestMeasure(),rpm.getInjectionTiming());			
			serial.printf("%c1001:5600\n1000:%d%c",0x02,loop,0x03);			
			
		}

		__probe();
		
//		__HAL_TIM_SET_COUNTER(&htim15,0);
//		z=__HAL_TIM_GET_COUNTER(&htim15);
//		if (z<zz) {
//			serial.printf("ms:%d\r\n",(e-s));
//			s=HAL_GetTick();
//7			
//		}
//		e=HAL_GetTick();
//		zz=z;
//		void HAL_TIM_IRQHandler (TIM_HandleTypeDef * htim)
		refreshFastSensors();
        refreshSlowSensors();
        doBoostControl();
        doTimingControl();
        doRelayControl();
        doIdlePidControl();

//		handleUserInterface();
//		dtc.save();

		HAL_Delay(1000/60);		
	}
		
/*
	dac.setCustomWaveAmplitude(a);
	HAL_ADC_Start(&hadc1);
	HAL_ADC_PollForConversion(&hadc1, 1111);
	QApos1 = HAL_ADC_GetValue(&hadc1);
	QApos2 = HAL_ADC_GetValue(&hadc1);

	HAL_ADC_Stop(&hadc1);
*/

}
