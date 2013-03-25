define(function() {
  function Example2() {
    this.board = null;
    this.led = null;
  }
  
  Example2.handle = function() {
    var that = this;
    require(['scripts/libs/Noduino.js', 'scripts/libs/Noduino.Socket.js', 'scripts/libs/Logger.js'], function(NoduinoObj, Connector, Logger) {
      var Noduino = new NoduinoObj({debug: false, host: 'http://localhost:8090'}, Connector, Logger);
      Noduino.connect(function(err, board) {
        $('#e2-exampleConnection .alert').addClass('hide'); 
        if (err) {
          $('#e2-exampleConnection .alert-error').removeClass('hide'); }
        else {
          $('#e2-exampleConnection .alert-success').removeClass('hide');
          $('#e2-status').html('ON');
          //Changing progression tracker when connection is etablished.
          document.getElementById("progress_image1").src = "images/prog_green.gif";
          that.board = board;
        }
      });
    });
    
  };
  
  Example2.stop = function() {
    this.led.stopBlinking();
  };
 
 //The original start function

  Example2.start = function(pin, interval) {
    var that = this;
    if (!that.led) {
      this.board.withLED({pin: pin}, function(err, LED) {
        if (err) { return console.log(err); }
        
        that.led = LED;
        that.led.blink(interval);
        that.led.on('change', function(data) {
          if (data.mode == '000') {
            $('#e2-status').removeClass('label-success');
            $('#e2-status').html('OFF');
          } else {
            $('#e2-status').addClass('label-success');
            $('#e2-status').html('ON');
          }
        });
      });
    } else {
      that.led.blink(interval);
    }
  };

 
  Example2.saveApp = function(range) {
    console.log("SaveRange");
    console.log(range);
    var that = this;
    if (!that.range) {
      this.board.withRANGE({range: range}, function(err, RANGE) {
        if (err) { return console.log(err); }

      });
    }
  };
  
  Example2.saveGeo = function(inputLat) {
    console.log("SaveGeo");
  };

  
  return Example2;
});