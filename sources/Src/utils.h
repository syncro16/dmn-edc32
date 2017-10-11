#ifndef UTILS_H
#define UTILS_H


//long map(long x, long in_min, long in_max, long out_min, long out_max);
float map(float x, float in_min, float in_max, float out_min, float out_max);

void toggleHumanReadableValues();

void printValue(int i,char type);

unsigned char mapValues(int raw,int mapMin,int mapMax);

float mapInterpolate(float p1,float p2, float amount);
float mapLookUp(unsigned char *mapData,unsigned char x,unsigned char y);

void printMapAxis(unsigned char axisType,unsigned char mapIdx,bool verbose);

int toKpa(int raw);

int toTemperature(int rawValue);

int toVoltage(int raw);

int toRpm(int raw);

int toTps(int raw);

//unsigned char simulatePressureActuator(int maxPressure,int currentPressure);

#endif
