#include "Core.h"
#include "stm32f3xx_hal.h"
#include "Serial.h"
#include "DTC.h"
#include "ConfEditor.h"
#include <string.h>

Core core;

Core::Core() {
	// Initialize core parameters
	// (file_id in EEPROM, initial_value, min, max, increment_step, bindedRawValue, bindedActualValue,isLocked?,type,description)

	node[nodeSoftwareVersion] = (nodeStruct) {0x0001,VERSION_NUMBER,0x0103,9999,1,valueEngineRPMMin,valueEngineRPMMax,NODE_PROPERTY_LOCKED,VALUE_INT,"Software version"};  
	node[nodeEngineRPM] =       (nodeStruct) {0x1001,0,0,0,1,valueEngineRPMFiltered,valueEngineRPMJitter, NODE_PROPERTY_LOCKED,VALUE_INT,"Engine RPM"};
	node[nodeEngineTiming] =    (nodeStruct) {0x1002,0,0,0,1,valueEngineTimingActual,valueNone, NODE_PROPERTY_LOCKED,VALUE_INJECTION_TIMING,"Injection Advance"};  
	node[nodeTempEngine] =      (nodeStruct) {0x1003,87+64,255,0,1,valueTempEngine,valueNone, NODE_PROPERTY_EDITABLE,VALUE_CELSIUS,"Engine temperature"};  
	node[nodeTempFuel] =        (nodeStruct) {0x1004,45+64,0,255,1,valueTempFuel,valueNone, NODE_PROPERTY_EDITABLE,VALUE_CELSIUS,"Fuel temperature"};  
	node[nodeTempAir] =         (nodeStruct) {0x1005,45+64, 0,255,1,valueTempAir,valueNone, NODE_PROPERTY_EDITABLE,VALUE_CELSIUS,"Air temperature"};  
	node[nodePressure] =        (nodeStruct) {0x1006,0,0,0,1,valueBoostPressure,valueNone, NODE_PROPERTY_LOCKED,VALUE_KPA,"Boost pressure"};  
	node[nodeHeartBeat] =       (nodeStruct) {0x1007,0,1,3,1,valueEngineRPMRaw,valueNone, NODE_PROPERTY_EDITABLE,VALUE_INT,"Heartbeat"};      
	node[nodeInjectionThresholdVoltage] = 
	                            (nodeStruct) {0x1000,0,0,1,1,valueInjectionThresholdVoltage,valueNone, NODE_PROPERTY_LOCKED,VALUE_VOLTAGE,"Injection trigger voltage ref."};     
	node[nodeBatteryVoltage] =  (nodeStruct) {0x1008,0,0,1,1,valueBatteryVoltage,valueNone, NODE_PROPERTY_LOCKED,VALUE_BATTERY_VOLTAGE,"Battery Voltage"};     
	node[nodeRunMode] =         (nodeStruct) {0x1009,0,0,1,1,valueRunMode,valueNone, NODE_PROPERTY_LOCKED,VALUE_INT,"Running State"};     
	node[nodeFuelTrim] =        (nodeStruct) {0x100A,0,0,1,1,valueNone,valueFuelTrim, NODE_PROPERTY_EDITABLE,VALUE_BOOLEAN,"Fuel trim enable"};

	node[nodeTPSMin] = 	        (nodeStruct) {0x1010,201,25,1000,1,valueTPSRaw,valueTPSActual,NODE_PROPERTY_EDITABLE,VALUE_VOLTAGE,"TPS: signal min limit"};  
	node[nodeTPSMax] =          (nodeStruct) {0x1011,885,25,1000,1,valueTPSRaw,valueTPSActual,NODE_PROPERTY_EDITABLE,VALUE_VOLTAGE,"TPS: signal max limit"};
	node[nodeTPSSafetyBits] =   (nodeStruct) {0x1012,0,0,255,1,valueNone,valueNone,NODE_PROPERTY_EDITABLE,VALUE_INT,"TPS: safety bits (0=off, 1=idleSw/WotSW)"};  
	node[nodeFuelCutAtStall] =  (nodeStruct) {0x1013,1,0,1,1,valueNone,valueNone,NODE_PROPERTY_EDITABLE,VALUE_BOOLEAN,"Injection: fuel cut when engine is stopped"};  
	node[nodeQAInjectorBalance] = 
	                            (nodeStruct) {0x1015,0,0,1,1,valueNone,valueNone,NODE_PROPERTY_EDITABLE,VALUE_BOOLEAN,"Injection: automatic balance"};  
	node[nodeTimingMethod] =    (nodeStruct) {0x1014,0,0,2,1,valueNone,valueTimingPIDAmount,NODE_PROPERTY_EDITABLE,VALUE_INT,"Injection: advance (0=off/1=open/2=closed)"};  
	                            
	node[nodeProbeSignalOutput] = 
	                            (nodeStruct) {0x1016,0,0,10,1,valueNone,valueNone,NODE_PROPERTY_EDITABLE,VALUE_INT,"nodeProbeSignalOutput"};  
	node[nodeFreqConvRatio] =   (nodeStruct) {0x1017,100,1,1000,1,valueNone,valueNone,NODE_PROPERTY_EDITABLE,VALUE_INT,"nodeFreqConvRatio"};  

	node[nodeIdleAdjusting] =   (nodeStruct) {0x1018,0,0,1,1,valueNone,valueIdlePIDCorrection,NODE_PROPERTY_EDITABLE,VALUE_BOOLEAN,"Idle: adjusting enabled"};  
	node[nodeIdleSpeedTarget] = (nodeStruct) {0x1019,830,350,1600,1,valueEngineRPMFiltered,valueNone,NODE_PROPERTY_EDITABLE,VALUE_INT,"Idle: speed target"};  
	node[nodeIdleKp] =          (nodeStruct) {0x101A,9,0,300,1,valueNone,valueNone,NODE_PROPERTY_EDITABLE,VALUE_INT,"Idle: PID Control Kp"};  
	node[nodeIdleKi] =          (nodeStruct) {0x101B,1,0,300,1,valueNone,valueNone,NODE_PROPERTY_EDITABLE,VALUE_INT,"Idle: PID Control Ki"};  
	node[nodeIdleKd] =          (nodeStruct) {0x1032,13,0,300,1,valueNone,valueNone,NODE_PROPERTY_EDITABLE,VALUE_INT,"Idle: PID Control Kd"};  
	node[nodeIdlePIDSpeed] =    (nodeStruct) {0x1035,15,1,140,1,valueNone,valueNone,NODE_PROPERTY_EDITABLE,VALUE_INT,"Idle: PID Control Speed"};  	
	node[nodeIdlePIDBias] =     (nodeStruct) {0x1036,33,1,200,1,valueNone,valueNone,NODE_PROPERTY_EDITABLE,VALUE_INT,"Idle: PID Control Bias"};  
	node[nodeIdleMaxFuel] =     (nodeStruct) {0x1033,520,0,1024,5,valueNone,valueNone,NODE_PROPERTY_EDITABLE,VALUE_INT,"Idle: PID Control Max Fuel"};  
	node[nodeIdleMinFuel] =     (nodeStruct) {0x1034,140,0,1024,5,valueNone,valueNone,NODE_PROPERTY_EDITABLE,VALUE_INT,"Idle: PID Control Min Fuel"};  

	node[nodeRPMDSP] =          (nodeStruct) {0x101C,0,0,3,1,valueNone,valueNone,NODE_PROPERTY_EDITABLE,VALUE_INT,"RPM: DSP (0=direct/1=avg/2=2xteeth)"};  

	node[nodeQAFeedbackMin] =   (nodeStruct) {0x101D,205,1,1022,1,valueQAfeedbackActual,valueQAfeedbackRaw,NODE_PROPERTY_EDITABLE,VALUE_VOLTAGE,"QA Feedback: signal min limit"};
	node[nodeQAFeedbackMax] =   (nodeStruct) {0x101E,881,1,1023,1,valueQAfeedbackActual,valueQAfeedbackRaw,NODE_PROPERTY_EDITABLE,VALUE_VOLTAGE,"QA Feedback: signal max limit"};
	node[nodeQAReferenceEnabled] = 
								(nodeStruct) {0x101F,0,0,1,1,valueNone,valueNone,NODE_PROPERTY_EDITABLE,VALUE_BOOLEAN,"QA Feedback: HDK reference enabled",};  
	node[nodeQASetPoint] =      (nodeStruct) {0x1020,0,0,1023,1,valueNone,valueQAfeedbackSetpoint,NODE_PROPERTY_LOCKED,VALUE_INT,"QA servo: setPoint"};  
	node[nodeQAMinPWM] =        (nodeStruct) {0x1021,100,1,700,1,valueNone,valueQAPWMActual,NODE_PROPERTY_EDITABLE,VALUE_INT,"QA servo: min PWM cycle (x/1024)"};      
	node[nodeQAMaxPWM] =        (nodeStruct) {0x1022,650,1,800,1,valueNone,valueQAPWMActual,NODE_PROPERTY_EDITABLE,VALUE_INT,"QA servo: max PWM cycle (x/1024)"};     
	node[nodeMAPMin] =          (nodeStruct) {0x1023,138,25,1000,1,valueMAPRaw,valueMAPActual,NODE_PROPERTY_EDITABLE,VALUE_VOLTAGE,"MAP: signal min limit (at 100kPa)"};  
	node[nodeMAPMax] =          (nodeStruct) {0x1024,435,25,1000,1,valueMAPRaw,valueMAPActual,NODE_PROPERTY_EDITABLE,VALUE_VOLTAGE,"MAP: signal max limit"};
	node[nodeMAPkPa] =          (nodeStruct) {0x1025,250,100,1000,5,valueBoostPressure,valueNone,NODE_PROPERTY_EDITABLE,VALUE_KPA,"MAP: sensor max pressure (control-map scale)"};  
	node[nodeControlMapScaleRPM] = 
	                            (nodeStruct) {0x1026,5000,100,10000,100,valueNone,valueNone,NODE_PROPERTY_EDITABLE,VALUE_INT,"RPM: engine max RPM (control-map scale)",};  
	node[nodeGenericDebugValue] = 
	                            (nodeStruct) {0x1027,0,0,1023,16,valueNone,valueNone,NODE_PROPERTY_EDITABLE,VALUE_KPA,"Generic variable for debug"};  
	node[nodeQADebugJitter] =   (nodeStruct) {0x1028,0,0,1024,1,valueNone,valueQAJitter,NODE_PROPERTY_LOCKED,VALUE_INT,"QA Debug: setPoint diff / FB Jitter"};  
	node[nodeQAPIDKp] =         (nodeStruct) {0x1029,80,0,1000,1,valueNone,valueQAPIDPparam,NODE_PROPERTY_EDITABLE,VALUE_INT,"QA PID Control: Kp factor"};  
	node[nodeQAPIDKi] =         (nodeStruct) {0x102A,3,0,1000,1,valueNone,valueQAPIDIparam,NODE_PROPERTY_EDITABLE,VALUE_INT,"QA PID Control: Ki factor"};  
	node[nodeQAPIDKd] =         (nodeStruct) {0x102B,3,0,1000,1,valueNone,valueQAPIDDparam,NODE_PROPERTY_EDITABLE,VALUE_INT,"QA PID Control: Kd factor"};  
	node[nodeQAPIDSpeed] =      (nodeStruct) {0x102C,25,1,128,1,valueNone,valueNone,NODE_PROPERTY_EDITABLE,VALUE_INT,"QA PID Control: Speed"};     
	node[nodeQAPIDBias] =       (nodeStruct) {0x102D,55,1,200,5,valueNone,valueNone,NODE_PROPERTY_EDITABLE,VALUE_INT,"QA PID Control: Bias"};      

	node[nodeBoostAdjusting] =  (nodeStruct) {0x102E,0,0,4,1,valueN75DutyCycleBaseForPid,valueBoostPIDCorrection,NODE_PROPERTY_EDITABLE,VALUE_INT,"Boost Control (0=off/1=open/2=closed/3/4)"};      
	node[nodeBoostSpeed] =      (nodeStruct) {0x102F,5,1,200,10,valueNone,valueNone,NODE_PROPERTY_EDITABLE,VALUE_INT,"Boost Control: PID Speed"};      
	node[nodeBoostKp] =         (nodeStruct) {0x1030,5,1,2000,1,valueNone,valueNone,NODE_PROPERTY_EDITABLE,VALUE_INT,"Boost Control: PID Kp factor"};      
	node[nodeBoostKi] =         (nodeStruct) {0x1031,5,1,2000,1,valueNone,valueNone,NODE_PROPERTY_EDITABLE,VALUE_INT,"Boost Control: PID Ki factor"};      
	node[nodeBoostKd] =         (nodeStruct) {0x1032,0,0,2000,1,valueNone,valueNone,NODE_PROPERTY_EDITABLE,VALUE_INT,"Boost Control: PID Kd factor"};  
	node[nodeBoostBias] =    	(nodeStruct) {0x1038,100,1,200,1,valueNone,valueNone,NODE_PROPERTY_EDITABLE,VALUE_INT,"Boost Control: PID Control Bias"};  
	node[nodeBoostPIDRange] =   (nodeStruct) {0x1039,20,1,200,1,valueNone,valueBoostPIDCorrection,NODE_PROPERTY_EDITABLE,VALUE_INT,"Boost Control: PID Range"};  
	node[nodeBoostTargetPressure] =   (nodeStruct) {0x0,0,1,200,1,valueBoostTarget,valueNone,NODE_PROPERTY_LOCKED,VALUE_KPA,"Boost Control: Target Pressure"};  
	node[nodeBoostActualPressure] =   (nodeStruct) {0x0,0,1,200,1,valueBoostPressure,valueNone,NODE_PROPERTY_LOCKED,VALUE_KPA,"Boost Control: Current Pressure"};  

	node[nodeTimingKp] =         (nodeStruct) {0x103A,10,1,200,1,valueNone,valueNone,NODE_PROPERTY_EDITABLE,VALUE_INT,"Advance Control: PID Kp factor"};      
	node[nodeTimingKi] =         (nodeStruct) {0x103B,2,1,200,1,valueNone,valueNone,NODE_PROPERTY_EDITABLE,VALUE_INT,"Advance Control: PID Ki factor"};   

	node[nodeActuatorPos] = (nodeStruct) {0x5000,800,80,920,5,valueNone,valueActuatorActualPosition,NODE_PROPERTY_EDITABLE,VALUE_INT,"Actuator: Target position"};  	
	node[nodeActuatorMinPos] = (nodeStruct) {0x5001,200,80,920,5,valueNone,valueActuatorSetPoint,NODE_PROPERTY_EDITABLE,VALUE_INT,"Actuator: low position stop"};  
	node[nodeActuatorMaxPos] = (nodeStruct) {0x5002,800,80,920,5,valueNone,valueActuatorSetPoint,NODE_PROPERTY_EDITABLE,VALUE_INT,"Actuator: high position stop"};  
	node[nodeActuatorMaxPWM] = (nodeStruct) {0x5003,200,5,255,5,valueNone,valueNone,NODE_PROPERTY_EDITABLE,VALUE_PWM8,"Actuator: P-value (responsiveness, speed)"};  

	node[nodeActuatorP] = (nodeStruct) {0x5004,135,0x0,4000,5,valueNone,valueNone,NODE_PROPERTY_EDITABLE,VALUE_INT,"Actuator: I-value (stability)"};  
	node[nodeActuatorI] = (nodeStruct) {0x5005,20,0x0,4000,5,valueNone,valueNone,NODE_PROPERTY_EDITABLE,VALUE_INT,"Actuator: calculated current PWM"};  
	node[nodeActuatorPWM] = (nodeStruct) {0x5006,0,0,0,1,valueNone,valueNone,NODE_PROPERTY_LOCKED,VALUE_PWM8,"Actuator: Max PWM"};  
	node[nodeActuatorHysteresis] = (nodeStruct) {0x5007,11,0,100,1,valueNone,valueNone,NODE_PROPERTY_EDITABLE,VALUE_INT,"Actuator: hysteresis"};  
	node[nodeActuatorDirection] = (nodeStruct) {0x5008,0,0,200,1,valueNone,valueActuatorSetPoint,NODE_PROPERTY_LOCKED,VALUE_INT,"Actuator: motor direction"};  
	node[nodeActuatorSteppingDelay] = (nodeStruct) {0x5009,0,0,200,1,valueNone,valueNone,NODE_PROPERTY_EDITABLE,VALUE_MS,"Actuator: stepping delay (0=continuous loop)"}; 

	currentNode = LIST_RESET;


	/* MAP header <map-id>, 0xf0, .... 
	Map id is used for saving/loading maps from eeprom, use unique id
	*/ 
   
	/* 0xf? Fuel injections maps */
	/* basic injection amount */

	static unsigned char fuelMap[] = {
	  0xF0,0xF0,'M','2','D',
	  0x8,0x6,MAP_AXIS_RPM,MAP_AXIS_TPS,MAP_AXIS_INJECTED_FUEL,
 		180,	80,     35,     0,      0,      0,      0,      0,
		180,    98,    98,    37,     0,      0,      0,      0,
	    180,    109,    109,    79,    37,     10,      0,      0,
	    180,    109,    123,    134,    143,    87,     10,     0,
        180,    109,    131,    151,    156,    156,    77,     0,
        180,    109,    150,    156,    156,    164,    164,    0,      
	  0,0,0,0,0,0,0                // lastX,lastY,lastRet float(4b)
	 };

	/* additive (injection) fuel map for boost */

	static unsigned char boostMap[] = {
	  0xF1,0xF0,'M','2','D',
	  0x8,0x6,MAP_AXIS_RPM,MAP_AXIS_KPA,MAP_AXIS_INJECTED_FUEL,
	  0,0,0,0,     0,0,0,0,
	  31,31,31,31, 31,31,31,0,
	  53,47,65,65, 75,75,75,0,
	  30,30,78,78, 85,85,85,0,	  
	  0,0,0,0, 0,0,0,0,
	  0,0,0,0, 0,0,0,0,
	  0,0,0,0,0,0,0                // lastX,lastY,lastRet float(4b)
	};

	/* Cold Start & idle fuel map */
	static unsigned char idleMap[] = {
		0xF2,0xF0,'M','2','D',
		0x8,0x4,MAP_AXIS_IDLERPM,MAP_AXIS_CELSIUS,MAP_AXIS_INJECTED_FUEL,
		255,255,255,114, 80,60,60,50,
		190,130,90,80, 70,60,50,40,		
		190,120,90,80, 70,60,50,40,
		190,110,90,80, 70,60,50,40,
		0,0,0,0,0,0,0                // lastX,lastY,lastRet float(4b)
	};


	static unsigned char fuelTrimFuelTemp[] = {
		0xF3,0xF0,'M','1','D',
		0x8,0x1,MAP_AXIS_CELSIUS,MAP_AXIS_NONE,MAP_AXIS_FUEL_TRIM_AMOUNT,
		128,128,128,128,128,128,128,128,
		0,0,0,0,0,0,0                // lastX,lastY,lastRet float(4b)
	};
	
	static unsigned char fuelTrimAirTemp[] = {
		0xF4,0xF0,'M','1','D',
		0x8,0x1,MAP_AXIS_CELSIUS,MAP_AXIS_NONE,MAP_AXIS_FUEL_TRIM_AMOUNT,
		128,128,128,128,128,128,128,128,
		0,0,0,0,0,0,0                // lastX,lastY,lastRet float(4b)
	};

	/* 0xe? engine timing */
	/*  static mode */

	static unsigned char openLoopAdvanceMap[] = {
		0xE0,0xF0,'M','2','D',
		0x6,0x6,MAP_AXIS_RPM,MAP_AXIS_INJECTED_FUEL,MAP_AXIS_DUTY_CYCLE,
		255,255,255, 255,255,255, 
		255,255,255, 255,255,210, 
		255,255,255, 255,180,180, 
		255,255,255,    190,140,0, 
		255,255,255,    100,80,0, 
		100,100,100,    50,50,0, 
		0,0,0,0,0,0,0                // lastX,lastY,lastRet float(4b)
	};

	/* dynamic mode */

	static unsigned char closedLoopAdvanceMap[] = {
		0xE1,0xF0,'M','2','D',
		0x6,0x6,MAP_AXIS_RPM,MAP_AXIS_INJECTED_FUEL,MAP_AXIS_INJECTION_TIMING,
		0,0,0, 0,0,0, 
		0,0,0, 0,0,0, 
		0,0,0, 0,0,0,  

		0,0,0, 0,0,0, 
		0,0,0, 0,0,0, 
		0,0,0, 0,0,0,  
		0,0,0,0,0,0,0                // lastX,lastY,lastRet float(4b)
	};


	/*  0xd? - Turbo wastegate / VNT control maps */

	/* basic duty cycle vs rpm*injection amount */

	static unsigned char turboControlMap[] = {
		0xD0,0xF0,'M','2','D',
		0x6,0x6,MAP_AXIS_RPM,MAP_AXIS_INJECTED_FUEL,MAP_AXIS_DUTY_CYCLE,
		201,227,210,201,171,158,
		201,227,208,192,169,158,	
		201,182,198,180,162,158,
		201,201,195,166,153,140,
		201,182,118,151,151,28,
		201,182,156,103,59,28,		
		0,0,0,0,0,0,0                // lastX,lastY,lastRet float(4b)
	};

	/* target pressure map */
	static unsigned char turboTargetPressureMap[] = {
		0xD1,0xF0,'M','2','D',
		0x6,0x4,MAP_AXIS_RPM,MAP_AXIS_TPS,MAP_AXIS_KPA,
		0,0,0,0,0,0,
		0,0,0,10,20,100,	
		0,0,10,30,100,128,
		100,100,100,100,100,100,
		0,0,0,0,0,0,0                // lastX,lastY,lastRet float(4b)
	};

	/* actuator opening/operating curve */
	static unsigned char actuatorTension[] = {
		0xD2,0xF0,'M','1','D',
		0x8,0x1,MAP_AXIS_RAW,MAP_AXIS_NONE,MAP_AXIS_RAW,
		255,215,192,143,90,55,23,0,
		0,0,0,0,0,0,0                // lastX,lastY,lastRet float(4b)
	};			

	/* 0xc? - temperature related maps */

	static unsigned char glowPeriodMap[] = {
		0xC0,0xF0,'M','1','D',
		0x6,0x1,MAP_AXIS_CELSIUS,MAP_AXIS_NONE,MAP_AXIS_SECONDS,
		30,30,20,10,0,0,
		0,0,0,0,0,0,0                // lastX,lastY,lastRet float(4b)
	};

	/* 0xb? - temperature sensor calibration maps */

	static unsigned char engineTempSensorMap[] = {
		0xB0,0xF0,'M','1','D',
		0x6,0x1,MAP_AXIS_VOLTAGE,MAP_AXIS_NONE,MAP_AXIS_CELSIUS,
		30,20,9,0,0,0,
		0,0,0,0,0,0,0                // lastX,lastY,lastRet float(4b)
	};

	static unsigned char fuelTempSensorMap[] = {
		0xB1,0xF0,'M','1','D',
		0x6,0x1,MAP_AXIS_VOLTAGE,MAP_AXIS_NONE,MAP_AXIS_CELSIUS,
		30,30,20,10,0,0,
		0,0,0,0,0,0,0                // lastX,lastY,lastRet float(4b)
	};

	static unsigned char airTempSensorMap[] = {
		0xB2,0xF0,'M','1','D',
		0x6,0x1,MAP_AXIS_VOLTAGE,MAP_AXIS_NONE,MAP_AXIS_CELSIUS,
		30,30,20,10,0,0,
		0,0,0,0,0,0,0                // lastX,lastY,lastRet float(4b)
	};
	static unsigned char ecuTempSensorMap[] = {
		0xB3,0xF0,'M','1','D',
		0x6,0x1,MAP_AXIS_VOLTAGE,MAP_AXIS_NONE,MAP_AXIS_CELSIUS,
		30,30,20,10,0,0,
		0,0,0,0,0,0,0                // lastX,lastY,lastRet float(4b)
	};

	/* 0x7? Generic control maps */

	static unsigned char idlePidP[] = {
		0x70,0xF0,'M','1','D',
		10,0x1,MAP_AXIS_IDLERPM,MAP_AXIS_NONE,MAP_AXIS_RAW,
		12,12,10,6,4, 2,2,2,5,10,
		0,0,0,0,0,0,0                // lastX,lastY,lastRet float(4b)
	};		

	mapNames[Core::mapIdxFuelMap] = "Basic Injection Map";
	mapNames[Core::mapIdxBoostMap] = "Additive Injection Map (Boost)";
	mapNames[Core::mapIdxIdleMap] = "Injection quantity when starting / idling";
	mapNames[Core::mapIdxOpenLoopAdvanceMap] = "Open Loop advance";
	mapNames[Core::mapIdxClosedLoopAdvanceMap] = "Closed Loop advance";
	mapNames[Core::mapIdxTurboControlMap] = "Turbo actuator; duty cycle base map";
	mapNames[Core::mapIdxTurboTargetPressureMap] = "Turbo actuator; target pressure";
	mapNames[Core::mapIdxGlowPeriodMap] = "Glow period (seconds)";
	mapNames[Core::mapIdxEngineTempSensorMap] = "Engine temp. sensor calibration";
	mapNames[Core::mapIdxFuelTempSensorMap] = "Fuel temp. sensor calibration";
	mapNames[Core::mapIdxAirTempSensorMap] = "Intake air temp. sensor calibration";
	mapNames[Core::mapIdxEcuTempSensorMap] = "Ecu temp. sensor calibration";

	mapNames[Core::mapIdxFuelTrimFuelTemp] = "Fuel Trim amount vs. Fuel Temp.";
	mapNames[Core::mapIdxFuelTrimAirTemp] = "Fuel Trim amount vs. Air Temp.";
	mapNames[Core::mapIdxActuatorTension] = "Turbo Actuator Operating Curve";
	mapNames[Core::mapIdxIdlePidP] = "Idle PID P-parameter during idle";


	maps[Core::mapIdxFuelMap] = (unsigned char*)&fuelMap;
	maps[Core::mapIdxIdleMap] = (unsigned char*)&idleMap;
	maps[Core::mapIdxBoostMap] = (unsigned char*)&boostMap;
	maps[Core::mapIdxOpenLoopAdvanceMap] = (unsigned char*)&openLoopAdvanceMap;
	maps[Core::mapIdxClosedLoopAdvanceMap] = (unsigned char*)&closedLoopAdvanceMap;
	maps[Core::mapIdxTurboControlMap] = (unsigned char*)&turboControlMap;
	maps[Core::mapIdxTurboTargetPressureMap] = (unsigned char*)&turboTargetPressureMap;
	maps[Core::mapIdxGlowPeriodMap] = (unsigned char*)&glowPeriodMap;
	maps[Core::mapIdxEngineTempSensorMap] = (unsigned char*)&engineTempSensorMap;
	maps[Core::mapIdxFuelTempSensorMap] = (unsigned char*)&fuelTempSensorMap;
	maps[Core::mapIdxAirTempSensorMap] = (unsigned char*)&airTempSensorMap;
	maps[Core::mapIdxEcuTempSensorMap] = (unsigned char*)&ecuTempSensorMap;

//	maps[Core::mapIdxFuelTrimMap] = (unsigned char*)&fuelTrimMap;
	maps[Core::mapIdxFuelTrimFuelTemp] = (unsigned char*)&fuelTrimFuelTemp;
	maps[Core::mapIdxFuelTrimAirTemp] = (unsigned char*)&fuelTrimAirTemp;
	maps[Core::mapIdxActuatorTension] = (unsigned char*)&actuatorTension;
	maps[Core::mapIdxIdlePidP] = (unsigned char*)&idlePidP;

	numberOfMaps=15;
}

