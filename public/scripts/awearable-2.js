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
    Awearable2.writeToBoard(Awearable.range, Awearable.led, Awearable.display);
    console.log(Awearable.display);
    $('#e2-exampleValidate .alert').addClass('hide'); 
    $('#e2-exampleValidate .alert-success').removeClass('hide'); 
  };  
  
  Awearable2.writeToBoard = function(range, led, display)
  {
    var that = this;
    
    console.log("range");
    console.log(range);
    console.log("led");
    console.log(led);
    console.log("display");
    console.log(display);
    if (!that.range)
    {
      this.board.withObject({range: range, led: led, display: display}, function(err, LED)
      {
        if (err)
        {
          return console.log(err);
        }
      });
    }
  };
  
  Awearable2.saveGroup = function(Groupmember)
  {
    Awearable2.writeGroup(Groupmember.lat, Groupmember.lon);
  };  
  
  //Saving group/ hardcoded latitude and longitude
  Awearable2.writeGroup = function(lat, lon)
  {
    console.log(lat);
    console.log(lon);
    var that = this;
    if(!that.lat)
    {
      console.log("her");
      console.log(lat);
      this.board.withPosition({lat: lat, lon: lon}, function(err)
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