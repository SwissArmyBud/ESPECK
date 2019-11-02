console.log("JS Loading...");

$(function(){
  console.log("JQuery Ready!");
  var ticker = 0;
  var progress = 0;
  setInterval(function(){
    ticker = Math.min(ticker + 1, 10);
    progress = (ticker/10.0) * 100;
    console.log("Progress: " + progress);
    $("#progressbar").css("width", parseInt(progress) + "%");
    $("#progressbar").text(parseInt(10 - ticker) + "s");
  }, 1000);
});
