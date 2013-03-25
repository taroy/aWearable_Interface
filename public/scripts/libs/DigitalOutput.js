define(function() {
  
  function DigitalOutput(options, Connector) {
    console.log("digitaloutput.js");
    if (false === (this instanceof DigitalOutput)) {
      return new DigitalOutput(options);
    }
    this.c      = Connector;
    this.pin    = options.pin;
    this.range  = options.range;
    this.mode   = this.c.LOW;
    this.events = {};
  };
  
  //console.log(options);

  DigitalOutput.prototype.setOn = function(callback) {
    if (!this.active) {
      this.active = true;
      this.set(this.c.HIGH, callback);
      this.emit('on');
      this.emit('change');
      console.log("her1");
    }
    
    if (callback) {
      callback();
      console.log("callback setOn");}
  };

  DigitalOutput.prototype.setOff = function(callback) {
    if (this.active) {
      this.active = false;
      this.set(this.c.LOW, callback);
      this.emit('off');
      this.emit('change');
      console.log("her3");
    }
    
    if (callback) {
      callback();
      console.log("callback setOff");}
  };

  DigitalOutput.prototype.set = function(mode, callback) {
    this.c.current().digitalWrite(this.pin, this.range, this.mode = mode, callback);
    console.log("her5");
  };
  
  DigitalOutput.prototype.clear = function(event) {
    this.events[event] = [];
    console.log("her6");
  };

  DigitalOutput.prototype.on = function(event, callback) {
    console.log(callback);
    if (!this.events[event]) {
      this.events[event] = []; }
    this.events[event].push(callback);
    console.log("her7");
  };

  DigitalOutput.prototype.emit = function(event, callback) {
    if (!this.events[event]) {
      return; }
    for (var i = 0; i < this.events[event].length; i++) {
      this.events[event][i](this); }
    console.log("her8");
  };
  
  return DigitalOutput;
});
