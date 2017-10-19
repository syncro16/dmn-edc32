$(document).ready(()=> {
	conf = new Configurator();
	conf.loadSettings();
	$("button.connect").on("click",()=>{conf.openConnection();});
});


class Configurator {
	constructor() {
		console.log("init");
		this.gaugeView = null;
		this.tick=0;
		this.settings={"online":"false"};
		this.connection=null;
		this.opts={"bitrate":115200};
		this.encoder = new TextDecoder("ISO-8859-1");

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
		if (typeof chrome == "undefined" || typeof chrome.serial == "undefined") {
			console.log("This is Chrome Application. You need to run it from chrome://apps/. ");
		} else {
//			$('.header .connection .usbSerial select').on('focus',() => {this.updateDevices();});
			$('.header .connection .usbSerial select').on('change',(e) => {
				console.log($(e.target,find("option:selected")));
			});			
			setInterval(()=>{this.updateDevices();},500);
		}

		if (chrome.serial) {
			chrome.serial.onReceive.addListener((e)=>{this.connectionReceive(e)});
			chrome.serial.onReceiveError.addListener((e)=>{this.connectionReceiveError(e)});
		}
		this.pollData();
	}
	loadSettings() {
		if (!chrome.storage) {
			console.log("Cannot load settings");
			return false;
		}
		chrome.storage.sync.get(null,(r)=>{
			this.settings = r;
			if (this.settings.online) {
				this.openConnection();
			}
		});	
	}
	saveSettings() {
		if (!chrome.storage) {
			console.log("Cannot save settings");
			return false;
		}
		chrome.storage.sync.set(this.settings);

	}
	updateDevices() {	
		if (!this.settings.online || this.settings.online == false) {

			chrome.serial.getDevices((d)=>{			
				if (this.settings.knownDevices && d.length == this.settings.knownDevices.length) {
					return false;
				}
				var select = $('.header .connection .usbSerial select');
				select.empty();			
				var sel=false;
				for (var i in d) {
					var e=$('<option>'+d[i].path+'</option>');
					console.log(d[i]);
					e.attr('name',d[i].path);				
					e.text(d[i].path);
					if ((this.settings.currentDevice && d[i].path == this.settings.currentDevice) ||
						(!this.settings.currentDevice && d[i].displayName == "STM32 STLink")) {
						if (!sel)
							e.attr('selected',true);
						sel=true;
					}
					select.append(e);
				}
				this.settings.knownDevices=d;
				select.find("option[remove]").empty();					
				console.log("UpdateDevices");				
			});
		}
	}
	openConnection() {
		// check if 
		if (!this.online) {
			this.settings.currentDevice=$('.header .connection .usbSerial select')[0].selectedOptions[0].value;
			console.log("openConnection:"+this.settings.currentDevice);
			
				chrome.serial.connect(this.settings.currentDevice,this.opts,(ret)=>{
					this.connection=ret;
					$("button.connect").attr("connected",1);
					$("button.connect").removeAttr("error");
					$("button.connect").text("Disconnect");
				}
			);
		} else {
			this.closeConnection();
		}

		
	}
	closeConnection() {
		console.log("closing connection");
		chrome.serial.disconnect(this.connection.connectionId,()=>{
			$("button.connect").removeAttr("connected");
			$("button.connect").removeAttr("error");
			$("button.connect").text("Connect");			
			console.log("closing done");			
			this.online = false;
			this.connection = null;
		});
		
	}
	connectionReceive(e) {
		this.online = 1;
		console.log(this.encoder.decode(e.data));
	}
	connectionReceiveError(e) {
		$("button.connect").attr("error",1);
		
		console.log("error");
		console.log(e);		
	}
			
	getTick() {
		return this.tick;
	}
	pollData() {
		this.counter=0;
		setInterval(()=>{
			this.tick++;
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
