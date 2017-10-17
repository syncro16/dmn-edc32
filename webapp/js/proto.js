$(document).ready(function() {
	console.log("Ready!");
});

class Item {
	constructor() {
		console.log('constr');
		console.log(this);
		this.refresh();
		var me=$("<div class='z'>TestItem<p></div>");
		$('#view').append(me);
		$(document).on('0x4242',() => {this.refresh();} );	
		
	}
	refresh() {
		console.log("refresh");
	}

}
$(document).on('0x4242',function(e) {
	console.log("joo");
	console.log(e);
});

function createItem() {
	var e=new Item();
//	var e=$("<div class='z'>TestItem<p></div>");
//	return e;
}

var items=[];
function doit() {
	createItem();
	//items.push(item);
	//$('#view').append(item);
	//item.on('0x4242',function(){console.log('append');});
	var event = new CustomEvent('0x4242', {
	detail: {
		username: "kuppanen"
	},
	bubbles: false,
	cancelable: false
});
	document.dispatchEvent(event);

}
