#include <WiFi.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <Adafruit_AHTX0.h>
#include <AutoStepper.h>  //created my library

// all pin numbers
#define SDA_PIN 21
#define SCL_PIN 22
const int relayPin = 2;
const int trigPin = 4;
const int echoPin = 15; 
int motorPin1 = 16;
int motorPin2 = 17;
int motorPin3 = 18;
int motorPin4 = 19;
const int fotoResPin = 34;

// class instances
AutoStepper stepper(motorPin1, motorPin2, motorPin3, motorPin4);

Adafruit_AHTX0 aht;
Adafruit_Sensor *aht_humidity, *aht_temp;
sensors_event_t humidity;
sensors_event_t temp;

// definded threshlod values
const int fotoResThrNight = 380;
const int fotoResThrDay = 750;
int stepsLimit = 51500; 

// the rest of global variables
bool isAHT  = false;
bool isAutoWork = false;
bool isLedAuto = false;
bool ledStripOn = false;
bool isPerson = false;
bool isUp = false;
bool isDown = false;
bool relayNowOn = false;
int fotoResVal = 0;
int spinDir = 0;
int stepsNo = 0;
int photoResVal = 0;
int distance = 0;
unsigned long prevMillisEnd = 0;
unsigned long prevMillisStart = 0;

// additional sting variables for displaying values on local website
String myTemp = "-";
String myHum = "-";
String blindPos = "NA";


// Replaced with my network credentials
const char* ssid     = "MyHomeWiFi";
const char* password = "123456789";

// Set async web server port number to 80
AsyncWebServer server(80);

// Search for parameters in HTTP POST request
const char* PARAM_INPUT_1 = "variable";
const char* PARAM_INPUT_2 = "state";

