#include "Serial.h"
#include <string.h>
#include "stm32f3xx_hal.h"
#include "stm32f3xx_hal_usart.h"
#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include "../edcMain.h"

extern UART_HandleTypeDef huart2;

Serial serial;


Serial::Serial() {
	dmaRxMode=0;
	receiveRingBufferWriteIdx=0;
	receiveRingBufferReadIdx=0;	
	receiveRingBufferLength=0;
	receiveOverflowError=0;
//	setvbuf(stdout, NULL, _IONBF, 160);	
//    HAL_UART_Receive_IT(&huart2, buz,1);
	buf=(char*)malloc(160);
}

void Serial::enableDmaRx() {
	dmaRxMode=1;
	HAL_UART_Receive_DMA(&huart2,(uint8_t*)dmaBuf,2);
}

void Serial::write(const char c) {
	HAL_UART_Transmit(&huart2,(uint8_t*)&c,1,0);
}

void Serial::print(const char *c) {
	HAL_UART_Transmit(&huart2,(uint8_t*)c,strlen(c),1000);
}

void Serial::printf(const char *fmt,...) {
    va_list args;
    va_start (args, fmt);
    int l=vsprintf(buf,fmt,args);
	va_end(args);
	
    HAL_UART_Transmit(&huart2,(uint8_t*)buf,l,1000);
}

void Serial::printf(const char *fmt, va_list args1) {
	
	int l=vsprintf(buf,fmt,args1);
    HAL_UART_Transmit(&huart2,(uint8_t*)buf,l,1000);
}

void Serial::print(int i) {
	printf("%i",i);
}


void Serial::printPads(unsigned char n, char padChar) {
    memset(buf,padChar,n);
    buf[n] = 0;
    print(buf);
}

void Serial::printWithPadding(int val,unsigned char width,char padChar) {
    // print enough leading zeroes!
    memset(buf,padChar,30);
    // append string presentation of number to end
    itoa(val,buf+30,10);
    // print string with given width
    print(buf+30+strlen(buf+30)-width);
}

void Serial::printWithPadding(const char *str,unsigned char width,char padChar) {
    // print enough leading zeroes!
    memset(buf,padChar,30);
    // append string presentation of number to end
    strcpy(buf+30, str);
    
    // print string with given width
    print(buf+30+strlen(buf+30)-width);
}

// VT102/Ansi functions

const char ANSIHideCursor[] = { 27,'[','?','2','5','h',0}; // doesn't work?

void Serial::ansiGotoXy(char x,char y) {
	printf("\e[%i;%iH",y,x);
}

void Serial::ansiClearScreen() {
	const char ANSIClearScreen[] =  { 27,'[','2','J',27,'[','H',0};	
  	print(ANSIClearScreen);
}

void Serial::ansiClearEolAndLf() {
	const char ANSIClearEolAndLf[] = { 27,'[','K','\r','\n',0};	
	print(ANSIClearEolAndLf);
}

void Serial::ansiClearEol() {
	const char ANSIClearEol[] = { 27,'[','K',0};	
    print(ANSIClearEol);
}

void Serial::ansiClearEos() {
	const char ANSIClearEos[] = { 27,'[','J',0};	
	print(ANSIClearEos);
}

void Serial::ansiHideCursor(bool status) {
	printf("\e[?25%s",status?"l":"m");	
}

void Serial::ansiSetReverse(bool status) {
	printf("\e[%sm",status?"7":"0");
}

void Serial::ansiSetBold(bool status) {
	printf("\e[%sm",status?"1":"0");
}
void Serial::ansiReset() {
	printf("\e[0m");
}

int Serial::poll(uint32_t timeoutMs) {
	int ret=0;
	if (dmaRxMode) {
		timeoutMs = timeoutMs+HAL_GetTick();
		while ((ret == 0) && (HAL_GetTick()<timeoutMs)) {
		}		
	} else {
		timeoutMs = timeoutMs+HAL_GetTick();
		while ((ret == 0) && (HAL_GetTick()<timeoutMs)) {
			if (available()) ret=1;
		}
	}
	return ret;
}

int Serial::available() {
   if (dmaRxMode) {
		return receiveRingBufferLength;
   } else {
	   return __HAL_USART_GET_FLAG(&huart2,USART_FLAG_RXNE);
   }
}

uint32_t Serial::read(uint8_t *dst,uint32_t size) {
	if (size>receiveRingBufferLength)
		size=receiveRingBufferLength;

	int len=size;
	while (len) {
		*dst++ = read();
		len--;
	}
	return size;
}

uint8_t Serial::read() {
	if (dmaRxMode) {
		receiveOverflowError=0;
		if (receiveRingBufferLength) {
			receiveRingBufferLength--;			
			uint8_t c = receiveRingBuffer[receiveRingBufferReadIdx];
			receiveRingBufferReadIdx = (receiveRingBufferReadIdx+1) % sizeof(receiveRingBuffer);	
			return c;
		} else {
			return 0;
		}
	} else {
		HAL_UART_AbortReceive(&huart2);	
		HAL_UART_Receive(&huart2, (uint8_t*)receiveRingBuffer,1,100);
	    return (char)receiveRingBuffer[0];		
	}
}

void Serial::appendReceiveRingBuffer(uint8_t c) {
	if (receiveRingBufferLength<sizeof(receiveRingBuffer)) {		
		receiveRingBuffer[receiveRingBufferWriteIdx]=c;
		receiveRingBufferLength++;
		receiveRingBufferWriteIdx = (receiveRingBufferWriteIdx+1) % sizeof(receiveRingBuffer);	
	} else {
		receiveOverflowError = true;
	}
		
}

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)   {
	serial.appendReceiveRingBuffer(serial.dmaBuf[1]);
}

void HAL_UART_RxHalfCpltCallback(UART_HandleTypeDef *huart) {
	serial.appendReceiveRingBuffer(serial.dmaBuf[0]);
}
