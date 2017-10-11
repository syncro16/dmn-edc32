#include <stdint.h>

/* Sample buffer size, we are generating 10kHz sinewave output with 128 samples = 1,28MHz sample rate :-) See TIMer6 setup */
#define DAC_WAVETABLE_SIZE 128
/* it seems DAC has some problems with rail-to-rail operation, so limit max values a bit*/
//#define DAC_MAX_LINEAR_RANGE 0x7B0 
#define DAC_MAX_LINEAR_RANGE_R (0x650/2)
#define DAC_MAX_LINEAR_RANGE_C (0x650)
#define DAC_OFS (0x0FF)

class Dac {
	private:
	uint16_t refWave[DAC_WAVETABLE_SIZE];
	uint16_t customWave[DAC_WAVETABLE_SIZE]; 
	float baseLut[DAC_WAVETABLE_SIZE];

	public:
	
	private:
	public:
	Dac();
	// start DAC1 ch1&ch2 output
	void startDmaSineGenerator();
	// set DAC2 ch1 output voltage
	void setCustomWaveAmplitude(float amp);
	void analogOut(int v);
};

extern volatile int dac_buf_updating;
extern volatile int dac_buf_collisions;


extern Dac dac;