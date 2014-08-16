
var appMessageQueue = {
	queue: [],
	numTries: 0,
	maxTries: 5,
	working: false,
	clear: function() {
		this.queue = [];
		this.working = false;
	},
	isEmpty: function() {
		return this.queue.length === 0;
	},
	nextMessage: function() {
		return this.isEmpty() ? {} : this.queue[0];
	},
	send: function(message) {
		if (message) this.queue.push(message);
		if (this.working) return;
		if (this.queue.length > 0) {
			this.working = true;
			var ack = function() {
				appMessageQueue.numTries = 0;
				appMessageQueue.queue.shift();
				appMessageQueue.working = false;
				//console.log("Succeeded");
				appMessageQueue.send();
			};
			var nack = function() {
				appMessageQueue.numTries++;
				appMessageQueue.working = false;
				appMessageQueue.send();
			};
			if (this.numTries >= this.maxTries) {
				//console.log('Failed sending AppMessage: ' + JSON.stringify(this.nextMessage()));
				ack();
			}
			//console.log('Sending AppMessage: ' + JSON.stringify(this.nextMessage()));
			Pebble.sendAppMessage(this.nextMessage(), ack, nack);
		}
	}
}

Pebble.addEventListener("ready",
  function(e) {
  	if(localStorage.getItem("data")){
  		window.configuration=JSON.parse(localStorage.getItem("data"));
  	}else{
  		window.configuration=[];
  	}
  	//console.log("Sending stacks...");
  	if(window.configuration.length>0){
  	Pebble.showSimpleNotificationOnPebble("Loading decks...", "Please wait");
  	var transactionId = appMessageQueue.send( { "0": 2, "1":window.configuration.length});
    for (var i = 0; i < window.configuration.length; i++) {
    	var transactionId = appMessageQueue.send( { "0": 0, "1":i, "2":window.configuration[i]["q"].length+" cards", "3":window.configuration[i]["name"]});
    };
	}else{
		Pebble.showSimpleNotificationOnPebble("Welcome!", "Add some cards in the app!");
	}
    //console.log("JavaScript app ready and running!");
  }
);

Pebble.addEventListener("showConfiguration",
  function(e) {
  	Pebble.openURL('http://magmastone.net/flashcards/configurator.html');
  }
);



function sendCard(id){
	//console.log("Here");
	//console.log(JSON.stringify(configuration));
	//console.log("Here2");
    var transactionId = appMessageQueue.send( { "0": 1, "1":configuration[id]["q"].length, "2":2});
    x=0;
    for (var i = 0; i < configuration[id]["q"].length; i++) {
    	//console.log("Sending...");
    	var transactionId1 = appMessageQueue.send( { "0": 1, "1":x, "2":0 ,"3":configuration[id]["q"][i]["q"]});
    	x++;
    	var transactionId2 = appMessageQueue.send( { "0": 1, "1":x, "2":1 ,"3":configuration[id]["q"][i]["a"]});
    	x++;
    };
}
Pebble.addEventListener("webviewclosed",
  function(e) {
    window.configuration= JSON.parse(decodeURIComponent(e.response));
    localStorage.setItem("data",JSON.stringify(configuration));
    var transactionId = appMessageQueue.send( { "0": 2, "1":window.configuration.length});
    for (var i = 0; i < window.configuration.length; i++) {
    	var transactionId = appMessageQueue.send( { "0": 0, "1":i, "2":window.configuration[i]["q"].length+" cards", "3":window.configuration[i]["name"]});
    };
  }
);

Pebble.addEventListener("appmessage",
  function(e) {
  	//console.log(JSON.stringify(e.payload));

  	sendCard(e.payload["card"]);
    Pebble.showSimpleNotificationOnPebble("Loading...", "Please wait");
    
  }
);