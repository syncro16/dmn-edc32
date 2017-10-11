#include "ConfEditor.h"
#include "utils.h"
#include "Core.h"
#include "DTC.h"
#include "Serial.h"


#include <stdlib.h>
#include <string.h>

ConfEditor confeditor;

const char *confEditorHeader[]  = {
	PRODUCT_NAME,"Status:",
	" ",
	" ",0};

const char *confEditorMainScreen[]  = {
	"Global shortcuts:",
	"  <0> Main menu",
	"  <1> DTC memory",
	"  <2> Configuration/Adaptation",
	"  <3> Map Editor",
	"  <4> Output tests",
	"  <5> Visualizer",	
	"  <.> Toggle status indicator (Status/RPM/TPS/Map)",    
	" ",
	"Send feedback to syncro16@outlook.com or visit http://dmn.kuulalaakeri.org/",
	0};

const char *confEditorDTCText[]  = {"Keys: r - reset fault memory, g - generate error","DTC Name","Count",0};

const char *confEditorAdaptationText[]  = {
	"Keys: -/+ adjust (;/: *32) <spc> raw/human </> page <crsr keys> move s-save",
	// 01234567890123456789012345678901234567890123456789012345678901234567890123456789
	"Item description (Page x/x)                      SetPoint   Input     Actual",
	"------------------------------------------------ ---------- --------- -------",    
	0};

const char confEditorMapCurrentOutput[]  = "Output: ";
const char confEditorMapEditorHelp[]  = "Keys: -/+ adjust (;/: *32), </> change c-copy v-paste s-save <crsr keys> move";
const char confEditorMapCurrentMap[]  = "Current map:";

ConfEditor::ConfEditor() {
	liveMode = false;
	keyPressed = -1;
	mapIdx = 0;
	page = 0;
	statusPrinted = false;
	uiEnabled = false;
	setSystemStatusMessage("");  
}

void ConfEditor::printHeader() {
	const char *row;
	char *status;
	char buf[16];
	
	// refrash  
	serial.ansiGotoXy(1,1);
	serial.ansiClearEol();
	
	serial.print(confEditorMainScreen[1+page]+6);
	
	serial.print(" (");
	serial.print(confEditorHeader[0]);
	serial.print(")");

	row = confEditorHeader[1];

	switch (statusIndex % 4) {
		default:
			status = (char*)&systemStatusMessage;
			break;
		case 1:
			itoa(core.controls[Core::valueEngineRPM],buf,10);
			memcpy(buf+strlen(buf)," RPM",5);
			status = buf;
			break;
		case 2:
			itoa(core.controls[Core::valueTPSActual],buf,10);
			memcpy(buf+strlen(buf)," TPS",5);
			status = buf;            
			break;
		case 3:
			itoa(core.controls[Core::valueBoostPressure],buf,10);
			memcpy(buf+strlen(buf)," kPa",5);
			status = buf;            
			break;            
	}
	serial.ansiGotoXy(80-strlen(row)-strlen(status),1);        
	serial.print(row);
	serial.print(status);
}

void ConfEditor::mainScreen() {
	if (!keyPressed)
		return;
	
	const char *row;
	unsigned char mapIdx=0;
	while ((row = confEditorMainScreen[mapIdx])) {
		serial.ansiGotoXy(1,mapIdx+3);
		serial.print(row);
		mapIdx++;
	}
}

const char confEditorOutputTestsGlow[80]  =   {"  <Q>          Glow Plug status:"};
const char confEditorOutputTestsFan[80]  =    {"  <A>                Fan status:"};
const char confEditorOutputPumpAdvance[80]  = {"  t/T   Pump advance duty cycle:"};
const char confEditorOutputN75[80]  =         {"  n/N            N75 duty cycle:"};
const char confEditorOutputLabelOn[80]  = "ON";
const char confEditorOutputLabelOff[80]  = "OFF";

