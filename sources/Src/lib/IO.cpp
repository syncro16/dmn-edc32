#include "IO.h"
/*
  HAL_TIM_Base_Start(&htim1); 
  HAL_TIM_PWM_Start(&htim1,TIM_CHANNEL_1); // TIM_CHANNEL_ALL fails!*/

IO io;

IO::IO() {
		
}
void IO::outputAux(bool state) {
	HAL_GPIO_WritePin(GPIOA,GPIO_PIN_8,state?GPIO_PIN_SET:GPIO_PIN_RESET);
}
void IO::outputFuelSolenoid(bool state) {
	HAL_GPIO_WritePin(GPIOC,GPIO_PIN_9,state?GPIO_PIN_SET:GPIO_PIN_RESET);	
}
void IO::outputGlowPlugs(bool state) {
	HAL_GPIO_WritePin(GPIOC,GPIO_PIN_8,state?GPIO_PIN_SET:GPIO_PIN_RESET);	
}
void IO::outputFanIntake(bool state) {
	HAL_GPIO_WritePin(GPIOC,GPIO_PIN_7,state?GPIO_PIN_SET:GPIO_PIN_RESET);		
}
void IO::outputMainPowerRelay(bool state) {
	HAL_GPIO_WritePin(GPIOC,GPIO_PIN_6,state?GPIO_PIN_SET:GPIO_PIN_RESET);			
}
void IO::pulseOutEngineSpeedSignal(int rpm) {
	// TODO set TIM17 PWM width.
}

bool IO::inputTPSWot() {
	return HAL_GPIO_ReadPin(GPIOB,GPIO_PIN_9);
}
bool IO::inputTPSIdle() {
	return HAL_GPIO_ReadPin(GPIOB,GPIO_PIN_8);	
}
bool IO::inputBrake() {
	return HAL_GPIO_ReadPin(GPIOB,GPIO_PIN_4);	
}
bool IO::inputCruiseSet() {
	return HAL_GPIO_ReadPin(GPIOB,GPIO_PIN_6);
}
bool IO::inputCruiseDecel() {
	return HAL_GPIO_ReadPin(GPIOB,GPIO_PIN_5);	
}
bool IO::inputAux() {
	return HAL_GPIO_ReadPin(GPIOC,GPIO_PIN_12);	
}