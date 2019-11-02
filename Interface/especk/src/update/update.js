console.log("JS Loading...");

var file;
var timer;
var ticker = 0;
var progress = 15;
var _halt = function(event){
  event.stopPropagation();
  event.preventDefault();
};
var _maxHeight = function(selector, height){
  $(selector).css("max-height", height + "px");
};

$(function(){
  console.log("JQuery Ready!");
  _maxHeight("#select_div", 250);
  $("#select_button").on("click", function(event){ _halt(event);
    $("#file_input")[0].click();
  });
  $("#file_input").on("change", function(event){ _halt(event);
    file = $("#file_input")[0].files[0];
    $("#file_text").text(file.name);
    _maxHeight("#confirm_div", 250);
  });
  $("#flash_button").on("click", function(event){ _halt(event);
    console.log("Sending form!");
    // SUBMIT FOR AND DISPLAY PROGRESS BAR
    $("#form")[0].submit();
    _maxHeight("#progress_div", 250);
    $("#progressbar").text("Loading Firmware...");
    setTimeout(function(){
      // CHANGE BAR COLOR AND BEGIN TO FILL
      $("#progressbar").removeClass("yellow")
                       .text("15%")
                       .addClass("green")
                       .css("width", "15%");
      // START UPDATING THE PROGRESS BAR
      // NOTE - These values represent the *assumed* rate of the file upload
      //        while an XHR object is explored for implementation. Currently
      //        there is no progress callback for sending the form to the ESP,
      //        so best-guess is fine until response is received from server,
      //        at which point the respective OK/ER pages are displayed.
      var approx = {
        updateRateDiv: 200, // INC TO SLOW DOWN BAR, DEC TO SPEED UP BAR
        updateInterval: 250 // INC TO SLOW DOWN BAR, DEC TO SPEED UP BAR
      };
      setInterval(function(){
        if(progress < 100){
          progress = 15 + (ticker++ * (file.size/1024) / approx.updateRateDiv);
          progress = Math.min(100, progress);
          console.log("Progress: " + progress);
          $("#progressbar").css("width", progress + "%");
          $("#progressbar").text(parseInt(progress) + "%");
        }
        else {
          // IF PROGRESS IS COMPLETE, ASK FOR PATIENCE
          _maxHeight("#wait_div", 150);
          var dots = "";
          while((ticker%4 > dots.length)){
            dots += ".";
          }
          $("#wait").text("Please wait." + dots);
        }
      }, approx.updateInterval);
    }, 1000);
  });
});
