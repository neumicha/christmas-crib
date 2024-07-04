// Complete project details: https://randomnerdtutorials.com/esp32-web-server-websocket-sliders/

var gateway = `ws://${window.location.hostname}/ws`;
var websocket;
window.addEventListener("load", onload);

function onload(event) {
  initWebSocket();
}

function getValues() {
  websocket.send("getValues");
}

function initWebSocket() {
  console.log("Trying to open a WebSocket connection…");
  websocket = new WebSocket(gateway);
  websocket.onopen = onOpen;
  websocket.onclose = onClose;
  websocket.onmessage = onMessage;
}

function onOpen(event) {
  console.log("Connection opened");
  getValues();
}

function onClose(event) {
  console.log("Connection closed");
  setTimeout(initWebSocket, 2000);
}

// NEW ONES

// Returns preset, key, value
function extractPresetAndValue(element) {
  return [
    parseInt(element.id.substr(0, 1)),
    element.id.substr(1),
    element.value,
  ];
}

function buildWsCommand(preset, key, value) {
  return "p=" + preset.toString() + "&" + key + "=" + value.toString();
}

function sendWsCommand(preset, key, value) {
  websocket.send(buildWsCommand(preset, key, value));
}

function updateUiValue(preset, key, value) {
  const element = document.getElementById(preset + key + "_value");
  if (element) {
    document.getElementById(preset + key + "_value").innerHTML = value;
  }
}

function setValue(element) {
  const [preset, key, value] = extractPresetAndValue(element);
  console.log(
    "Set value: " + preset.toString() + "|" + key + "|" + value.toString()
  );
  updateUiValue(preset, key, value);
  sendWsCommand(preset, key, value);
}

function onMessage(event) {
  // TODO
  console.log(event.data);
  var myObj = JSON.parse(event.data);

  ["lWhite", "mSpeed", "lRgb", "aType", "sSource", "sVolume", "sState"].forEach(
    (item) => {
      for (var i = 0; i < myObj[item].length; i++) {
        console.log(item + i);
        document.getElementById("0" + item + i + "_value").innerHTML =
          myObj[item][i];
        if (item != "sState") {
          document.getElementById("0" + item + i).value = myObj[item][i];
        }
      }
    }
  );
}

// console.log(myObj);
// var keys = Object.keys(myObj);

// for (var i = 0; i < keys.length; i++) {
//   var key = keys[i];
//   document.getElementById(key).innerHTML = myObj[key];
//   document.getElementById("slider" + (i + 1).toString()).value = myObj[key];
// }

// OLD ONES

/* function updateSliderPWM(element) {
  var sliderNumber = element.id.charAt(element.id.length - 1);
  var sliderValue = document.getElementById(element.id).value;
  document.getElementById("sliderValue" + sliderNumber).innerHTML = sliderValue;
  console.log(sliderValue);
  websocket.send(sliderNumber + "s" + sliderValue.toString());
}

function updateColor(element) {
  document.getElementById(element.id + "value").innerHTML = element.value;
  websocket.send(element.id + "=" + element.value.toString());
}

function playSound(element) {
  websocket.send(element.id);
}

function updateVolume(element) {
  document.getElementById(element.id + "value").innerHTML = element.value;
  websocket.send(element.id + "=" + element.value.toString());
}

function updateStepperSpeed(element) {
  document.getElementById(element.id + "value").innerHTML = element.value;
  websocket.send(element.id + "=" + element.value.toString());
} */

// Tabs
function selectPreset(evt, tabName) {
  var i, tabcontent, tablinks;
  tabcontent = document.getElementsByClassName("tabcontent");
  for (i = 0; i < tabcontent.length; i++) {
    tabcontent[i].style.display = "none";
  }
  tablinks = document.getElementsByClassName("tablinks");
  for (i = 0; i < tablinks.length; i++) {
    tablinks[i].className = tablinks[i].className.replace(" active", "");
  }
  document.getElementById(tabName).style.display = "block";
  evt.currentTarget.className += " active";
}

// Get the element with id="1select" and click on it
document.getElementById("1select").click();
