$(document).ready(()=> {
	conf = new Configurator();
});


class Configurator {
	constructor() {
		console.log("init");
		this.gaugeView = null;
		
		if (typeof gaugeTypes == "object") {
			this.gaugeView = new GaugeView();
		} else {
			alert("Can't load gauges, check file: settings/gauges.js");
		} 
		this.showGauges();
		window.onresize = () => {
			clearTimeout(this.resizeTimeout);
			this.resizeTimeout=setTimeout(()=>{this.resizeItems();},200);
		}
		var onGetDevices = function(ports) {
			for (var i=0; i<ports.length; i++) {
			  console.log(ports[i].path);
			}
		  }
		if (typeof chrome == "undefined" || typeof chrome.serial == "undefined") {
			alert("This is Chrome Application. You need to run it from chrome://apps/. ");
		} else {
			chrome.serial.getDevices(onGetDevices);
		}
		this.pollData();
	}
	pollData() {
		this.counter=0;
		setInterval(()=>{
			this.counter++;
			if (this.counter>6000)
				this.counter=0;			
			/* update any component related this parameter */
			var event = new CustomEvent('0x4242', {
				detail: {
					value:this.counter
				},
				bubbles: false,
				cancelable: false
			});
			document.dispatchEvent(event);
		},1000/60);
		
	}

	resizeItems() {
		console.log("resizeItems");
		this.gaugeView.resizeItems();
	}
	discardOldView() {
		$("#viewInactive").append($("#viewActive div"));
	}
	showGauges() {
		this.discardOldView();
		$("#viewActive").append(this.gaugeView.getDom());
		this.gaugeView.resizeItems();		
	}
	
}
