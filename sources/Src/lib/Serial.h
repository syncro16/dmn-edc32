#include <stdint.h>
#include <stdarg.h>
class Serial {
	private:
//	char buf[160];
	char *buf;
		
	bool dmaRxMode;
    public:
	volatile  uint8_t dmaBuf[2];
	volatile  uint8_t receiveRingBuffer[128];
	volatile  uint16_t receiveRingBufferReadIdx;
	volatile  uint16_t receiveRingBufferWriteIdx;	
	volatile  uint16_t receiveRingBufferLength;
	volatile  uint8_t receiveOverflowError;
	private:
	public:
	Serial();	
	void enableDmaRx();
	void print(int i);	
	void printf(const char *fmt,...);
    void printf(const char *fmt, va_list args1);
	void print(const char *c);
	void printPads(unsigned char n, char padChar);
	void printWithPadding(int val,unsigned char width,char padChar);
	void printWithPadding(const char *str,unsigned char width,char padChar);
	void write(const char c);  

	void ansiGotoXy(char x,char y);	
	void ansiClearScreen();	
	void ansiClearEolAndLf();
	void ansiClearEol();	
	void ansiClearEos();
	void ansiHideCursor(bool status);
	void ansiSetReverse(bool status);		
	void ansiSetBold(bool status);		
	void ansiReset();		
	
    int available();
	int poll(uint32_t timeoutMs);	
	void appendReceiveRingBuffer(uint8_t c);
	
    uint8_t read();    
    uint32_t read(uint8_t *dst,uint32_t size);   
	
};
extern Serial serial;
