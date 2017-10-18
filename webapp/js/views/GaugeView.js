class GaugeView {
	constructor() {
		// Render gauges to hidden "views" div. It is moved to visible view by configurator class, if needed
		// Gauges listen certain events, like resize and EDC data updates 

		console.log("Adding "+Object.keys(gaugeTypes).length+" gauges");
		this.dom = $('#GaugeView');
		$("#viewInactive").append(this.dom);
		
		this.meterWidth = 200;
		this.meterHeight = 200;
		this.sizes={"large":0,"medium":0,"small":0};;
		for (var i in gaugeTypes) {
			this.sizes[gaugeTypes[i].size]++;
		}
		for (var sz in this.sizes) {
			if (this.sizes[sz]>0) {
				for (var i in gaugeTypes) {
					if (gaugeTypes[i].size == sz) {
						gaugeTypes[i].instance = this.addGauge($('#GaugeView .'+sz),i,gaugeTypes[i].init);
					}
				}		
			}	
		}	
	}
	resizeItems() {
		console.log("GaugeView resizeItems");
		for (var sz in this.sizes) {
			if (this.sizes[sz]>0) {
				var cs = getComputedStyle($('#GaugeView .'+sz)[0]);
				var w = parseInt(cs.getPropertyValue('width'),10)/this.sizes[sz];
				var h = parseInt(cs.getPropertyValue('height'),10);
				if (typeof w == "number" && typeof h == "number") {
					var z = Math.min(w,h);					
					for (var i in gaugeTypes) {
						if (gaugeTypes[i].size == sz) {
							gaugeTypes[i].instance.update({"width":z,"height":z});
						}
					}
				}		
			}	
		}		
	}
	addGauge(container,gaugeName,settings) {
		console.log(gaugeName);
		var e = $("<div class='componentGauge'><canvas></canvas></div>");
		e.find('canvas').attr('id','gauge-'+gaugeName);
		$(container).append(e);

		settings.renderTo = 'gauge-'+gaugeName;
		settings.value = '0';
		var gauge = new RadialGauge(settings);;	
		settings.instance = gauge;
		gauge.update({ "width": 42,"height": 42 });
		gauge.draw();
		$(document).on('0x4242',(e) => {gauge.update({"value":e.detail.value})} );			
		return gauge;		
	}
	getDom() {
		return this.dom;
	}
}