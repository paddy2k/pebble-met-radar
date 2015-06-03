var myAppMessageQueue = {
	queue: [],
	numTries: 0,
	maxTries: 5,
	wworking: false,
	clear: function() {
		this.queue = [];
		this.wworking = false;
	},
	isEmpty: function() {
		return this.queue.length === 0;
	},
	nextMessage: function() {
		return this.isEmpty() ? {} : this.queue[0];
	},
	send: function(message) {
    var _this = this; 
		if (message) this.queue.push(message);
		if (this.wworking) return;
		if (this.queue.length > 0) {
			this.wworking = true;
			if (this.numTries >= this.maxTries) {
				console.log('Failed sending AppMessage: ' + JSON.stringify(this.nextMessage()));
				ack();
			}
			console.log('Sending AppMessage: ' + JSON.stringify(this.nextMessage()));
			Pebble.sendAppMessage(_this.nextMessage(), 
        function(e) {
				  _this.numTries = 0;
			  	_this.queue.shift();
		  		_this.wworking = false;
	  			_this.send();
          console.log('AppMessage SENT: ' + JSON.stringify(this.nextMessage()));
          console.log('AppMessage SENT: ' + JSON.stringify(e));
			  },
        function(e) {
  				_this.numTries++;
  				_this.wworking = false;
  				_this.send();
          console.log('AppMessage NACK: ' + JSON.stringify(e));
  			}
      );
		}
	}
};