void Core::save() {

	uint32_t errLocation;
	
	FLASH_EraseInitTypeDef eraseType;
	uint32_t address = FLASH_OFFSET_SETTINGS;
	
	eraseType.TypeErase = FLASH_TYPEERASE_PAGES;
	eraseType.PageAddress = FLASH_OFFSET_SETTINGS;
	eraseType.NbPages = 2; // 4kB

	HAL_FLASH_Unlock();

	HAL_FLASHEx_Erase(&eraseType,&errLocation);	

	// header 2 byte
	HAL_FLASH_Program(FLASH_TYPEPROGRAM_HALFWORD, address, CONFIGURATION_FILE_2BYTE_ID);
	address += 2;
		
	// Nodes (2 byte align)
	unsigned char idx;
	uint16_t nibble;
	for (idx = 0;idx<NODE_MAX;idx++) {
		nibble = node[idx].fileId;
		HAL_FLASH_Program(FLASH_TYPEPROGRAM_HALFWORD, address,nibble);
		address += 2;
		HAL_FLASH_Program(FLASH_TYPEPROGRAM_HALFWORD, address,2);
		address += 2;	
		nibble = node[idx].value;		
		HAL_FLASH_Program(FLASH_TYPEPROGRAM_HALFWORD, address,nibble);
		address += 2;	
	}	

	// Maps (2 byte align)
	for (idx = 0;idx<numberOfMaps;idx++) {
		char x = core.maps[idx][5];
		char y = core.maps[idx][6];
		uint16_t size = x*y+15; // actually 15, but rest of fields are not necessary to save
		uint16_t* nibble = (uint16_t*) core.maps[idx];
		
		HAL_FLASH_Program(FLASH_TYPEPROGRAM_HALFWORD, address,nibble[0]);
		address += 2;
		HAL_FLASH_Program(FLASH_TYPEPROGRAM_HALFWORD, address,size);
		address += 2;	
		nibble = (uint16_t*) core.maps[idx];				
		for (unsigned char mapofs=0;mapofs<size/2;mapofs++) {
			// Possible out-of-range error, but not a big deal because we are only reading
			HAL_FLASH_Program(FLASH_TYPEPROGRAM_HALFWORD, address, nibble[mapofs]);
			address += 2;
		}
	}	
	
	// EOF File ID
	HAL_FLASH_Program(FLASH_TYPEPROGRAM_HALFWORD, address, 0xFFFF);
	
	HAL_FLASH_Lock();
	confeditor.setSystemStatusMessage("Saved!");
}


