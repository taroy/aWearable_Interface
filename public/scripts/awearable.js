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
    latitude: '63,427029',
    longitude: '10,396700',
    //modules: [],
    range: 0
  };
  
  var Groupmember =
  {
    lat: 63.427029,
    lon: 10.396700
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
      Awearable.range = $('#rangeValue').val();
      console.log(Awearable.range);
      require(['awearable-2'], function(example)
      {
        e2 = example;
        example.saveApp(Awearable);
      });
    });
  
    //Save group - Maria
    $('#saveGroup').click(function(e)
    {
      e.preventDefault();
      require(['awearable-2'], function(example)
      {
        e2 = example;
        example.saveGroup(Groupmember);
      });
    });
  };

  return Events;
});