const char index_html[] PROGMEM = R"rawliteral(
<!DOCTYPE HTML><html>
<head>
  <title>ESP Web Server</title>
  <meta name="viewport" content="width=device-width, initial-scale=1">
  <link rel="icon" href="data:,">
  <style>
    html {font-family: Arial; display: inline-block; text-align: center;}
    h2 {font-size: 1.8rem;}
    p {font-size: 1.2rem;}
    .info-label{
      font-size: 1.2rem;
      vertical-align:middle;
      padding-bottom: 5px;
    }
    body {max-width: 600px; margin:0px auto; padding-bottom: 5px;}
    .switch {position: relative; display: inline-block; width: 88px; height: 40px} 
    .switch input {display: none}
    .slider {position: absolute; top: 0; left: 0; right: 0; bottom: 0; background-color: #ccc; border-radius: 6px}
    .slider:before {position: absolute; content: ""; height: 32px; width: 40px; left: 4px; bottom: 4px; background-color: #fff; -webkit-transition: .3s; transition: .3s; border-radius: 3px}
    input:checked+.slider {background-color: #7FFF00}
    input:checked+.slider:before {-webkit-transform: translateX(40px); -ms-transform: translateX(40px); transform: translateX(40px)}
    .button {
      padding: 10px 20px; font-size: 22px; text-align: center; outline: #0D084F; color: #fff; background-color: #52CBF1; border: none; border-radius: 5px;
      cursor: pointer; -webkit-tap-highlight-color: rgba(0,0,0,0); 
    } 
    .button:hover {background-color: #52CBF1 }
    .button:active {
      background-color: #E26AF1; 
      transform: translateY(2px);
    }
    .buttonRed {
      padding: 10px 20px; font-size: 20px; text-align: center; outline: none; color: #fff; background-color: #E40F0F; border: none; border-radius: 5px;
      cursor: pointer; -webkit-tap-highlight-color: rgba(0,0,0,0); 
    } 
    .buttonRed:hover {background-color: #E40F0F }
    .buttonRed:active {
      background-color: #1fe036;
      transform: translateY(2px);
    }
  </style>
</head>
<body>
  <p>
  <button class="button" onmousedown="toggleButton('tempUpdate');" ontouchstart="toggleButton('tempUpdate');" onmouseup="toggleButton('OFF');" ontouchend="toggleButton('OFF');">UPDATE</button>
  </p>
  <p>
    <span class="info-label">Temperature = </span> 
    <span id="temp">%TEMP%</span>
    <span class="info-label"> C</span> 
  </p>   
  <p>
    <span class="info-label">Humidity = </span> 
    <span id="humid">%HUM%</span>
    <span class="info-label"> rH</span> 
  </p> 
  <p>
    <span class="info-label">Person distance = </span> 
    <span id="dist">%DIS%</span>
  </p> 
  %LEDBUTTONS%

  <h2>blind control</h2>
  <p>
    <span class="info-label">Light sensor value = </span> 
    <span id="lighting">%LIGHT%</span>
  </p>
  <p>
    <span class="info-label">Blind position: </span> 
    <span id="blindPos">%POS%</span>
  </p>
  %BLINDSBUTTONS%

  <p>
  <button class="button" onmousedown="toggleButton('UP');" ontouchstart="toggleButton('UP');" onmouseup="toggleButton('OFF');" ontouchend="toggleButton('OFF');">is UP</button>
  <button class="button" onmousedown="toggleButton('DOWN');" ontouchstart="toggleButton('DOWN');" onmouseup="toggleButton('OFF');" ontouchend="toggleButton('OFF');">is DOWN</button>
  </p>
  <p>
  <buttonRed class="buttonRed" onmousedown="toggleButton('stop');" ontouchstart="toggleButton('stop');" onmouseup="toggleButton('OFF');" ontouchend="toggleButton('OFF');">STOP</button>
  </p>

<script>
  setInterval(function ( ) {
    var xhttp = new XMLHttpRequest();
    xhttp.onreadystatechange = function() {
      if (this.readyState == 4 && this.status == 200) {
        document.getElementById("temp").innerHTML = this.responseText;
      }
    };
    xhttp.open("GET", "/temp", true);
    xhttp.send();
  }, 60000 ) ;

  setInterval(function ( ) {
    var xhttp = new XMLHttpRequest();
    xhttp.onreadystatechange = function() {
      if (this.readyState == 4 && this.status == 200) {
        document.getElementById("humid").innerHTML = this.responseText;
      }
    };
    xhttp.open("GET", "/humid", true);
    xhttp.send();
  }, 60000 ) ;

  setInterval(function ( ) {
    var xhttp = new XMLHttpRequest();
    xhttp.onreadystatechange = function() {
      if (this.readyState == 4 && this.status == 200) {
        document.getElementById("lighting").innerHTML = this.responseText;
      }
    };
    xhttp.open("GET", "/lighting", true);
    xhttp.send();
  }, 10000 ) ;

  setInterval(function ( ) {
    var xhttp = new XMLHttpRequest();
    xhttp.onreadystatechange = function() {
      if (this.readyState == 4 && this.status == 200) {
        document.getElementById("blindPos").innerHTML = this.responseText;
      }
    };
    xhttp.open("GET", "/blindPos", true);
    xhttp.send();
  }, 5000 ) ;

    setInterval(function ( ) {
    var xhttp = new XMLHttpRequest();
    xhttp.onreadystatechange = function() {
      if (this.readyState == 4 && this.status == 200) {
        document.getElementById("dist").innerHTML = this.responseText;
      }
    };
    xhttp.open("GET", "/dist", true);
    xhttp.send();
  }, 1000 ) ;

  function toggleCheckbox(element) {
    var xhr = new XMLHttpRequest();
    if(element.checked){ xhr.open("GET", "/update?variable="+element.id+"&state=1", true); }
    else { xhr.open("GET", "/update?variable="+element.id+"&state=0", true); }
    xhr.send();
  }

  function toggleButton(x) {
    var xhr = new XMLHttpRequest();
    xhr.open("GET", "/" + x, true);
    xhr.send();
  }

</script>
</body>
</html>
)rawliteral";

// Replaces placeholder with button section in your web page
String processor(const String& var){
  //Serial.println(var);
  if(var == "BLINDSBUTTONS"){
    String buttons = "";
    buttons += "<h4>AUTO blind</h4><label class=\"switch\"><input type=\"checkbox\" onchange=\"toggleCheckbox(this)\" id=\"autoBlind\" " + outputIsOn(0) + "><span class=\"slider\"></span></label>";
    buttons += "<h4>GO UP</h4><label class=\"switch\"><input type=\"checkbox\" onchange=\"toggleCheckbox(this)\" id=\"spinUp\" " + outputIsOn(1) + "><span class=\"slider\"></span></label>";
    buttons += "<h4>GO DOWN</h4><label class=\"switch\"><input type=\"checkbox\" onchange=\"toggleCheckbox(this)\" id=\"spinDown\" " + outputIsOn(-1) + "><span class=\"slider\"></span></label>";
    return buttons;
  }
  if(var == "LEDBUTTONS"){
    String buttons = "";
    buttons += "<h4>AUTO LED</h4><label class=\"switch\"><input type=\"checkbox\" onchange=\"toggleCheckbox(this)\" id=\"autoLed\" " + outputIsOn(2) + "><span class=\"slider\"></span></label>";
    buttons += "<h4>LED strip</h4><label class=\"switch\"><input type=\"checkbox\" onchange=\"toggleCheckbox(this)\" id=\"relay\" " + outputIsOn(3) + "><span class=\"slider\"></span></label>";
    return buttons;
  }
  if(var == "LIGHT"){
    return String(photoResVal);
  }
  if(var == "POS"){
    return blindPos;
  }
  if(var == "DIS"){
    return String(distance);
  }
  if(var == "TEMP"){
    return myTemp;
  }
  if(var == "HUM"){
    return myHum;
  }

  return String();
}

String outputIsOn(int caseNo){
  switch(caseNo){
    case 0:
      if(isAutoWork){
        return "checked";
      }
    case 1:
      if(!isAutoWork && spinDir == 1){
        return "checked";
      }
    case -1:
      if(!isAutoWork && spinDir == -1){
        return "checked";
      }
    case 2:
      if(isLedAuto){
        return "checked";
      }
    case 3:
      if(ledStripOn){
        return "checked";
      }
  }
  return "";
}

void setup(){
  // Serial port for debugging purposes
  Serial.begin(115200);

  pinMode(trigPin, OUTPUT); //dist. sensor output pin
  pinMode(echoPin, INPUT);  //dist. sensor input pin
  pinMode(relayPin, OUTPUT);
  digitalWrite(relayPin, LOW); 
  
  // Initialize AHT sensor
  if (!aht.begin()) {
    Serial.println("Failed to find AHT10 chip");
  }
  else{
    isAHT = true;
    Serial.println("AHT10 Found!");

    aht_temp = aht.getTemperatureSensor();
    aht_humidity = aht.getHumiditySensor();
    // uncomment below section to get info about sensor details
    // aht_temp->printSensorDetails();
    // aht_humidity->printSensorDetails();

    delay(1000);
  }
  
  // Connect to Wi-Fi
  WiFi.begin(ssid, password);
  Serial.print("Connecting to WiFi..");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(1000);
  }
  Serial.println("");
  // Print ESP Local IP Address
  Serial.println(WiFi.localIP());

  // Route for root / web page
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send_P(200, "text/html", index_html, processor);
  });

  server.on("/lighting", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send_P(200, "text/plain", String(analogRead(fotoResPin)/4).c_str());
  });
  server.on("/blindPos", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send_P(200, "text/plain", blindPos.c_str());
  });
  server.on("/dist", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send_P(200, "text/plain", String(distance).c_str());
  });
  server.on("/temp", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send_P(200, "text/plain", myTemp.c_str());
  });
  server.on("/humid", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send_P(200, "text/plain", myHum.c_str());
  });
  server.on("/UP", HTTP_GET, [] (AsyncWebServerRequest *request) {
    isUp = true;
    isDown = false;
    blindPos = "UP";
    request->send(200, "text/plain", blindPos.c_str());
  });
  server.on("/DOWN", HTTP_GET, [] (AsyncWebServerRequest *request) {
    isDown = true;
    isUp = false;
    blindPos = "DOWN";
    request->send(200, "text/plain", blindPos.c_str());
  });
  server.on("/stop", HTTP_GET, [] (AsyncWebServerRequest *request) {
    spinDir = 0;
    blindPos = "NA";
    request->send(200, "text/plain", blindPos.c_str());
  });
  server.on("/tempUpdate", HTTP_GET, [] (AsyncWebServerRequest *request) {
    getTempHum();
    request->send(200, "text/plain", "OK");
  });

  // Send a GET request to <ESP_IP>/update?output=<inputMessage1>&state=<inputMessage2>
  server.on("/update", HTTP_GET, [] (AsyncWebServerRequest *request) {
    String inputMessage1;
    String inputMessage2;
    // GET input1 value on <ESP_IP>/update?output=<inputMessage1>&state=<inputMessage2>
    if (request->hasParam(PARAM_INPUT_1) && request->hasParam(PARAM_INPUT_2)) {
      inputMessage1 = request->getParam(PARAM_INPUT_1)->value();
      inputMessage2 = request->getParam(PARAM_INPUT_2)->value();
      if(inputMessage1 == "autoBlind"){
        isAutoWork = inputMessage2.toInt();
        Serial.print("isAuto = ");
        Serial.println(isAutoWork);
      }
      else if(inputMessage1 == "spinUp"){
        spinDir = inputMessage2.toInt();
        Serial.print("spinDir = ");
        Serial.println(spinDir);
      }
      else if(inputMessage1 == "spinDown"){
        spinDir = (inputMessage2.toInt()) * (-1);
        Serial.print("spinDir = ");
        Serial.println(spinDir);
      }
      else if(inputMessage1 == "relay"){
        ledStripOn = inputMessage2.toInt();
        Serial.print("LED = ");
        Serial.println(ledStripOn);
      }
      if(inputMessage1 == "autoLed"){
        isLedAuto = inputMessage2.toInt();
        Serial.print("isLedAuto = ");
        Serial.println(isLedAuto);
      }
    }
    else {
      inputMessage1 = "No message sent";
      inputMessage2 = "No message sent";
    }

    request->send(200, "text/plain", "OK"); 
  });

  // Start server
  server.begin();
}

