/**
 * Board.js – Arduino Board Controller
 * This file is part of noduino (c) 2012 Sebastian Müller <c@semu.mp>
 *
 * @package     noduino
 * @author      Sebastian Müller <c@semu.mp>
 * @license     MIT License – http://www.opensource.org/licenses/mit-license.php 
 * @url         https://github.com/semu/noduino
 */

define(['./LED.js', './Button.js', './AnalogInput.js',  './DigitalOutput.js', './Speaker.js', './Motor.js', './RANGE.js'], function(LEDObj, ButtonObj, AnalogInputObj, DigitalOutObj, SpeakerObj, MotorObj) {

  /**
   * Create Board
   * @param object options set options like pin
   * @param object Connector switch between Serial or Socket mode
   */
  function Board(options, Connector) { 
    if (false === (this instanceof Board)) {
      return new Board(options); }
      
    this.c          = Connector;
    this.options    = options;
    console.log(this.options);
    this.pinMapping = {};
  };

  /**
   * Write to digital output on pin
   * @param integer pin pin number
   * @param string mod pin mode
   * @param function callback
   */
  Board.prototype.digitalWrite = function(range, mode, next) {
    console.log("digitalWrite in board");
    this.c.digitalWrite(range, mode, function(err) {
      if (err) { return next(err); }
    });
  };
  
  /**
   * Check if pin is already in use
   * @param integer pin pin number
   * @return boolean
   */
  Board.prototype.pinAvailable = function(pin) {
    return (this.pinMapping[pin] ? true : false);
  };

  /**
   * Get type of pin on board
   * @param integer pin pin number
   * @return mixed
   */
  Board.prototype.pinType = function(pin) {
    return this.pinMapping[pin];
  }
  
  /**
   * Create AnalogInput object on board
   * @param object options
   * @param function callback
   */
  Board.prototype.withAnalogInput = function(options, next) {
    this.with(this.c.TYPE_ANALOGIN, options, next);
  }
  
  /**
   * Create Button object on board
   * @param object options
   * @param function callback
   */
  Board.prototype.withButton = function(options, next) {
    this.with(this.c.TYPE_BUTTON, options, next);
  };
  
    /**
   * Create Range object on board
   * @param object options
   * @param function callback
   */
  Board.prototype.withRANGE = function(options) {
    this.with(this.c.TYPE_DIGITALOUT, options);
    console.log("withRANGE");
    console.log(options);
  };

  /**
   * Create DigitalOutput object on board
   * @param object options
   * @param function callback
   */
  Board.prototype.withDigitalOutput = function(options, next) {
    this.with(this.c.TYPE_DIGITALOUT, options, next);
  };

  /**
   * Create LED object on board
   * @param object options
   * @param function callback
   */
  Board.prototype.withLED = function(options, next) {
    this.with(this.c.TYPE_LED, options, next);
  };

  /**
   * Create Motor object on board
   * @param object options
   * @param function callback
   */
  Board.prototype.withMotor = function(options, next) {
    this.with(this.c.TYPE_MOTOR, options, next);
  };

  /**
   * Create Speaker object on board
   * @param object options
   * @param function callback
   */
  Board.prototype.withSpeaker = function(options, next) {
    this.with(this.c.TYPE_SPEAKER, options, next);
  };

  /**
   * Handle object creation
   * @param string what object type
   * @param object options
   * @param function callback
   */
  Board.prototype.with = function(what, options, next) {
    if (this.pinAvailable(options.range)) {
      return next(new Error('PIN already in use')); }
    
    var that = this;
    this.pinMapping[options.pin] = what;
    
    switch (what) {
      case this.c.TYPE_LED:
        this.c.withLED(options.pin, function(err, pin) {
          if (err) { return next(err); }
          next(null, new LEDObj({"pin": pin, "type": what}, that.c));
        });
      break;
      //Brukes egentlig disse??
      case this.c.TYPE_RANGE:
        this.c.withRANGE(options.range, function(err, range) {
          console.log("TYPE_RANGE");
          if (err) { return next(err); }
          next(null, new RANGEObj({"range": range, "type": what}, that.c));
        });
      break;
      case this.c.TYPE_DIGITALOUT:
        this.c.digitalWrite(options.range, function(err, range) {
          console.log("TYPE_DIGITALOUT");
          if (err) { return next(err); }
          next(null, new DigitalOut({"range": range, "type": what}, that.c));
        });
      break;
      case this.c.TYPE_BUTTON:  
        this.c.withButton(options.pin, function(err, pin) {
          if (err) { return next(err); }
          next(null, new ButtonObj({"pin": pin, "type": what}, that.c));
        });      
      break;
      case this.c.TYPE_ANALOGIN:
        this.c.withAnalogIn(options.pin, function(err, pin) {
          if (err) { return next(err); }
          next(null, new AnalogInputObj({"pin": pin, "type": what}, that.c));
        });
      break;
      case this.c.TYPE_MOTOR:
        this.c.withLED(options.pin, function(err, pin) {
          if (err) { return next(err); }
          next(null, new MotorObj({"pin": pin, "type": what}, that.c));
        });
      break;
      case this.c.TYPE_SPEAKER:
        this.c.withDigitalOut(options.pin, function(err, pin) {
          if (err) { return next(err); }    
          next(null, new SpeakerObj({"pin": pin, "type": what}, that.c));
        });
      break;
    }
  }

  return Board;
});
