define(['./DigitalOutput.js'], function(LED) {
  
  console.log("publicLed");
  
  LED.prototype.toggle = function(callback) {
    if (this.mode == this.c.LOW) {
      return this.setOn(callback); }
    this.setOff(callback);
  };
  
  LED.prototype.stopBlinking = function() {
    if (this.interval) {
      clearInterval(this.interval); }
  }

  LED.prototype.blink = function(interval) {
    console.log(interval);
    if (interval*1 < 25) {
      interval = 50; }
    this.stopBlinking();
    
    var that = this;
    this.interval = setInterval(function(){
      console.log(interval);
      that.toggle();
    }, interval);
  };

  return LED;
});
