define(function(require, exports, module) {
  
  console.log("NODUINO.SERIAL");

  function SerialNoduino (options) {
    if (false === (this instanceof SerialNoduino)) {
      return new SerialNoduino(options); }
      
    this.options = options;
    this.boards  = [];
    this.board   = null;
    this.logger  = null;
  };
  
  SerialNoduino.prototype.setLogger = function(Logger) {
    this.logger = Logger;
  };
  
  SerialNoduino.prototype.log = function(level, msg) {
    this.logger.msg(level, msg);
  };

  SerialNoduino.prototype.connection  = 'serial';
  SerialNoduino.prototype.HIGH        = '255';
  SerialNoduino.prototype.LOW         = '000';
  SerialNoduino.prototype.MODE_OUT    = 'out';
  SerialNoduino.prototype.MODE_IN     = 'in';  
  SerialNoduino.prototype.TYPE_MOTOR    = 0x30;
  SerialNoduino.prototype.TYPE_LED      = 0x31;
  SerialNoduino.prototype.TYPE_BUTTON   = 0x32;
  SerialNoduino.prototype.TYPE_ANALOGIN = 0x33;
  SerialNoduino.prototype.TYPE_DIGITALOUT = 0x34;
  SerialNoduino.prototype.TYPE_SPEAKER = 0x35;
  SerialNoduino.prototype.TYPE_RANGE = 0x36;
  
  
  
  SerialNoduino.prototype.current = function() {
    return this.boards[0];
  };

  SerialNoduino.prototype.write = function(data) {
    this.current().write(data);
    console.log(data);
    console.log("NoduinoSerial write method");
  };

  SerialNoduino.prototype.connect = function(options, next) {
    /** This is Server Code: Just works with Node.js **/
    var Board = require('../../../duino/lib/board.js');
    var that = this;
    new Board({'debug': this.options.debug || false}, function(err, board) {
      if (err) { return next(new Error('Unable to connect')); }
      
      // Disabled multi board support now, but keep in mind…
      that.boards = [board]; 
      that.board  = 0;
      
      that.current().on('ready', function(){
        next(null, board); });
    });
  }

  SerialNoduino.prototype.withLED = function(pin, next) {
    this.current().pinMode(pin, this.MODE_OUT);
    next(null, pin);
    console.log("WithLED");
  };

  SerialNoduino.prototype.withButton = function(pin, next) {
    this.current().pinMode(pin, this.MODE_IN);
    next(null, pin);
  };
  
  SerialNoduino.prototype.withAnalogIn = function(pin, next) {
    this.current().pinMode(pin, this.MODE_IN);
    next(null, pin);
  }  
  
  SerialNoduino.prototype.digitalWrite = function(range, next) {
    console.log("SerialNoduino digitalWrite");
    console.log(range);
    this.current().digitalWrite(range, next);
  };  
  
  SerialNoduino.prototype.watchAnalogIn = function(AnalogInput, callback) {
    var that = this;
    setInterval(function () {
      that.current().analogRead(AnalogInput.pin);
    }, 50);
  
    this.current().on('data', function(m) {
      m = m.split('::');
      var event = {pin: that.normalizePin(m[0]), 'state': m[1]*1};
      
      if (callback) {
        return callback(event); }      
      
      if (event.pin == AnalogInput.pin) {
        AnalogInput.set(event.state); }
    });
  }
  
  SerialNoduino.prototype.normalizeVal = function(val) {
  	return ("000" + val).substr(-3);
  }

  SerialNoduino.prototype.normalizePin = function (pin) {
    return ("00" + pin).substr(-2);
  };
  
  SerialNoduino.prototype.digitalRead = function(range) {
    console.log("SerialNoduino digitalRead");
    //pin = this.normalizePin(pin);
    this.log('info', 'digitalRead from range ' + range);
    this.write('02' + range + this.normalizeVal(0));
  };
  
  SerialNoduino.prototype.watchDigitalIn = function(DigitalIn, callback) {
    var that = this;

    setInterval(function () {
      that.digitalRead(DigitalIn.pin);
    }, 50);
    
    this.current().on('data', function(m) {
      m = m.split('::');
      var event = {pin: that.normalizePin(m[0]), 'state': m[1]*1};
      
      if (callback) {
        return callback(event); }
        
      if (event.pin == DigitalIn.pin) {
        if (event.state == 0 && DigitalIn.isOn()) {
          DigitalIn.setOff();
        }
        if (event.state == 1 && !DigitalIn.isOn()) {
          DigitalIn.setOn();
        }
      }    
    });
  };
  
  return SerialNoduino;  
});