void ConfEditor::pageOutputTests() {
	bool redrawView = false;

	core.controls[Core::valueOutputTestMode] = true;

	switch (keyPressed) {
		case 0:
			break;
		case 'Q':
		case 'q':
			core.controls[Core::valueOutputGlow] = !core.controls[Core::valueOutputGlow];
			redrawView = true;
			break;
		case 'A':
		case 'a':
			core.controls[Core::valueOutputFan] = !core.controls[Core::valueOutputFan];
			redrawView = true;
			break;
		case 't':
			core.controls[Core::valueEngineTimingDutyCycle] -= 16;
			if (core.controls[Core::valueEngineTimingDutyCycle]<0)
					core.controls[Core::valueEngineTimingDutyCycle] = 0;
			redrawView = true;				
			break;
		case 'T':
			core.controls[Core::valueEngineTimingDutyCycle] += 16;
			if (core.controls[Core::valueEngineTimingDutyCycle]>255)
					core.controls[Core::valueEngineTimingDutyCycle] = 255;
			redrawView = true;						
			break;
		case 'n':
			core.controls[Core::valueN75DutyCycle] -= 16;
			if (core.controls[Core::valueN75DutyCycle]<0)
					core.controls[Core::valueN75DutyCycle] = 0;
			redrawView = true;				
			break;
		case 'N':
			core.controls[Core::valueN75DutyCycle] += 16;
			if (core.controls[Core::valueN75DutyCycle]>255)
					core.controls[Core::valueN75DutyCycle] = 255;
			redrawView = true;						
			break;

		default:
			redrawView = true;
	} 
	if (redrawView) {
		serial.ansiClearScreen();
		printHeader();		
		serial.ansiGotoXy(1,5);
		serial.print(confEditorOutputTestsGlow);
		serial.ansiGotoXy(35,5);
		serial.print(core.controls[Core::valueOutputGlow]?(confEditorOutputLabelOn):(confEditorOutputLabelOff));
		serial.ansiGotoXy(1,6);
		serial.print(confEditorOutputTestsFan);
		serial.ansiGotoXy(35,6);
		serial.print(core.controls[Core::valueOutputFan]?(confEditorOutputLabelOn):(confEditorOutputLabelOff));		
		serial.ansiGotoXy(1,7);
		serial.print(confEditorOutputPumpAdvance);
		serial.ansiGotoXy(35,7);
		serial.print(core.controls[Core::valueEngineTimingDutyCycle]);	
		serial.ansiGotoXy(1,8);
		serial.print(confEditorOutputN75);
		serial.ansiGotoXy(35,8);
		serial.print(core.controls[Core::valueN75DutyCycle]);						
	}
}

//const char confEditorVisualizer[80]  =             {"A1234567890 0123456789ABCDEF0123456789ABCDEF0123456789ABCDEF0123456789ABCDEF"};

const char confEditorVisualizerTPS[]  =             {"        TPS 0% . . . . . . . . . . . . . 50% . . . . . . . . . . . . . .100%"};
const char confEditorVisualizerFuelAmount[80]  =    {" FuelAmount 0% . . . . . . . . . . . . . 50% . . . . . . . . . . . . . .100%"};
const char confEditorVisualizerQAFB[80]  = 			{"QA FeedBack 0% . . . . . . . . . . . . . 50% . . . . . . . . . . . . . .100%"};
const char confEditorVisualizerAdvance[80]  =       {"    Advance 0% . . . . . . . . . . . . . 50% . . . . . . . . . . . . . .100%"};
const char confEditorVisualizerN75[80]  =     		{"        N75 0% . . . . . . . . . . . . . 50% . . . . . . . . . . . . . .100%"};
const char confEditorVisualizerMapActual[80]  = 	{" MAP Actual 0% . . . . . . . . . . . . . 50% . . . . . . . . . . . . . .100%"};
const char confEditorVisualizerMapRequest[80]  = 	{"MAP Request 0% . . . . . . . . . . . . . 50% . . . . . . . . . . . . . .100%"};
const char confEditorVisualizerRPM[80]  = 			{"        RPM "};



