console.log("JS Loading...");

var xmlHttp;
var callbackRequest;
var newEl = document.createElement;
var _halt = function(event){
  event.stopPropagation();
  event.preventDefault();
};
var cloneWithHTML = function(el, html){
  var clone = el.cloneNode();
  clone.innerHTML = html;
  return clone;
};
var _maxHeight = function(selector, height){
  $(selector).css("max-height", height + "px");
};

var networks = {};
var MAX_NETWORK_SCORE = 10;

var processNetworks = function(netArray){
  // REWARD PRESENT NETWORKS BY TWO
  netArray.forEach(function(network){
    // C INE
    if(!networks[network.id]){
      networks[network.id] = {
        quality: 2,
        security: network.sec == "Y" ? true : false
      };
    } else {
      // INCREMENT EXISTING
      networks[network.id].quality = Math.min(MAX_NETWORK_SCORE, (networks[network.id].quality+2) );
    }
  });

  // PENALIZE ALL NETWORKS BY ONE, DELETE ANY ZERO VALUE RESULTS
  Object.keys(networks).forEach(function(name){
    if(networks[name].quality == 1){
      delete networks[network.id];
    } else {
      networks[name].quality--;
    }
  });

  console.log("Networks Processed...");
  console.log(networks);
};

var tableButtonHandler = function(event){ _halt(event);
  console.log("BUTTON EVENT, VALUE:");
  console.log(event.target.value);
  var box = document.getElementById("ssid_input");
  box.value = event.target.value;
};

var rebuildNetworkTable = function(){

  var table = newEl("table");
  var tHead = newEl("thead");
  var tBody = newEl("tbody");

  var tr = newEl("tr");
  var th = newEl("th");
  var td = newEl("td");

  var temp, tRow;

  tRow = tr.cloneNode();
  tRow.append(cloneWithHTML(th, "Name"))
      .append(cloneWithHTML(th, "Security"))
      .append(cloneWithHTML(th, "Quality"))
      .append(cloneWithHTML(th, "Use"));

  tHead.append(tRow);

  Object.keys(networks).forEach(function(network){
    tRow = tr.cloneNode();

    tRow.append(cloneWithHTML(td, network));

    temp = td.cloneNode();
    var status = document.createElement("div");
    status.classList.add("statusblob");
    if(networks[network].security){
      status.classList.add("green");
    } else {
      status.classList.add("red");
    }
    temp.append(status);
    tRow.append(temp);

    tRow.append(cloneWithHTML(td, networks[network].quality));

    temp = cloneWithHTML(newEl("button"), "USE");
    temp.value = network;
    temp.classList.add("network_button");
    tRow.append(temp);

    tBody.append(tRow);
  });

  table.append(tHead);
  table.append(tBody);
  table.classList.add("twelve");
  table.classList.add("columns");

  $("#table_div").empty();

  $("#table_div").append(table);
  $(".network_button").on("click", tableButtonHandler);

};

var orscCallback = function(){
  if(xmlHttp.readyState == 4){
    _maxHeight("#wait_div", 0);
    _maxHeight("#confirm_div", 350);
    console.log("Code " + xmlHttp.status + ", with response:");
    console.log(xmlHttp.responseText);
    try{
      var response = JSON.parse(xmlHttp.responseText);
      console.log(response);
      processNetworks(response.networks);
      rebuildNetworkTable();
      _maxHeight("#networks_div", 500);
    } catch (error) {
      console.log(error);
    }
    setTimeout(function(){
      console.log("New wifi list request...");
      sendPost(undefined);
    }, 5000);
  }
};

var sendPost = function(credentialString){
  xmlHttp = new XMLHttpRequest();
  xmlHttp.onreadystatechange = orscCallback;
  xmlHttp.open("POST", "/configure");
  if(headerVal){
    sendCredentials.setRequestHeader("X-ESP-CONFIG-WIFI", credentialString);
  }
  xmlHttp.send();
};

$(function(){
  console.log("JQuery Ready!");
  _maxHeight("#wait_div", 250);

  sendPost(undefined);

  $("#send_button").on("click", function(event){ _halt(event);

    var credentials = $("#ssid_input").val() + ":" + $("#pass_input").val();
    sendPost(credentials);

  });

});
