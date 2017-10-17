var gaugeTypes = {
	"RPM":{
		"bindEdcValue":"",
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
			strokeTicks: true,
			highlights: [
				{
					"from": 1800,
					"to": 3250,
					"color": "rgba(50, 200, 50, .75)"
				},			
				{
					"from": 5250,
					"to": 6000,
					"color": "rgba(200, 50, 50, .75)"
				}
			]
		}
	}
}