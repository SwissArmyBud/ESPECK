var UPD_ANNOUNCE_PORT = 1920;

var dgram = require('dgram');
var listener = dgram.createSocket('udp4');

var start = (process.platform == 'darwin'? 'open': process.platform == 'win32'? 'start': 'xdg-open');

listener.on('listening', function () {
    console.log("UDP Listener is active...");
});

listener.on('message', function (message, client) {
    console.log("\nMessage, incoming...");
    console.log(message.toString());
    var parts = message.toString().split(":");
    // If this is an auto-discovery request, connect over https to appliance
    if(parts[0] == "ECK-AutoDiscovery"){
      console.log("Verified ECK @ " + client.address);
      require('child_process').exec(start + ' https://' + client.address + "/home.html");
      // After launching appliance SPA, exit
  		listener.removeAllListeners("message");
  		setTimeout(function(){
  			process.exit();
  		}, 1000);
    }
});

listener.bind(UPD_ANNOUNCE_PORT);
