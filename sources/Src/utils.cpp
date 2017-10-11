#include "utils.h"

#include "Core.h"
#include "defines.h"
#include "Serial.h"
#include <string.h>
#include <math.h>
#include <stdlib.h>


// This is the original map() function in Arduino Core
float map(float x, float in_min, float in_max, float out_min, float out_max)
{
  return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

bool isHumanReadable = true;

void toggleHumanReadableValues() {
    isHumanReadable=!isHumanReadable;
}

void printValue(int i,char type) {
    char buf[10];
    unsigned int fixed;    
    if (!isHumanReadable) {
        type = VALUE_INT;
    }
    // always prints 6 chars
    switch (type) {
/*        case VALUE_BAR:
            break;
        case VALUE_PERCENTAGE:
            break;*/
        case VALUE_PWM8:
            serial.printWithPadding(map(i,0,255,0,100),5,' ');
            serial.print("%  ");            
            break;
        case VALUE_MS:
            serial.printWithPadding(i,5,' ');
            serial.print("ms ");        
            break;            
        case VALUE_KPA:
            serial.printWithPadding(toKpa(i),5,' ');
            serial.print("kPa");        
            break;

        case VALUE_CELSIUS:                   
            serial.printWithPadding(i-64,5,' ');
            serial.print("C  ");                            
            break;

/*        case VALUE_DEGREE:
        break;*/
        case VALUE_INJECTION_TIMING:
//            ltoa((BTDC_MARK+i),buf,10);
            itoa((i),buf,10);
            for (unsigned char c=0;c<5-strlen(buf)-1;c++)
                serial.print(" ");
            for (unsigned char c=0;c<strlen(buf);c++) {
                if (c == strlen(buf)-1) 
                    serial.print(".");
                serial.print(buf[c]);           
            }
            serial.print("°  ");                                        
            break;            
        case VALUE_VOLTAGE:
			fixed = ((unsigned int)i*48);   // ~4.97v
			itoa(fixed,buf,10);
//			serial.printf("_%c",buf[0]);
            if (strlen(buf) == 5) {
				serial.printf(" %c.%c%cv  ",buf[0],buf[1],buf[2]);
				/*
                serial.print(buf[0]);
                serial.print(".");
                serial.print(buf[1]);
                serial.print(buf[2]);*/
            } else if (strlen(buf) == 4) {
				serial.printf(" 0.%c%cv  ",buf[0],buf[1]);
				/*
                serial.print("0.");
                serial.print(buf[0]);
                serial.print(buf[1]);*/
            }  else if (strlen(buf) == 3) {
					serial.printf(" 0.0%cv  ",buf[0]);
				
  //              serial.print("0.0");
  //              serial.print(buf[0]);
            } else {
	               serial.print(" 0.00v  ");
			}            
//            serial.print("v");                
            break;
        case VALUE_BATTERY_VOLTAGE:
            fixed = (float)(1.93*(float)i); // r1= 3000, r2 = 10000
            itoa(fixed,buf,10);
            if (strlen(buf) == 4) {
                serial.print(buf[0]);
                serial.print(buf[1]);
                serial.print(".");
                serial.print(buf[2]);
                serial.print(buf[3]);
            } else
            if (strlen(buf) == 3) {
                serial.print(" ");
                serial.print(buf[0]);
                serial.print(".");
                serial.print(buf[1]);
                serial.print(buf[2]);
            } else if (strlen(buf) == 2) {
                serial.print(" 0.0");
                serial.print(buf[0]);
            } else {
                serial.print(" 0.00");
            }            
            serial.print("v  ");                
            break;            
/*
        case VALUE_FIXED_POINT_2:
            // TODO         
            break;*/
        case VALUE_BOOLEAN:
            if (i==0) {
                serial.print("  Off   ");
            } else {
                serial.print("   On   ");                
            }
            break;
        case VALUE_HEX:
            itoa(i,buf,16);
            serial.printWithPadding(buf,5,' ');            
            serial.print("h");
            break;
        case VALUE_INT:
        default:
            serial.printWithPadding(i,5,' ');
            serial.print("   ");            
    }
}

unsigned char mapValues(int raw,int mapMin,int mapMax) {
    if (raw < mapMin)
        return 0;
    if (raw >= mapMax)
        return 0xff;
    
    return map(raw,mapMin,mapMax,0,255);
}

unsigned char mapValuesSqueeze(int raw,int mapMin,int mapMax) {
    return map(raw,0,255,mapMin,mapMax);
}

// Interpolate value between p1 and p1, given by pos 0.0 - 1.0)
float mapInterpolate(float p1,float p2, float amount) {
	if (amount<0)
		amount=0;
	if (amount>1)
		amount=1;
    return (p1*(1-amount)+p2*amount);
}

float mapLookUp(unsigned char *mapData,unsigned char x,unsigned char y) {
    unsigned char isInterpolated = *(mapData+2+2);
    unsigned char tableSizeX = *(mapData+3+2);
    unsigned char tableSizeY = *(mapData+4+2);
	unsigned char yPos;
	float amountX=0;
	float amountY=0;
    int ofs = 10; // skip headers

    *(mapData+ofs+tableSizeX*tableSizeY) = x;
    *(mapData+ofs+tableSizeX*tableSizeY+1) = y;
    
    if (tableSizeY) {
        yPos = y / (256/(tableSizeY-1));
    } 
    else {
        yPos = 0;
    }
    unsigned char xPos = (x / (256/(tableSizeX-1)));
    
    unsigned char p1 = *(mapData+ofs+(yPos*tableSizeX)+xPos);
    unsigned char p2 = *(mapData+ofs+(yPos*tableSizeX)+(((xPos+1)>=tableSizeX)?xPos:xPos+1));
    unsigned char p3 = *(mapData+ofs+((((yPos+1)>=tableSizeX)?yPos:yPos+1)*tableSizeX)+xPos);
    unsigned char p4 = *(mapData+ofs+((((yPos+1)>=tableSizeX)?yPos:yPos+1)*tableSizeX)+(((xPos+1)>=tableSizeX)?xPos:xPos+1));
    
    float ret;
    if (isInterpolated == 'D') {
//		float amountX = (x % (256/(tableSizeX-1)))*(256/(tableSizeX-1));
		float cellWidthX=(256/(tableSizeX-1));
		float cellWidthY=(256/(tableSizeY-1));
		
		// TODO tsekkaa tää, kusee
		amountX = (1.0/cellWidthX)*fmodf(x,cellWidthX);
//		amountX = (int)x%(int)cellWidth;;
		
//		amountX = ((float)1/(float)(256/(tableSizeX-1)))*(float)(x%(256/(tableSizeX-1)));
		
        if (tableSizeY) {
            // 2D
//			float amountY = (y % (256/(tableSizeY-1)))*(256/(tableSizeY-1));
//			amountY = ((float)1/(float)(256/(tableSizeY-1)))*(float)(y%(256/(tableSizeY-1)));
			amountY = (1.0/cellWidthY)*fmodf(y,cellWidthY);

			int y1 = mapInterpolate(p1,p2,amountX); // TODO 		
			
            int y2 = mapInterpolate(p3,p4,amountX);
			ret = mapInterpolate(y1,y2,amountY);		
        } 
        else {
            // 1D
            ret = mapInterpolate(p1>>4,p2>>4,amountX);
        }
    } 
    else {
        ret = p1 << 4;
	}
    *(int*)(mapData+ofs+tableSizeX*tableSizeY+2) = ret;
    return ret;
}

void printMapAxis(unsigned char axisType,unsigned char value,bool verbose) {
    switch (axisType) {
        case MAP_AXIS_NONE:
            break;
        case MAP_AXIS_RPM:
            serial.print(toRpm(value));
            if (verbose) serial.print(" Rpm");
            break;
        case MAP_AXIS_IDLERPM:
            serial.print(value*4);
            if (verbose) serial.print(" Rpm");
            break;            
        case MAP_AXIS_TPS:
            serial.print(toTps(value));
            if (verbose) serial.print("% TPS");
            break;
        case MAP_AXIS_KPA:
            serial.print(toKpa(value));
            if (verbose) serial.print(" kPa");
            break;
        case MAP_AXIS_VOLTAGE:
            //serial.print(toVoltage(value),g);
            //if (verbose) serial.print(" mV");
            printValue(value*4,VALUE_VOLTAGE);
            break;
        case MAP_AXIS_CELSIUS:
            serial.print(value-64);
            if (verbose) serial.print(" °C");
            break;
        case MAP_AXIS_INJECTION_TIMING:
            //serial.print(toVoltage(value),DEC);
            //if (verbose) serial.print(" mV");
            printValue(value*4,VALUE_INJECTION_TIMING);
            break;   
         case MAP_AXIS_INJECTED_FUEL:
            serial.print(value);
            if (verbose) serial.print(" IQ");
            break;
         case MAP_AXIS_FUEL_TRIM_AMOUNT:
            serial.print((int)value-128);
            if (verbose) serial.print(" Ftrim");
            break;                         
        default:
            serial.print(value);
            if (verbose) serial.print(" Raw");
    }
}

int toKpa(int raw) {
    unsigned long f = (((unsigned long)core.node[Core::nodeMAPkPa].value*256ul)*(unsigned long)raw)/65280ul;// 65536ul;
    return f;
    //return raw*1.18; // defaults for 3bar map
}

int toTemperature( int rawValue) {
   // int ret = mapLookUp(tempMap,rawValue,0);
    //return ret-64;
    return -1;
}

int toVoltage(int raw) {
    // mVolt
    return int(raw*19.541); // ~4.98v
}

int toRpm(int raw) {
    unsigned long f = (((unsigned long)core.node[Core::nodeControlMapScaleRPM].value*256ul)*(unsigned long)raw)/65280ul;//65536ul;
    return f;  
//    return round(((float)core.node[Core::nodeRPMMax].value/255.0)*(float)raw);
}

int toTps(int raw) {
    // percent
    return int(raw/2.55);
}

/*
unsigned char simulatePressureActuator(int maxPressure,int currentPressure) {
    
    unsigned int i,ret;
    
   
    if (maxPressure == 0)
        return 255;
    
    // slighty logaritmic movement based on map
    long int idx = (((long int)currentPressure*100/(long int)maxPressure)*256)/100;
    if (idx<0) {
        i = 0;
    } else if (idx>255) {
        i = 255;

    } else {
       i = mapLookUp(boostMovementRemap,idx,0); 
    }  
    
     return i;

}*/


