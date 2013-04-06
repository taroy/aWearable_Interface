define(function(require, exports, module) {
  
  console.log("NODUINO.SOCKET");

  function Board(options) {
    
  };
  
  Board.prototype.pinMode = function() {
    
  };

  function SocketNoduino (options) {
    if (false === (this instanceof SocketNoduino)) {
      return new SocketNoduino(options); }
    
    console.log("socketNoduino options");
    console.log(options);
      
    this.options  = options;
    console.log(options);
    this.boards   = [];
    this.board    = null;
    this.logger   = null;
    this.io       = null;
    this.sockets  = [];
    this.callbacks= {};
    this.checkSocket();
  };

  SocketNoduino.prototype.setLogger = function(Logger) {
    this.logger = Logger;
  };
  
  SocketNoduino.prototype.log = function(level, msg) {
    return this.logger.msg(level, msg);
  };

  SocketNoduino.prototype.connection  = 'serial';
  SocketNoduino.prototype.HIGH        = '255';
  SocketNoduino.prototype.LOW         = '000';
  SocketNoduino.prototype.MODE_OUT    = 'out';
  SocketNoduino.prototype.MODE_IN     = 'in';  
  SocketNoduino.prototype.DIGITAL_ON    = '1';
  SocketNoduino.prototype.DIGITAL_OFF   = '0';
  SocketNoduino.prototype.TYPE_MOTOR    = 0x30;
  SocketNoduino.prototype.TYPE_LED      = 0x31;
  SocketNoduino.prototype.TYPE_BUTTON   = 0x32;
  SocketNoduino.prototype.TYPE_ANALOGIN = 0x33;
  SocketNoduino.prototype.TYPE_DIGITALOUT = 0x34;
  SocketNoduino.prototype.TYPE_SPEAKER = 0x35;
  
  SocketNoduino.prototype.current = function() {
    return this;
  };
  
  SocketNoduino.prototype.checkSocket = function() {
    /** This is Client code: This works in your web browser */
    if (!this.io) {
      this.io = io.connect(this.options.host); 
      this.boards[0] = Board;
    }
    
    var that  = this; 
    var board = this.boards[0];
    
    this.io.on('response', function(data) { 
      var err = data;
      switch(data.msg) { 
        case 'board.connect': 
          if (data.response == 'ready') {
            err=null; }
          if (that.callbacks[data.msg].length > 0) {
            var max = that.callbacks[data.msg].length;
            for (var i = 0; i < max; i++) {
              if (!that.callbacks[data.msg][i]) {
                continue; }
              
              that.callbacks[data.msg][i](err, board); 
              delete that.callbacks[data.msg][i];
            }
          }
        break;
      } 
    });
  };
  
  SocketNoduino.prototype.setSocket = function() {

  };

  SocketNoduino.prototype.connect = function(options, next) {
    if (!next) { 
      next = options; } 
    if (!this.callbacks['board.connect']) {
      this.callbacks['board.connect'] = []; }
    
    this.callbacks['board.connect'].push(next);
    this.log('sending command through socket');
    this.pushSocket('board.connect');
    
    return;
  }
  
  SocketNoduino.prototype.pushSocket = function(type, data) {
    console.log("push socket");
    this.log('socket-write', type + ': ' + JSON.stringify(data));
    this.io.emit(type, data);
    console.log(data);
  }
  
  SocketNoduino.prototype.write = function(cmd, callback) {
    console.log("socket da vet, write");
    console.log(cmd);
    this.log('info', 'writing: ' + cmd);
    this.pushSocket('serial', {'type': 'write', 'write': cmd, 'id': this.io.socket.sessionid});
  };

  SocketNoduino.prototype.pinMode = function(pin, val) {
    console.log("SocketNoduino.prototype.pinMode");
    pin = this.normalizePin(pin);
    val = (val == this.MODE_OUT ? this.normalizeVal(1) : this.normalizeVal(0));
    this.log('info', 'set pin ' + pin + ' mode to ' + val);
    
    this.write('00' + pin + val);    
  };
  
  //Maria
  SocketNoduino.prototype.withObject = function(range, lat, lon) {
    console.log("SocketNoduino.prototype.withObject");
    this.log('info', 'set range ' + range );
    
    this.write('00' + range + lat + lon);    
  };
  
  //Maria
  SocketNoduino.prototype.withPosition = function(lat, lon) {
    console.log("SocketNoduino.prototype.withPosition");
    this.log('info', 'set position ' + lat + lon );
    
    this.write('00' + ':' + lat + ':' + lon);    
  };

  SocketNoduino.prototype.normalizeVal = function(val) {
  	return ("000" + val).substr(-3);
  }

  SocketNoduino.prototype.normalizePin = function (pin) {
    return ("00" + pin).substr(-2);
  };

  SocketNoduino.prototype.withLED = function(pin, next) {
    console.log("SocketNoduino.prototype.withLED");
    console.log(pin);
    this.pinMode(pin, this.MODE_OUT);
    next(null, pin);
  };
  
  // Maria
  SocketNoduino.prototype.withRANGE = function(range) {
    console.log("withRANGE in socket")
    return (range);
  };

  SocketNoduino.prototype.withButton = function(pin, next) {
    this.pinMode(pin, this.MODE_IN);
    next(null, pin);
  };
  
  SocketNoduino.prototype.withAnalogIn = function(pin, next) {
    this.pinMode(pin, this.MODE_IN);
    next(null, pin);
  }  
  
  /*
  SocketNoduino.prototype.digitalWrite = function(pin, range, val, next) {
  	pin = this.normalizePin(pin);
  	val = this.normalizeVal(val);
  	this.log('info', 'analogWrite to pin ' + pin + ': ' + val);
  	this.write(range + range + range + range);
	console.log("pin");
	console.log(pin);
	console.log("val");
	console.log(val);
	console.log("next");
	console.log(next);
	console.log("range");
	console.log(range);
    
    if (next) {
      next(null, pin); }
  };
  */
  
  
  //Is this the shit?
  SocketNoduino.prototype.digitalWrite = function(range, val, next) {
      this.log('info', 'analogWrite to pin ' + range);
      this.write(range);
      console.log("digitalWrite in socket noduino");
      if (next) {
       next(null, range); }
  };  
  
  SocketNoduino.prototype.watchAnalogIn = function(AnalogInput) {
    var that = this;
    
    this.analogRead(AnalogInput.pin);
    this.io.on('response', function(data) {
      if (data.type == 'analogRead' && data.pin == AnalogInput.pin) {
        that.log('socket-read', JSON.stringify(data));
        var event = {pin: data.pin, value: data.value*1};
        if (event.pin == AnalogInput.pin) {
          AnalogInput.set(event.value);
        }
      }
    });
  }
  
  SocketNoduino.prototype.analogRead = function (pin) {
    this.pushSocket('serial', {'type': 'analogRead', 'pin': this.normalizePin(pin)});
  }
  
  SocketNoduino.prototype.digitalRead = function (pin) {
    this.pushSocket('serial', {'type': 'digitalRead', 'pin': this.normalizePin(pin)});
  }
  
  SocketNoduino.prototype.watchDigitalIn = function(DigitalIn) {
    var that = this;
    
    this.digitalRead(DigitalIn.pin);
    this.io.on('response', function(data) {
      if (data.type == 'digitalRead' && data.pin == DigitalIn.pin) {
        that.log('socket-read', JSON.stringify(data));
        
        if (data.value == 1) {
          DigitalIn.setOn(); }
        else {
          DigitalIn.setOff(); }
      }
    });
  }
  
  return SocketNoduino;  
});

