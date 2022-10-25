#include "IO.h"
/*
  HAL_TIM_Base_Start(&htim1); 
  HAL_TIM_PWM_Start(&htim1,TIM_CHANNEL_1); // TIM_CHANNEL_ALL fails!*/

extern volatile uint16_t adc1buf[16];
extern volatile uint16_t adc2buf[1];

  
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
	if (rpm == 0) {
		htim17.Instance->CCR1 = 0;
		return;
	}
	// 65535 = 1Hz
	htim17.Instance->CCR1 = 65*2;  // 2ms
	htim17.Instance->ARR = 65535/(rpm/60);
	/* Set the Autoreload value */
//	TIMx->ARR = (uint32_t)Structure->Period ;
}
void IO::pwmOutQuantityAdjuster_12b(uint16_t a) {
	htim1.Instance->CCR4 = a;  
}
void IO::pwmOutTiming_8b(uint8_t a) {
	htim2.Instance->CCR1 = a;  	
}
void IO::pwmOutBoostSolenoid_8b(uint8_t a) {
	htim2.Instance->CCR2 = a;  	
}
void IO::pwmOutAux_8(uint8_t a) {
	htim2.Instance->CCR3 = a;  		
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
uint16_t IO::analogInBatteryVoltage() {
	return adc1buf[0];
}
uint16_t IO::analogInManifoldPressre() {
	return adc1buf[1];	
}
uint16_t IO::analogInTPS() {
	return adc1buf[2];
}
uint16_t IO::analogInFuelTemp() {
	return adc1buf[3];	
}
uint16_t IO::analogInCoolantTemp() {
	return adc1buf[4];	
}
uint16_t IO::analogInAuxTemp() {
	return adc1buf[5];	
}
uint16_t IO::analogInAuxIn1() {
	return adc1buf[6];	
}
uint16_t IO::analogInIntTemp() {
	return adc1buf[7];	
}
uint16_t IO::analogQAPosition() {
	return adc2buf[0];	
}