void ConfEditor::pageVisualizer() {
	static char oldTps,oldFuelAmount,oldAdvance,oldN75,oldQAFB,oldMap,oldMapRequest;
	int oldRPM;
	if (keyPressed) {
		oldTps = -1;
		oldFuelAmount = -1;
		oldAdvance = -1;
		oldN75 = -1;
		oldQAFB = -1;
		oldMap = -1;
		oldRPM = -1;
		serial.ansiClearScreen();
		printHeader();

		serial.ansiGotoXy(1,4);
		serial.print(confEditorVisualizerTPS);

		serial.ansiGotoXy(1,5);
		serial.print(confEditorVisualizerFuelAmount);		

		serial.ansiGotoXy(1,6);
		serial.print(confEditorVisualizerQAFB);	

		serial.ansiGotoXy(1,7);
		serial.print(confEditorVisualizerAdvance);	

		serial.ansiGotoXy(1,8);
		serial.print(confEditorVisualizerN75);	

		serial.ansiGotoXy(1,9);
		serial.print(confEditorVisualizerMapActual);	

		serial.ansiGotoXy(1,10);
		serial.print(confEditorVisualizerMapRequest);	

		serial.ansiGotoXy(1,11);
		serial.print(confEditorVisualizerRPM);	
	}


	if (core.controls[Core::valueTPSActual]/4 != oldTps) {
		oldTps = core.controls[Core::valueTPSActual]/4;
		serial.ansiGotoXy(13,4);
		for (char i=0;i<oldTps;i++)
			serial.print("=");
		serial.print(confEditorVisualizerTPS+oldTps+12);
			
	}

	if (core.controls[Core::valueFuelAmount8bit]/4 != oldFuelAmount) {
		oldFuelAmount = core.controls[Core::valueFuelAmount8bit]/4;
		serial.ansiGotoXy(13,5);
		for (char i=0;i<oldFuelAmount;i++)
			serial.print("*");
		serial.print(confEditorVisualizerFuelAmount+oldFuelAmount+12);	
	}

	if (core.controls[Core::valueQAfeedbackRaw]/16 != oldQAFB) {
		oldQAFB = core.controls[Core::valueQAfeedbackRaw]/16;
		serial.ansiGotoXy(13,6);
		for (char i=0;i<oldQAFB;i++)
			serial.print("*");
		serial.print(confEditorVisualizerQAFB+oldQAFB+12);	
	}

	if (core.controls[Core::valueEngineTimingDutyCycle]/4 != oldAdvance) {
		oldAdvance = core.controls[Core::valueEngineTimingDutyCycle]/4;
		serial.ansiGotoXy(13,7);
		for (char i=0;i<oldAdvance;i++)
			serial.print("*");
		serial.print(confEditorVisualizerAdvance+oldAdvance+12);	
	}

	if (core.controls[Core::valueN75DutyCycle]/4 != oldN75) {
		oldN75 = core.controls[Core::valueN75DutyCycle]/4;
		serial.ansiGotoXy(13,8);
		for (char i=0;i<oldN75;i++)
			serial.print("*");
		serial.print(confEditorVisualizerN75+oldN75+12);
	}	


	if (core.controls[Core::valueBoostPressure]/4 != oldMap) {
		oldMap = core.controls[Core::valueBoostPressure]/4;
		serial.ansiGotoXy(13,9);
		for (char i=0;i<oldMap;i++)
			serial.print("*");
		serial.print(confEditorVisualizerMapActual+oldMap+12);
	}	
	if (core.controls[Core::valueBoostTarget]/4 != oldMapRequest) {
		oldMapRequest = core.controls[Core::valueBoostTarget]/4;
		serial.ansiGotoXy(13,10);
		for (char i=0;i<oldMapRequest;i++)
			serial.print("*");
		serial.print(confEditorVisualizerMapRequest+oldMapRequest+12);	
	}		
	if ((core.controls[Core::valueEngineRPM]/10)*10 != oldRPM) {
		oldRPM = (core.controls[Core::valueEngineRPM]/10)*10;
		serial.ansiGotoXy(13,11);
		serial.printWithPadding(oldRPM,5,' ');
		serial.print(confEditorVisualizerRPM+oldRPM+12);
	}
	if (keyPressed == 'P') {
		core.controls[Core::valueQADebug] = !core.controls[Core::valueQADebug];
		if (core.controls[Core::valueQADebug]) serial.printf("QA Disabled");
	}

	serial.ansiGotoXy(13,4+2*8);
	if (core.controls[Core::valueBoostActuatorClipReason] == BOOST_MIN_CLIP) {
		serial.print("min clip");
	} else 	if (core.controls[Core::valueBoostActuatorClipReason] == BOOST_MAX_CLIP) {
		serial.print("MAX clip");
	} else {
		serial.ansiClearEol();
	}


}

