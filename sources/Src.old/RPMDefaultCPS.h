//#ifndef RPMDEFAULTCPS_H
//#define RPMDEFAULTCPS_H

#include "RPMBase.h"
#include "Core.h"

class RPMDefaultCps : public RPMBase {
	public:
	void init();
	unsigned int getLatestMeasure();
	unsigned int getLatestRawValue();
	int getInjectionTiming();
	private:
	void setupTimers();

};


//#endif
