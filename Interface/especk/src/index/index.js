console.log("JS Ready!");
var _halt = function(event){
  event.stopPropagation();
  event.preventDefault();
};

$(function() {
    console.log("Framework loaded!");
    $("#status_button").on("click", function(event){ _halt(event);
      window.location = "/status";
    });
    $("#spiffs_button").on("click", function(event){ _halt(event);
      window.location = "/edit";
    });
    $("#update_button").on("click", function(event){ _halt(event);
      window.location = "/update";
    });
});
