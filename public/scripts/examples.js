define(function() {
  
  var e1 = null;
  var e2 = null;
  var e3 = null;
  function Events() {
    
  }
  
  Events.bind = function() {
    /**
     * Exampel #1
     */
    $('#e1-buttonConnect').click(function(e) {
      e.preventDefault();
      
      $('#e1-exampleConnection .alert').addClass('hide');    
      $('#e1-exampleConnection .alert-info').removeClass('hide');
      $('#e1-exampleConnection .alert-info').html('Trying to connect to your Arduino…');      
      require(['example-1'], function(example) {
        example.handle();
      });      
    });
    
    $('#e2-buttonStop').click(function(e) {
      e.preventDefault();
      e2.stop();
    });
    
    $('#e2-buttonStart').click(function(e) {
      e.preventDefault();
      $('#e2-secondStep .alert').addClass('hide');    
      
      if ($('#e2-interval').val()*1 < 25) {
        $('#e2-secondStep .alert-error').removeClass('hide');
        $('#e2-secondStep .alert-error').html('Interval less than 25ms is not allowed!');
      } else if (e2 == null) {
        $('#e2-secondStep .alert-error').removeClass('hide');
        $('#e2-secondStep .alert-error').html('Connect to your Arduino first!');      
      } else {
        console.log($('#e2-pinValue').val());
        e2.start($('#e2-pinValue').val(), $('#e2-interval').val());
      }
    });

    
    //Sending range to Arduino board. - Maria
    $('#e2-buttonSend').click(function(e) {
      e.preventDefault();
      
      var lat = parseFloat($('#inputLat').val());
      var lon = parseFloat($('#inputLong').val());
      
      console.log("Application name:", $('#nameForm').val());
      console.log("Range:", $('#e2-rangeValue').val());
      e2.saveApp($('#e2-rangeValue').val());
      //e2.saveApp($('#e2-rangeValue').val(), $('#inputLat').val(), $('#inputLong').val());
    });
    
    //Sending lat and long to Arduino board - Maria
    $('#e2-geoSend').click(function(e) {
      e.preventDefault();
      console.log($('#inputLat').val());
      var v1 = parseFloat($('#inputLat').val());
      console.log(v1);
      if(v1!=null) {
        console.log(v1);
        e2.saveGeo(v1);
        console.log(v1);
      }
      
      
    });    
    
    //Changing progress tracker when range is selected - Maria
    $('#e2-rangeValue').change(function() {
      if ($(this).val() != 000) {
        document.getElementById("progress_image5").src = "images/prog_green.gif";
      }
    });
    
 
    
    //Preventing form from commiting when enter is pressed - Maria
    $(document).ready(function() {
      $(window).keydown(function(event){
        if(event.keyCode == 13) {
          event.preventDefault();
          return false;
        }
      });
    });
    
    $('#e2-buttonConnect').click(function(e) {
      e.preventDefault();
      
      $('#e2-exampleConnection .alert').addClass('hide');    
      $('#e2-exampleConnection .alert-info').removeClass('hide');
      $('#e2-exampleConnection .alert-info').html('Trying to connect to your Arduino…');      
      require(['example-2'], function(example) {
        e2 = example;
        example.handle();
      });      
    });
    
    $('#e3-buttonConnect').click(function(e) {
      e.preventDefault();
      
      $('#e3-exampleConnection .alert').addClass('hide');    
      $('#e3-exampleConnection .alert-info').removeClass('hide');
      $('#e3-exampleConnection .alert-info').html('Trying to connect to your Arduino…');      
      require(['example-3'], function(example) {
        example.handle();
      });      
    });
    
    
  };
  
  //Trying out progression tracker
  /*
  $(window).load(function(){
    $("ol.progtrckr").each(function(){
        $(this).attr("data-progtrckr-steps", 
                     $(this).children("li").length);
    });
  })*/
  
  return Events;
});