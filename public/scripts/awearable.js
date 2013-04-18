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
    range: 0,
    display: 0,
    led: 0
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
      console.log("buttonSend");
      Awearable.name = $('#nameForm').val()
      Awearable.range = $('#rangeValue').val();
      Awearable.display = document.getElementById('displayId').innerHTML;
      Awearable.led = document.getElementById('ledId').innerHTML;
     
      
      
        // display and led
      //Check that a part is chosen(display or led)
      if((Awearable.display == "") && (Awearable.led == "")) {
        console.log("ButtonSend canceled:Choose at least one component");
       
        $('#e2-exampleValidate .alert').addClass('hide');
        $('#e2-exampleValidate .alert-info').removeClass('hide');
        $('#e2-exampleValidate .alert-info').html('Choose at least one part for your application before uploading'); 
        return;
      }
     //Check that display options are set
      if(Awearable.display == 'd') {
        console.log("ButtonSend canceled:Choose display options");
        
        $('#e2-exampleValidate .alert').addClass('hide');
        $('#e2-exampleValidate .alert-info').removeClass('hide');
        $('#e2-exampleValidate .alert-info').html('Choose display options');
        return;
      }
      
      //Check that led options are set
      if(Awearable.led == 'l') {
        console.log("ButtonSend canceled: Choose light options");
       
        $('#e2-exampleValidate .alert').addClass('hide');
        $('#e2-exampleValidate .alert-info').removeClass('hide');
        $('#e2-exampleValidate .alert-info').html('Choose light options');
        return;
      }
      
      //Check that range is set
      if(Awearable.range==0) {
        console.log("Choose range");
       
        $('#e2-exampleValidate .alert').addClass('hide');
        $('#e2-exampleValidate .alert-info').removeClass('hide');
        $('#e2-exampleValidate .alert-info').html('Set range');
        return;  
      }
      
      //Check this in awearble-2. When connected to arduino
      $('#e2-exampleValidate .alert').addClass('hide'); 
      $('#e2-exampleValidate .alert-success').removeClass('hide');
        
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