bool Core::load() {
	uint32_t address = FLASH_OFFSET_SETTINGS;	
	uint16_t *buf = (uint16_t*)address;
	int idx=0;
	uint16_t fileId;
	uint16_t size;
	unsigned char totalNodes=0,totalMaps=0;

	if (buf[0] == CONFIGURATION_FILE_2BYTE_ID) {
		idx++;
		do {
			fileId = buf[idx];
			idx++;
			if (fileId == 0xFFFF)
				break;
			size = buf[idx];
			idx++;
			
			switch ((fileId & 0xFF00)) {
				case 0x1000:
				
					if (size == 2) {
						for (unsigned char i = 0;i<NODE_MAX;i++) {
							if (node[i].fileId == fileId) {
								memcpy(&node[i].value,(char*)&buf[idx],2);
								totalNodes++;
							}
						}
					} else {
						dtc.setError(DTC_CONF_PARTIAL_MISMATCH);
					}
				break;
				case 0xF000:
				
					for (unsigned char i = 0;i<numberOfMaps;i++) {	
						unsigned int mapId;
						mapId = (int)core.maps[i][1]*256+core.maps[i][0];
					
						if (mapId == fileId) {
							
							if (size == (core.maps[i][5]*core.maps[i][6]+15)) {
								totalMaps++;
								memcpy((char*)core.maps[i],(const char*)&buf[idx],size);				
							} else {
								dtc.setError(DTC_CONF_PARTIAL_MISMATCH);
							}
						}
					}						
				break;	
			}
			idx += size/2;
			
		} while (idx<4096); 
		
	} else {
		dtc.setError(DTC_CONF_MISMATCH);
		return false;
	}
	serial.printf("Loaded %d nodes and %d maps.\r\n",totalNodes,totalMaps);
	return true;
}

void Core::setCurrentNode(int start) {
	if (start<=NODE_MAX) { 
		currentNode = start;
	} else {
		currentNode = LIST_RESET;
	}
}

int Core::seekNextNode() {
	if (currentNode+1<=NODE_MAX) { 
		currentNode++;
	} else {
		currentNode = LIST_RESET;
	}
	return currentNode;
}

nodeStruct* Core::getNextNode() {
	seekNextNode();
	return getNodeData();
}

nodeStruct* Core::getNodeData() {
	if (currentNode == LIST_RESET)
		return NULL;
	return &node[currentNode];
}

void Core::incValue() {
	setValue(node[currentNode].value+node[currentNode].step);
}

void Core::decValue() {
	setValue(node[currentNode].value-node[currentNode].step);
}

void Core::setValue(int value) {
	if (node[currentNode].properties != NODE_PROPERTY_LOCKED) {
		if (value < node[currentNode].min) 
			value = node[currentNode].min;
		if (value > node[currentNode].max) 
			value = node[currentNode].max;
	
		node[currentNode].value = value;
	}
}