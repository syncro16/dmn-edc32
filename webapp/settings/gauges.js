var gaugeDefaults = {
	borders:false,
	colorBorderShadow:"none",
	needleShadow:false,
	valueTextShadow:false,
	valueBoxStroke:0,
	colorValueBoxShadow:"none",
	borders:false,
	colorNeedleShadowUp:"red",
	colorNeedle:"rgb(205, 0, 0)",
	colorNeedleStart:"rgb(205, 0, 0	)",			
	colorNeedleEnd:"rgb(205, 0, 0	)",		
	fontUnitsSize:32,
	needleCircleOuter:true,
    needleCircleInner:true,
	strokeTicks: true
};

var gaugeTypes = {
	"RPM":{
		"bindEdcValue":0x4242,
		"size":"large",		
		"priority":1,
		"init":	{
			units: "RPM",
			minValue: 0,
			maxValue: 6000,	
			majorTicks: [
				"0",
				"1000",
				"2000",
				"3000",
				"4000",
				"5000",
				"6000"
			],
			minorTicks: 4,
			highlights: [
				{
					"from": 5250,
					"to": 6000,
					"color": "rgba(200, 50, 50, .75)"
				}
			]
		}
	},
	"Boost":{
		"bindEdcValue":0x4243,
		"size":"large",		
		"priority":1,		
		"init":	{
			units: "Boost (bar)",
			minValue: 0,
			maxValue: 3,

			majorTicks: [
				"0",
				"0.5",
				"1.0",
				"1.5",
				"2.0",
				"2.5",
				"3.0"
			],
			minorTicks: 2,
			strokeTicks: true,
			highlights: []			
		}
	},
	"TPS":{
		"bindEdcValue":0x4243,
		"size":"large",		
		"priority":2,		
		"init":	{
			units: "TPS %",
			minValue: 0,
			maxValue: 100,
			majorTicks: [
				"0",
				"25",
				"50",
				"75",
				"100"
			],
			minorTicks: 2,
			highlights: []			
		}
	},
	"Injection":{
		"bindEdcValue":0x4243,
		"size":"medium",	
		"priority":1,		
		"init":	{
			units: "Injection quantity",
			minValue: 0,
			maxValue: 4096,
			majorTicks: [
				"0",
				"1024",
				"2048",
				"3072",
				"4096"
			],
			minorTicks: 4,
			highlights: [
				{
					"from": 0,
					"to": 768,
					"color": "rgba(50, 225, 50, 1)"
				},
				{
					"from":769,
					"to": 888,
					"color": "rgba(255, 200, 0, 1)"
				}								
			]			
		}
	},
	"Timing":{
		"bindEdcValue":0x4243,
		"size":"medium",
		"priority":2,		
		"init":	{
			units: "Timing °BTDC",
			minValue: -10,
			maxValue: 40,
			majorTicks: [
				"-10",
				"0",
				"10",
				"20",
				"30",
				"40"
			],
			minorTicks: 2,
			strokeTicks: true,
			highlights: [
				{
					"from": 12.5,
					"to": 13.5,
					"color": "rgba(50, 225, 50, 1)"
				},				
			]			
		}
	},	
	"Coolant":{
		"bindEdcValue":0x4243,
		"priority":15,		
		"size":"medium",		
		"init":	{
			units: "Coolant °C",
			minValue: -20,
			maxValue: 140,
			majorTicks: [
				"-20",
				"0",
				"20",
				"40",
				"60",
				"80",
				"100",
				"120",
				"140"
			],
			minorTicks: 2,
			highlights: [
				{
					"from": -20,
					"to": 20,
					"color": "rgba(0, 0, 200, .25)"
				},				
				{
					"from": 110,
					"to": 140,
					"color": "rgba(200, 50, 50, .75)"
				}
			]		
		}
	},
	"Intake":{
		"bindEdcValue":0x4243,
		"priority":15,				
		"size":"medium",		
		"init":	{
			units: "Intake °C",
			minValue: -20,
			maxValue: 140,
			majorTicks: [
				"-20",
				"0",
				"20",
				"40",
				"60",
				"80",
				"100",
				"120",
				"140"
			],
			minorTicks: 2,
			strokeTicks: true,
			highlights: [
				{
					"from": 65,
					"to": 140,
					"color": "rgba(255, 130, 30, .85)"
				}
			]			
		}
	},
	"Fuel":{
		"bindEdcValue":0x4243,
		"priority":15,				
		"size":"small",		
		"init":	{
			units: "Fuel °C",
			minValue: -20,
			maxValue: 140,
			majorTicks: [
				"-20",
				"0",
				"20",
				"40",
				"60",
				"80",
				"100",
				"120",
				"140"
			],
			minorTicks: 2,
			highlights: [
				
			]			
		}
	},
	"AuxTemp":{
		"bindEdcValue":0x4243,
		"priority":15,				
		"size":"small",		
		"init":	{
			units: "Aux °C",
			minValue: -20,
			maxValue: 140,
			majorTicks: [
				"-20",
				"0",
				"20",
				"40",
				"60",
				"80",
				"100",
				"120",
				"140"
			],
			minorTicks: 2,
			strokeTicks: true,
			highlights: [
				
			]			
		}
	},
	"AuxIn":{
		"bindEdcValue":0x4243,
		"priority":15,				
		"size":"small",		
		"init":	{
			units: "Aux In (v)",
			minValue: 0,
			maxValue: 5,
			majorTicks: [
				"0",
				"1",
				"2",
				"3",
				"4",
				"5"
			],
			minorTicks: 2,
			strokeTicks: true,
			highlights: [
				
			]			
		}
	},
	"Battery":{
		"bindEdcValue":0x4243,
		"priority":3,				
		"size":"small",		
		"init":	{
			units: "Battery (v)",
			minValue: 9,
			maxValue: 18,
			majorTicks: [
				"9",
				"10",
				"11",
				"12",
				"13",
				"14",
				"15",
				"16"
			],
			minorTicks: 2,
			highlights: [	
			]			
		}
	},
	"QaPWM":{
		"bindEdcValue":0x4243,
		"priority":1,				
		"size":"small",		
		"init":	{
			units: "QA PWM",
			minValue: 0,
			maxValue: 4096,
			majorTicks: [
				"0",
				"1024",
				"2048",
				"3072",
				"4096"
			],
			minorTicks: 2,
			highlights: [	
			]			
		}
	},	
	"TimingPWM":{
		"bindEdcValue":0x4243,
		"priority":4,				
		"size":"small",		
		"init":	{
			units: "Timing PWM",
			minValue: 0,
			maxValue: 255,
			majorTicks: [
				"0",
				"64",
				"128",
				"192",
				"256",
			],
			minorTicks: 2,
			highlights: [	
			]			
		}
	},
	"BoostPWM":{
		"bindEdcValue":0x4243,
		"priority":4,				
		"size":"small",		
		"init":	{
			units: "Boost PWM",
			minValue: 0,
			maxValue: 255,
			majorTicks: [
				"0",
				"64",
				"128",
				"192",
				"256",
			],
			minorTicks: 2,
			highlights: [	
			]			
		}
	},
	"AuxPWM":{
		"bindEdcValue":0x4243,
		"priority":10,				
		"size":"small",		
		"init":	{
			units: "Aux PWM",
			minValue: 0,
			maxValue: 255,
			majorTicks: [
				"0",
				"64",
				"128",
				"192",
				"256",
			],
			minorTicks: 2,
			highlights: [	
			]			
		}
	}																	
}