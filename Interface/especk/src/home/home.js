console.log("JS Ready!");
_halt = function(event){
  event.stopPropagation();
  event.preventDefault();
};
$(function() {
    console.log("Framework loaded!");
    $("#stuff_button").on("click", function(event){ _halt(event);
      console.log("Hey, look, STUFF!");
    });
    $("#button_button").on("click", function(event){ _halt(event);
      console.log("BUTTON PRESS!");
    });
    $("#request_button").on("click", function(event){ _halt(event);
      console.log("Request path started...");
      var blinkPeriod = $("#time_input")[0].value;
      console.log("Requesting - " + blinkPeriod);

      var xhr = new XMLHttpRequest();
      xhr.onreadystatechange = function(){
        if(xhr.readyState == 4){
          console.log("Code " + xhr.status + ", with response:");
          console.log(xhr.responseText);
        }
      };
      // Set type and destination of request to the appliance
      // Note - In demo the endpoint is routed through a capture portal
      // but in a deployed appliance all specific routes should be bound, with
      // an error handler to catch bad requests that drop through
      xhr.open("POST", "/endpoint");
      xhr.setRequestHeader("X-ESP-VALUE-TEST", Number(blinkPeriod));
      xhr.send();
    });
});
