$(document).ready(()=> {
	console.log("moro");
/*	var gauge = new RadialGauge({
		renderTo: 'canvas-id',
		width: 300,
		height: 300,
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
		],
		colorPlate: "#fff",
		borderShadowWidth: 0,
		borders: false,
		needleType: "arrow",
		needleWidth: 4,
		needleCircleSize: 7,
		needleCircleOuter: true,
		needleCircleInner: false,
		value:760
	}).draw();
*/
//	document.body.appendChild(gauge.options.renderTo);	
//	$(".componentSmall").append(gauge.options.renderTo);
//	addMeter();
	conf = new Configurator();
	conf.addGauge("RPM");
});


class Configurator {
	constructor() {
		console.log("init");
		this.gauges={};
		
		window.onresize = () => {
			clearTimeout(this.resizeTimeout);
			this.resizeTimeout=setTimeout(()=>{this.resizeItems();},200);
		}
		this.resizeItems();
		if (typeof gaugeTypes != "object") {
			alert("Can't load gauges, check file: settings/gauges.js");
		}
	}
	resizeItems() {
		console.log("resizeItems");
		var meterCs = getComputedStyle($('#gauges')[0]);
		if (meterCs) {
			this.meterWidth = parseInt(meterCs.getPropertyValue('width'),10) / (this.gauges.length?this.gauges.length:1);
			this.meterHeight = parseInt(meterCs.getPropertyValue('height'),10);		
			for (var i in this.gauges) {
				this.gauges[i].update({'width':this.meterWidth,'height':this.meterHeight });
			}
		}
	}
	addGauge(gaugeName) {
		if (typeof gaugeTypes[gaugeName] != "object") {		
			console.log("addGauge: "+gaugeName+" no definition");
			return false;
		}

		var e = $("<div class='componentGauge'><canvas></canvas></div>");
		e.find('canvas').attr('id','gauge-'+gaugeName);
		$("#gauges").append(e);

		var settings = gaugeTypes[gaugeName].init;
		settings.renderTo = 'gauge-'+gaugeName;
		settings.value = '666.0';
		var gauge = new RadialGauge(settings);;	
		console.log(this.meterWidth );
		gauge.update({ width: this.meterWidth  });
		gauge.update({ value: this.meterHeight  });		
		
		gauge.draw();
		this.gauges.gaugeName = gauge;
		return true;
	}
	
}