void ConfEditor::pageDTC() {
	switch (keyPressed) {
		case 'R':            
		case 'r':
			dtc.resetAll();
			break;
		case 'g':
			dtc.setError(DTC_TRAP_1);
			break;
	}
	if (keyPressed || tick % 32 == 0) {
		serial.ansiClearScreen();
		printHeader();

		serial.ansiGotoXy(1,3);
		serial.printf(confEditorDTCText[0]);
		
		serial.ansiGotoXy(1,5);
		serial.print(confEditorDTCText[1]);
		serial.ansiGotoXy(60,5);		
		serial.print(confEditorDTCText[2]);
		
		serial.ansiGotoXy(1,6);
		serial.printPads(70,'-');
		
		char y=7;
		while (dtc.seekNextError()) {        
			serial.ansiGotoXy(3,y);
			
			serial.print(dtc.getName());
			serial.ansiGotoXy(60,y);
			
			serial.print(dtc.getCount());
			y++;
		}

	}    
}

void ConfEditor::pageAdaptation() {
	bool redrawView = false;
	int i;
	const int rows=17;
	nodeStruct *item;
	
	int mapIdx = activeRow+rows*corePageNumber;
	char amount = 1;
	switch (keyPressed) {
		case 's':
			core.save();
			break;
		case '<':
		case KEY_LEFT:
			if (corePageNumber>0) {
				corePageNumber--;
				activeRow = 0;
			}
			redrawView = true;
			break;
		case '>':
		case KEY_RIGHT:
			if ((corePageNumber+1)*rows<Core::NODE_MAX) {
				corePageNumber++;
				activeRow = 0;
			}
			redrawView = true;
			break;;
		case KEY_UP:
			if (activeRow>0)
				activeRow--;
			break;
		case KEY_DOWN:
			if (activeRow<(rows-1) && mapIdx<Core::NODE_MAX)
				activeRow++;
			break;
		case ':':
			amount = 32;
		case '+':
			core.setCurrentNode(mapIdx);
			for (char i=0;i<amount;i++)
				core.incValue();
			break;
		case ';':
			amount = 32;            
		case '-':
			core.setCurrentNode(mapIdx);
			for (char i=0;i<amount;i++)
				core.decValue();
			break;
		case ' ':
			toggleHumanReadableValues();
			break;
		case 0:
			break;
			
		default:
			redrawView = true;
			break;

	}
	
	if (redrawView) {
		// refresh entire screen
		serial.ansiClearScreen();
		printHeader();
		
		serial.ansiGotoXy(1,3);
		
		serial.print(confEditorAdaptationText[0]);
		
		serial.ansiGotoXy(1,5);
		serial.print(confEditorAdaptationText[1]);
		
		serial.ansiGotoXy(1,6);
		serial.printf(confEditorAdaptationText[2]);
		
		serial.ansiGotoXy(24,5);
		serial.print(corePageNumber+1);
		serial.ansiGotoXy(26,5);
		serial.print((Core::NODE_MAX+rows-1)/rows);
		
		for (i=0;i<rows;i++) {
			int mapIdx = i+rows*corePageNumber;
			if (mapIdx<=Core::NODE_MAX) {
				core.setCurrentNode(mapIdx);
				item = core.getNodeData();

                if (item->properties) {   
					serial.ansiGotoXy(3,7+i);

					serial.print(item->description);
					if ((item->properties & NODE_PROPERTY_LOCKED) == NODE_PROPERTY_LOCKED) {
						serial.print(" (view only)");
					} 
				}
			}
		}

	}
	
	// current conf/sensor values
	if (redrawView || keyPressed || tick % 16 == 0) {
		for (i=0;i<rows;i++) {
			int mapIdx = i+rows*corePageNumber;        
			if (mapIdx<=Core::NODE_MAX) {
				core.setCurrentNode(mapIdx);            
				item = core.getNodeData();
				if (activeRow == i)
					serial.ansiSetReverse(true);
			
				serial.ansiGotoXy(50,7+i);                
				if (item->properties) {   					
					printValue(item->value,item->type);
				} else {
					serial.print("        ");
				}

				if (activeRow == i)
					serial.ansiSetReverse(false);

				// TODO active line


				if (item->rawValueKey != Core::valueNone) {
					serial.ansiGotoXy(60,7+i);
					//serial.print(controls[item->rawValueKey]);
					printValue(core.controls[item->rawValueKey],item->type);
				}
				
				if (item->actualValueKey != Core::valueNone) {
					serial.ansiGotoXy(70,7+i);
					//serial.print(controls[item->actualValueKey]);
					printValue(core.controls[item->actualValueKey],VALUE_PERCENTAGE);
				}
					
			}        
		}
	}	
}

