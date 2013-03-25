define(function() {
  function Awearable2()
  {
    this.board = null;
    this.led = null;
  }
  
  Awearable2.handle = function() {
    console.log("awearable2");
    var that = this;
    require(['scripts/libs/Noduino.js', 'scripts/libs/Noduino.Socket.js', 'scripts/libs/Logger.js'], function(NoduinoObj, Connector, Logger) {
      var Noduino = new NoduinoObj({debug: false, host: 'http://localhost:8090'}, Connector, Logger);
      Noduino.connect(function(err, board) {
        $('#e2-exampleConnection .alert').addClass('hide'); 
        if (err) {
          $('#e2-exampleConnection .alert-error').removeClass('hide'); }
        else {
          $('#e2-exampleConnection .alert-success').removeClass('hide'); 
          that.board = board;
        }
      });
    });
    
  };
  
  //Saving application/Awearable object
  Awearable2.saveApp = function(Awearable)
  {
    Awearable2.writeToBoard(Awearable.range, Awearable.latitude, Awearable.longitude);
  };  
  
  Awearable2.writeToBoard = function(range, lat, lon)
  {
    console.log("writeToBoard1");
    console.log(lat);
    var that = this;
    //that = Window {top:window, window:Window, location:location osv..}
    if (!that.range)
    {
      console.log("writeToBoard");
      console.log(range);
      console.log(lat);
      this.board.withObject({range: range, lat: lat, lon: lon}, function(err, LED)
      {
        if (err)
        {
          return console.log(err);
        }
      });
    }
  };
  return Awearable2;
});