void loop() {
  photoResVal = analogRead(fotoResPin) / 4; 

  blindFun();
  ledFun(25, 100, 15000, 1000);
}


//-------------------------------------------
//additional functions
//-------------------------------------------

void getDistance() {
  long time; 
 
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
 
  time = pulseIn(echoPin, HIGH);
  distance = (int) (time / 58);  // operation needed to get a distance in cm 
}

void ledFun(unsigned int a, unsigned int b, unsigned long offTime, unsigned long onTime) {
  // get distance only when there is dark and roller blind is not spinning
  if(spinDir == 0 && photoResVal < fotoResThrDay ){          //<------ comment this for tests
    getDistance();
  }
  if(isLedAuto){
    unsigned long currMillis = millis();
    if(distance > a && distance < b) {
      if(!isPerson) {
        prevMillisStart = currMillis;
        isPerson = true;
      } 
    } 
    else if (isPerson == true) {
      prevMillisEnd = currMillis;
      isPerson = false;
    }

    bool relayPrev = relayNowOn;
    if(isPerson == true && (currMillis - prevMillisStart) >= onTime) {
      relayNowOn = true;
    }
    if(isPerson == false && (currMillis - prevMillisEnd) >= offTime) {
      relayNowOn = false;
    }
    if(relayPrev == false && relayNowOn == true){
      digitalWrite(relayPin, HIGH);
    }
    else if(relayPrev == true && relayNowOn == false){
      digitalWrite(relayPin, LOW);
    }

  }
  else{
    if(ledStripOn){
      digitalWrite(relayPin, HIGH);
    }
    else{
      digitalWrite(relayPin, LOW);
    }
  }
}