void ConfEditor::pageMapEditor() {    
	bool updateCell = false;
	bool updateCursor = false;
	bool redrawView = false;
	// unsigned char *mapData = editorMaps[mapIdx];
	//unsigned char *mapData = core.boostMap;
	unsigned char *mapData = core.maps[mapIdx]+2; // 2 skip file_id offset

	unsigned char tableSizeX = *(mapData+3);
	unsigned char tableSizeY = *(mapData+4);
	unsigned char axisTypeX = *(mapData+5);
	unsigned char axisTypeY = *(mapData+6);
	unsigned char axisTypeResult = *(mapData+7);
	unsigned char lastXpos = *(mapData+8+tableSizeX*tableSizeY);
	unsigned char lastYpos = *(mapData+8+tableSizeX*tableSizeY+1);
	int lastValue = *(mapData+8+tableSizeX*tableSizeY+2);
//	unsigned int lastValue10b = *(unsigned int*)(mapData+8+tableSizeX*tableSizeY+3);
	
	const char xPad = 5;
	char xSpace ;

	if (tableSizeX>8) {
		xSpace = 5;
	} else {
		xSpace = 7;
	}


	const char yPad = 7;
	const char ySpace = 2;
	
	switch (keyPressed) {
		case 'c':
			mapEditorData.clipboard = *(mapData+8+mapEditorData.cursorX+mapEditorData.cursorY*tableSizeX);
			break;
		case 'v':
			(*(mapData+8+mapEditorData.cursorX+mapEditorData.cursorY*tableSizeX)) = mapEditorData.clipboard;
			updateCell = true;
			break;
		case 'h':
			mapEditorData.cursorXold = mapEditorData.cursorX;		
			mapEditorData.cursorYold = mapEditorData.cursorY;		
			
			if (mapEditorData.cursorX>0)
				mapEditorData.cursorX--;
			updateCursor = true;
			break;
		case 'l':
			mapEditorData.cursorXold = mapEditorData.cursorX;		
			mapEditorData.cursorYold = mapEditorData.cursorY;		
			
			if (mapEditorData.cursorX<tableSizeX-1)
				mapEditorData.cursorX++;
			updateCursor = true;
			break;
		case 'k':
			mapEditorData.cursorXold = mapEditorData.cursorX;				
			mapEditorData.cursorYold = mapEditorData.cursorY;				
			if (mapEditorData.cursorY>0)
				mapEditorData.cursorY--;
			updateCursor = true;
			break;
		case 'j':
			mapEditorData.cursorXold = mapEditorData.cursorX;				
			mapEditorData.cursorYold = mapEditorData.cursorY;		
			if (mapEditorData.cursorY<tableSizeY-1)
				mapEditorData.cursorY++;
			updateCursor = true;
			break;
		case ':':
			if (*(mapData+8+mapEditorData.cursorX+mapEditorData.cursorY*tableSizeX)<(0xff-32))
				(*(mapData+8+mapEditorData.cursorX+mapEditorData.cursorY*tableSizeX))+=32;
			updateCell = true;            
			break;			
		case '+':
			if (*(mapData+8+mapEditorData.cursorX+mapEditorData.cursorY*tableSizeX)<0xff)
				(*(mapData+8+mapEditorData.cursorX+mapEditorData.cursorY*tableSizeX))++;
			updateCell = true;            
			break;
		case '-':
			if (*(mapData+8+mapEditorData.cursorX+mapEditorData.cursorY*tableSizeX)>0)
				(*(mapData+8+mapEditorData.cursorX+mapEditorData.cursorY*tableSizeX))--;
			updateCell = true;            
			break;			
		case ';':
			if (*(mapData+8+mapEditorData.cursorX+mapEditorData.cursorY*tableSizeX)>32)
				(*(mapData+8+mapEditorData.cursorX+mapEditorData.cursorY*tableSizeX))-=32;
			updateCell = true;            
			break;
		case 's':
			core.save();
			break;
		case 0:
			// nop
			break;
		case '<':
			if (mapIdx > 0 ) mapIdx--;
			keyPressed = 255; // force redraw
			pageMapEditor();
			return;
		case '>':
			if (mapIdx<core.numberOfMaps-1)
				mapIdx++;
			keyPressed = 255; // force redraw
			pageMapEditor();
			return;
		default:
			redrawView = true;
			updateCursor = true;
			if (mapEditorData.currentMap!=mapIdx) {
				mapEditorData.cursorX=0;
				mapEditorData.cursorY=0;
				mapEditorData.cursorXold=0;
				mapEditorData.cursorYold=0;				
				mapEditorData.currentMap = mapIdx;
			}
	}
	if (redrawView) {
		serial.ansiClearScreen();
		printHeader();
		serial.ansiGotoXy(0,3);
		serial.print(confEditorMapEditorHelp);
		
		serial.ansiGotoXy(0,5);
		serial.print(confEditorMapCurrentMap);
		serial.print(core.mapNames[mapIdx]);
		
		// Table X header
		
		for (int x=0;x<tableSizeX;x++) {
			serial.ansiGotoXy(xPad+(1+x)*xSpace,yPad);
			int mapIdx = round((float)((255/(float)(tableSizeX-1)))*(float)x);
			serial.printPads(1,' ');
			printMapAxis(axisTypeX,mapIdx, ((x==0||x==(tableSizeX-1))?true:false));
		}
		serial.ansiGotoXy(xPad+xSpace,yPad+1);
		serial.printPads(tableSizeX*xSpace,'-');
		
		// Table Y header
		
		for (int y=0;y<tableSizeY;y++) {
			serial.ansiGotoXy(xPad-1,yPad+(1+y)*ySpace);
			int mapIdx = round((float)((255/(float)(tableSizeY-1)))*(float)y);
			
			printMapAxis(axisTypeY,mapIdx,true);
			serial.ansiGotoXy(xPad+xSpace-1,yPad+(1+y)*ySpace);
			serial.print("|");
			if (y<tableSizeY-1) {
				serial.ansiGotoXy(xPad+xSpace-1,yPad+(1+y)*ySpace+1); // works for ySpace=2
				serial.print("|");
			}
			
		}
		for (int y=0;y<tableSizeY;y++) {
			for (int x=0;x<tableSizeX;x++) {
				serial.ansiGotoXy(xPad+(1+x)*xSpace,yPad+(1+y)*ySpace);
				serial.printPads(1,' ');
				//serial.print(*(mapData+8+x*y),DEC);
				printMapAxis(axisTypeResult,*(mapData+8+x+y*tableSizeX),0);
			}
		}
	}

	if (updateCursor || updateCell) {
		// redraw cell (inactive)
		serial.ansiSetReverse(false);	
		serial.ansiGotoXy(xPad+xSpace+mapEditorData.cursorXold*xSpace,yPad+ySpace+mapEditorData.cursorYold*ySpace);
		serial.printPads(xSpace-2,' ');
		serial.ansiGotoXy(xPad+xSpace+mapEditorData.cursorXold*xSpace+1,yPad+ySpace+mapEditorData.cursorYold*ySpace);
		printMapAxis(axisTypeResult,*(mapData+8+mapEditorData.cursorXold+mapEditorData.cursorYold*tableSizeX),0);

		// redraw cell (active)
		serial.ansiGotoXy(xPad+xSpace+mapEditorData.cursorX*xSpace,yPad+ySpace+mapEditorData.cursorY*ySpace);
		serial.ansiSetReverse(true);
		serial.printPads(xSpace-2,' ');
		serial.ansiGotoXy(xPad+xSpace+mapEditorData.cursorX*xSpace+1,yPad+ySpace+mapEditorData.cursorY*ySpace);
		printMapAxis(axisTypeResult,*(mapData+8+mapEditorData.cursorX+mapEditorData.cursorY*tableSizeX),0);
		serial.ansiSetReverse(false);
	}
	

	// table live view (last queried X, Y and returned interpolated value (8bit or 10bit interpolated value)
	if (tick % 4 == 0) {
		serial.ansiGotoXy(2,yPad+ySpace+round((float)mapEditorData.lastY*(float)((float)(tableSizeY-1)*(float)ySpace/255)));
		serial.print("  ");
		serial.ansiGotoXy(xPad+xSpace+round((float)mapEditorData.lastX*(float)((float)tableSizeX*(float)xSpace/255)),yPad-1);
		serial.print(" ");
		
		mapEditorData.lastY = lastYpos;
		mapEditorData.lastX = lastXpos;
		
		serial.ansiGotoXy(2,yPad+ySpace+round((float)lastYpos*(float)((float)(tableSizeY-1)*(float)ySpace/255)));
		serial.print(">>");
		serial.ansiGotoXy(xPad+xSpace+round((float)lastXpos*(float)((float)tableSizeX*(float)xSpace/255)),yPad-1);
		serial.print("v"); 

		serial.ansiGotoXy(xPad+xSpace,yPad+tableSizeY*ySpace+2);
		serial.printf(confEditorMapCurrentOutput);
		printMapAxis(axisTypeResult,lastValue,1);
//		serial.print(" / ");
//		serial.printf("%d",lastValue);
		serial.ansiClearEol();
		
	}

}

