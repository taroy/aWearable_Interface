define(function()
{
  var a = null;
  
  function Events()
  {
    
  }
  
  console.log("awearable");
  
  var Awearable =
  {
    name: null,
    //members: [],
    latitude: 0.00,
    longitude: 0.00,
    //modules: [],
    range: 0
  };
  
  /*
  getName: function(){
    return this.name;
  }
  setName: function(name){
    this.name = name;
  }*/
  
  Events.bind = function()
  {
    
    $('#buttonConnect').click(function(e) {
      e.preventDefault();
      
      $('#e2-exampleConnection .alert').addClass('hide');    
      $('#e2-exampleConnection .alert-info').removeClass('hide');
      $('#e2-exampleConnection .alert-info').html('Trying to connect to your Arduino…');      
      require(['awearable-2'], function(example) {
        e2 = example;
        example.handle();
      });      
    });

    
    //Assigning values to Awearable object - Maria
    $('#buttonSend').click(function(e)
    {
      e.preventDefault();
      
      Awearable.name = $('#nameForm').val()
      Awearable.latitude = $('#inputLat').val();
      Awearable.longitude = $('#inputLong').val();
      Awearable.range = $('#rangeValue').val();
      console.log(Awearable.range);
      require(['awearable-2'], function(example)
      {
        e2 = example;
        example.saveApp(Awearable);
      });
    });
  };

  return Events;
});
