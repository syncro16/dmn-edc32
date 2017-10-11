#include "DTC.h"

//#undef EEPROM_h
//#include <EEPROM.h>
#include "utils.h"
#include "ConfEditor.h"
#include "stm32f3xx_hal.h"
#include "Serial.h"

#include <string.h>

DTC dtc;

const char *DTC::DTC_CODES[] = { 
	"No error.", //0
	"Internal error.", //1   
	"Default settings loaded.", //2
	"Diagnostic trap #1", //3
	"Diagnostic trap #2", //4
	"Diagnostic trap #3", //5
	"Diagnostic trap #4", //6
	"Diagnostic trap #5", //7
	"Unknown DTC code 8", // 8
	"DTC Storage mismatch", // 9
	"TPS#1 unconnected", // 10
	"TPS#2 unconnected", // 11
	"MAP unconnected", // 12 
	"Quantity Adjuster unconnected", // 13
	"Unplausible needle lift sensor signal", // 14
	"Unplausible RPM sensor signal", // 15
	"Engine temperature sensor unconnected", // 16
	"Fuel temperature sensor unconnected", // 17
	"Air temperature sensor unconnected", // 18
	"TPS signal unplausible", // 19
	"Conf. mismatch", // 20
	"Conf. partial mismatch", // 21
	"Unknown DTC Code 22", // 22
	"Unknown DTC Code 23", // 23
	"Unknown DTC Code 24", // 24
	"Unknown DTC Code 25", // 25
	"Unknown DTC Code 26", // 26
	"Unknown DTC Code 27", // 27
	"Unknown DTC Code 28", // 28
	"Unknown DTC Code 29", // 29
	"Unknown DTC Code 30", // 30
	"Unknown DTC Code 31", // 31
	"Unknown DTC Code 32", // 32
	"Unknown DTC Code 33", // 33
	"Unknown DTC Code 34", // 34
	"Unknown DTC Code 35", // 35
	"Unknown DTC Code 36", // 36
	"Unknown DTC Code 37", // 37
	"Unknown DTC Code 38", // 38
	"Unknown DTC Code 39", // 39
	"Unknown DTC Code 40", // 40
	"Unknown DTC Code 41", // 41
	"Unknown DTC Code 42", // 42
	"Unknown DTC Code 43", // 43
	"Unknown DTC Code 44", // 44
	"Unknown DTC Code 45", // 45
	"Unknown DTC Code 46", // 46
	"Unknown DTC Code 47", // 47
	"Unknown DTC Code 48", // 48
	"Unknown DTC Code 49", // 49
	"Unknown DTC Code 50", // 50
	"Unknown DTC Code 51", // 51
	"Unknown DTC Code 52", // 52
	"Unknown DTC Code 53", // 53
	"Unknown DTC Code 54", // 54
	"Unknown DTC Code 55", // 55
	"Unknown DTC Code 56", // 56
	"Unknown DTC Code 57", // 57
	"Unknown DTC Code 58", // 58
	"Unknown DTC Code 59", // 59
	"Unknown DTC Code 60", // 60
	"Unknown DTC Code 61", // 61
	"Unknown DTC Code 62", // 62
	"Unknown DTC Code 63", // 63
};	

// private
DTC::DTC() {
    load();
    iterator = -1;
}
void DTC::save() {
    if (isTouched && HAL_GetTick() - lastSave >5000) {
        lastSave = HAL_GetTick();
        saveToFlash();
    }
}
void DTC::saveToFlash() {
	uint32_t errLocation;
	
	FLASH_EraseInitTypeDef eraseType;
	uint32_t address = FLASH_OFFSET_DTC;//0x8000000 + 1024*60;
	
	eraseType.TypeErase = FLASH_TYPEERASE_PAGES;
	eraseType.PageAddress = FLASH_OFFSET_DTC;
	eraseType.NbPages = 1; // 2kB

	HAL_FLASH_Unlock();

	HAL_FLASHEx_Erase(&eraseType,&errLocation);	
	HAL_FLASH_Program(FLASH_TYPEPROGRAM_HALFWORD, address, DTC_FILE_2BYTE_ID);
	address += 2;
	
	uint16_t *data = (uint16_t*) &errorCodesCurrent;
	
	for (unsigned int ofs=0;ofs<sizeof(errorCodesCurrent)/2;ofs++) {
		HAL_FLASH_Program(FLASH_TYPEPROGRAM_HALFWORD, address+ofs*2, data[ofs]);

	}
	HAL_FLASH_Lock();
	isTouched = false;
	
}
void DTC::load() {
	uint32_t address = FLASH_OFFSET_DTC;	
	uint16_t *buf = (uint16_t*)address;
	
	if (buf[0] != DTC_FILE_2BYTE_ID) {
		resetAll();
		setError(DTC_STORAGE_MISMATCH);
		return;
	}
	memcpy((char*)&errorCodesOnStartup,(char*)(FLASH_OFFSET_DTC+2),sizeof(errorCodesCurrent));
	memcpy((char*)&errorCodesCurrent,(char*)(FLASH_OFFSET_DTC+2),sizeof(errorCodesCurrent));
}
// public:

bool DTC::seekNextError() {
    while (iterator < MAX_DTCS-1) {
        iterator++;
        if (errorCodesCurrent[iterator]>0)
            return true;
    }
    iterator = -1;
    return false;
}
const char* DTC::getName() {
    if (iterator != -1) {
		return DTC_CODES[iterator];
    } else {
		return "";
	}
}
int DTC::getIndex() {
    return iterator;
}
unsigned char DTC::getCount() {
    if (iterator != -1)
        return errorCodesCurrent[iterator];
    return 0;
}

void DTC::setError(unsigned int dtc) {
    if (errorCodesOnStartup[dtc] < 255 
        && errorCodesCurrent[dtc] == errorCodesOnStartup[dtc]) {
        isTouched = true;
        errorCodesCurrent[dtc]++;
        confeditor.setSystemStatusMessage(DTC_CODES[dtc]);
    }
}

void DTC::resetAll() {
    memset(errorCodesCurrent,0,sizeof(errorCodesOnStartup));
    memset(errorCodesOnStartup,0,sizeof(errorCodesOnStartup));
    isTouched = true;
    saveToFlash();
    confeditor.setSystemStatusMessage("DTC Reset");
}

bool DTC::isErrorActive(unsigned int dtc) {
    return errorCodesCurrent[dtc]?true:false;;
}

