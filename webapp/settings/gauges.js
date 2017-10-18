var gaugeTypes = {
	"RPM":{
		"bindEdcValue":0x4242,
		"size":"large",
		"init":	{
			units: "RPM",
			minValue: 0,
			maxValue: 6000,
			colorNeedle:"rgb(205, 0, 0)",
			colorNeedleStart:"rgb(205, 0, 0	)",			
			colorNeedleEnd:"rgb(205, 0, 0	)",			
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
			strokeTicks: true,
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
		"init":	{
			units: "Boost (bar)",
			minValue: 0,
			maxValue: 3,
			colorNeedle:"rgb(205, 0, 0)",
			colorNeedleStart:"rgb(205, 0, 0	)",			
			colorNeedleEnd:"rgb(205, 0, 0	)",
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
		"init":	{
			units: "TPS %",
			minValue: 0,
			maxValue: 100,
			colorNeedle:"rgb(205, 0, 0)",
			colorNeedleStart:"rgb(205, 0, 0	)",			
			colorNeedleEnd:"rgb(205, 0, 0	)",
			majorTicks: [
				"0",
				"25",
				"50",
				"75",
				"100"
			],
			minorTicks: 2,
			strokeTicks: true,
			highlights: []			
		}
	},
	"Injection":{
		"bindEdcValue":0x4243,
		"size":"medium",		
		"init":	{
			units: "Injection quantity",
			minValue: 0,
			maxValue: 4096,
			colorNeedle:"rgb(205, 0, 0)",
			colorNeedleStart:"rgb(205, 0, 0	)",			
			colorNeedleEnd:"rgb(205, 0, 0	)",
			majorTicks: [
				"0",
				"1024",
				"2048",
				"3072",
				"4096"
			],
			minorTicks: 4,
			strokeTicks: true,
			highlights: []			
		}
	},
	"Timing":{
		"bindEdcValue":0x4243,
		"size":"medium",		
		"init":	{
			units: "Timing °BTDC",
			minValue: -10,
			maxValue: 40,
			colorNeedle:"rgb(205, 0, 0)",
			colorNeedleStart:"rgb(205, 0, 0	)",			
			colorNeedleEnd:"rgb(205, 0, 0	)",
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
			highlights: []			
		}
	},	
	"Coolant":{
		"bindEdcValue":0x4243,
		"size":"medium",		
		"init":	{
			units: "Coolant °C",
			minValue: -20,
			maxValue: 140,
			colorNeedle:"rgb(205, 0, 0)",
			colorNeedleStart:"rgb(205, 0, 0	)",			
			colorNeedleEnd:"rgb(205, 0, 0	)",
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
		"size":"medium",		
		"init":	{
			units: "Intake °C",
			minValue: -20,
			maxValue: 140,
			colorNeedle:"rgb(205, 0, 0)",
			colorNeedleStart:"rgb(205, 0, 0	)",			
			colorNeedleEnd:"rgb(205, 0, 0	)",
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
		"size":"small",		
		"init":	{
			units: "Fuel °C",
			minValue: -20,
			maxValue: 140,
			colorNeedle:"rgb(205, 0, 0)",
			colorNeedleStart:"rgb(205, 0, 0	)",			
			colorNeedleEnd:"rgb(205, 0, 0	)",
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
	"AuxTemp":{
		"bindEdcValue":0x4243,
		"size":"small",		
		"init":	{
			units: "Aux °C",
			minValue: -20,
			maxValue: 140,
			colorNeedle:"rgb(205, 0, 0)",
			colorNeedleStart:"rgb(205, 0, 0	)",			
			colorNeedleEnd:"rgb(205, 0, 0	)",
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
		"size":"small",		
		"init":	{
			units: "Aux In (v)",
			minValue: 0,
			maxValue: 5,
			colorNeedle:"rgb(205, 0, 0)",
			colorNeedleStart:"rgb(205, 0, 0	)",			
			colorNeedleEnd:"rgb(205, 0, 0	)",
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
		"size":"small",		
		"init":	{
			units: "Battery (v)",
			minValue: 9,
			maxValue: 18,
			colorNeedle:"rgb(205, 0, 0)",
			colorNeedleStart:"rgb(205, 0, 0	)",			
			colorNeedleEnd:"rgb(205, 0, 0	)",
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
			strokeTicks: true,
			highlights: [	
			]			
		}
	},
	"QaPWM":{
		"bindEdcValue":0x4243,
		"size":"small",		
		"init":	{
			units: "QA PWM",
			minValue: 0,
			maxValue: 4096,
			colorNeedle:"rgb(205, 0, 0)",
			colorNeedleStart:"rgb(205, 0, 0	)",			
			colorNeedleEnd:"rgb(205, 0, 0	)",
			majorTicks: [
				"0",
				"1024",
				"2048",
				"3072",
				"4096"
			],
			minorTicks: 2,
			strokeTicks: true,
			highlights: [	
			]			
		}
	},	
	"TimingPWM":{
		"bindEdcValue":0x4243,
		"size":"small",		
		"init":	{
			units: "Timing PWM",
			minValue: 0,
			maxValue: 255,
			colorNeedle:"rgb(205, 0, 0)",
			colorNeedleStart:"rgb(205, 0, 0	)",			
			colorNeedleEnd:"rgb(205, 0, 0	)",
			majorTicks: [
				"0",
				"64",
				"128",
				"192",
				"256",
			],
			minorTicks: 2,
			strokeTicks: true,
			highlights: [	
			]			
		}
	},
	"BoostPWM":{
		"bindEdcValue":0x4243,
		"size":"small",		
		"init":	{
			units: "Boost PWM",
			minValue: 0,
			maxValue: 255,
			colorNeedle:"rgb(205, 0, 0)",
			colorNeedleStart:"rgb(205, 0, 0	)",			
			colorNeedleEnd:"rgb(205, 0, 0	)",
			majorTicks: [
				"0",
				"64",
				"128",
				"192",
				"256",
			],
			minorTicks: 2,
			strokeTicks: true,
			highlights: [	
			]			
		}
	},
	"AuxPWM":{
		"bindEdcValue":0x4243,
		"size":"small",		
		"init":	{
			units: "Aux PWM",
			minValue: 0,
			maxValue: 255,
			colorNeedle:"rgb(205, 0, 0)",
			colorNeedleStart:"rgb(205, 0, 0	)",			
			colorNeedleEnd:"rgb(205, 0, 0	)",
			majorTicks: [
				"0",
				"64",
				"128",
				"192",
				"256",
			],
			minorTicks: 2,
			strokeTicks: true,
			highlights: [	
			]			
		}
	}																	
}