void blindFun(){
  if(isAutoWork){
    autoBlind();
    stepsNo = stepper.motorSpinning(spinDir, isAutoWork);
  }
  else {
    if(spinDir == 1 || spinDir == -1){
      stepsNo = stepper.motorSpinning(spinDir, isAutoWork);
    }
    else{
      stepsNo = stepper.motorSpinning(0, isAutoWork);
    }
  }
}

void autoBlind() {
  if((photoResVal < fotoResThrNight) && isUp) {
    spinDir = -1;
    blindPos = "moving down";
  } 
  else if((photoResVal > fotoResThrDay) && isDown) {
    spinDir = 1;
    blindPos = "moving up";
  }
  if(isUp && stepsNo > stepsLimit) {
    isUp = false;
    spinDir = 0;
    stepsNo = 0;
    delay(1500);
    isDown = true;
    blindPos = "DOWN";
  }
  if(isDown && stepsNo > stepsLimit) {
    isDown = false;
    spinDir = 0;
    stepsNo = 0;
    delay(1500);
    isUp = true;
    blindPos = "UP";
  }
}

void getTempHum(){
  if(isAHT && spinDir == 0){
    aht_humidity->getEvent(&humidity);
    aht_temp->getEvent(&temp);
    myTemp = String(temp.temperature - 3);
    myHum = String(humidity.relative_humidity);
    delay(1000);
  }
}
