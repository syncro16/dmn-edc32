// Diagnostic trouble code(s)
//
// Stores DTC on eeprom. Only one error per power-on period is registered (unless DTC is cleared). Count of errors are stored in eeprom too.


#ifndef DTH_H
#define DTH_H

#include <cstddef>



#define DTC_FILE_2BYTE_ID 0x4454 // DT

#define DTC_NO_ERROR 0
#define DTC_INTERNAL_ERROR 1
#define DTC_CONFIGURATION_ERROR 2
#define DTC_TRAP_1 3
#define DTC_TRAP_2 4
#define DTC_TRAP_3 5
#define DTC_TRAP_4 6
#define DTC_TRAP_5 7
#define DTC_INTERNAL_UNUSED_1 8
#define DTC_STORAGE_MISMATCH 9
#define DTC_TPS_UNCONNECTED 10
#define DTC_TPS2_UNCONNECTED 11
#define DTC_MAP_UNCONNECTED 12
#define DTC_QUANTITY_ADJUSTER_UNCONNECTED 13
#define DTC_NEEDLESENSOR_UNPLAUSIBLE_SIGNAL 14
#define DTC_RPM_UNPLAUSIBLE_SIGNAL 15
#define DTC_ENGINE_TEMP_UNCONNECTED 16
#define DTC_FUEL_TEMP_UNCONNECTED 17
#define DTC_AIR_TEMP_UNCONNECTED 18
#define DTC_TPS_UNPLAUSIBLE 19

#define DTC_CONF_MISMATCH 20
#define DTC_CONF_PARTIAL_MISMATCH 21


#define MAX_DTCS 64
#define MAX_DTC_LEN 60

//const char DTC_CODES[] PROGMEM = "z";



class DTC {
private:
	static const char *DTC_CODES[];
    long lastSave;
    int iterator;
    bool isTouched;
    
    unsigned char errorCodesOnStartup[MAX_DTCS];    
    unsigned char errorCodesCurrent[MAX_DTCS];
    void saveToFlash();
    void load();
public:
    DTC();   
    bool seekNextError();
    const char *getName();
    unsigned char getCount();
    int getIndex();
    void setError(unsigned int dtc);
    void resetAll();
    void save();
    bool isErrorActive(unsigned int dtc);
    
};

extern DTC dtc;

#endif


