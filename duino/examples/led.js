var arduino = require('../');

console.log("examplesLed")

var board = new arduino.Board({
  debug: true
});

var led = new arduino.Led({
  board: board
});

board.on('ready', function(){
  console.log("examplesLed");
  led.blink();
});