void ConfEditor::setSystemStatusMessage(const char *msg) {
	statusPrinted = false;
	if (strlen(msg) >= sizeof(systemStatusMessage)) {
		memcpy(systemStatusMessage,msg,sizeof(systemStatusMessage)-1);
		systemStatusMessage[sizeof(systemStatusMessage)] = 0;
	} else {
		strcpy(systemStatusMessage,msg);
	}
}

void ConfEditor::toggleStatus() {
	statusIndex++;
	statusPrinted=false;
}

void ConfEditor::handleInput(char node) {
	keyPressed = node;
	if (!uiEnabled) {
		serial.ansiClearScreen();
		uiEnabled = true;
	}
	if (node == '.')
		toggleStatus();
	if (node>='0' && node<='9') {
		page = node-'0';
		serial.ansiClearScreen();
		tick = 0;
		statusPrinted = false;
	}
}

void ConfEditor::refresh() {
	if (!uiEnabled)
		return;
	
	if (page>5)
		page = 0;
	
	if (!statusPrinted/* || statusIndex != 0*/) {
		printHeader();
		statusPrinted = true;
	}
	
	switch (page) {
		case 0:
			core.controls[Core::valueOutputTestMode] = false;
			mainScreen();
			break;
		case 1:
			core.controls[Core::valueOutputTestMode] = false;		
			pageDTC();
			break;
		case 2:
			core.controls[Core::valueOutputTestMode] = false;		
			pageAdaptation();
			break;
		case 3:
			core.controls[Core::valueOutputTestMode] = false;		
			pageMapEditor();
			break;
		case 4:
			core.controls[Core::valueOutputTestMode] = true;		
			pageOutputTests();
			break;
		case 5:
			core.controls[Core::valueOutputTestMode] = false;		
			pageVisualizer();
			break;						
	}
	keyPressed = 0;
	tick++;
}

