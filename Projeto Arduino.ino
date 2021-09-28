#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <Hash.h>
#include <ESPAsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <Adafruit_Sensor.h>
#include <DHT.h>

const char* ssid = "Senha do Wi-fi 2.4Ghz";
const char* password = "agoraeusei";

#define DHTPIN 5

#define DHTTYPE DHT22 

DHT dht(DHTPIN,DHTTYPE);

float t = 0.0;
float h = 0.0;
int p = analogRead(A0);

AsyncWebServer server(80);

unsigned long previousMillis = 0;

const long interval = 100; // -> tempo de atualização

const char index_html[] PROGMEM = R"rawliteral(
<!DOCTYPE HTML><html>
<head>
  <meta name="viewport" content="width=device-width, initial-scale=1">
  <link rel="stylesheet" href="https://use.fontawesome.com/releases/v5.7.2/css/all.css" integrity="sha384-fnmOCqbTlWIlj8LyTjo7mOUStjsKC4pOpQbqyi7RrhN7udi9RwhKkMHpvLbHG9Sr" crossorigin="anonymous">
  <style>
 /*   html {
     font-family: Arial;
     display: inline-block;
     margin: 0px auto;
     text-align: center;
    }
    h2 { font-size: 3.0rem; }
    p { font-size: 3.0rem; }
    .units { font-size: 1.2rem; }
    .dht-labels{
      font-size: 1.5rem;
      vertical-align:middle;
      padding-bottom: 15px;
    }*/
  
  </style>
</head>
<body>
  <h2>ESP8266 DHT Server</h2>
  <p>
   
    <span class="dht-labels">Temperature</span> 
    <span id="temperature">%TEMPERATURE%</span>
   
  </p>
  <p>
    
    <span class="dht-labels">Humidity</span>
    <span id="humidity">%HUMIDITY%</span>
    <sup class="units">%</sup>
  </p>
   <p>
   
    <span class="dht-labels">Potenciometro</span>
    <h2 id="potenciometro" >%POTENCIOMETRO%</h2>
   
  </p>
  
</body>
<script>

setInterval(function ( ) {
  var xhttp = new XMLHttpRequest();
  xhttp.onreadystatechange = function() {
    if (this.readyState == 4 && this.status == 200) {
      document.getElementById("temperature").innerHTML = this.responseText;
    }
  };
  xhttp.open("GET", "/temperature", true);
  xhttp.send();
}, 1000 ) ;

setInterval(function ( ) {
  var xhttp = new XMLHttpRequest();
  xhttp.onreadystatechange = function() {
    if (this.readyState == 4 && this.status == 200) { // Quando a requisição é feita com sucesso
      document.getElementById("humidity").innerHTML = this.responseText; // o elemento é atulizado
    }
  };
  xhttp.open("GET", "/humidity", true); // Solicita a url o valor mais recente
  xhttp.send();
}, 1000 ) ;

setInterval(function ( ) {
  var xhttp = new XMLHttpRequest();
  xhttp.onreadystatechange = function() {
    if (this.readyState == 4 && this.status == 200) {
      document.getElementById("potenciometro").innerHTML = this.responseText;
    }
  };
  xhttp.open("GET", "/potenciometro", true);
  xhttp.send();
}, 1000 ) ;
</script>
</html>)rawliteral";

// Replaces placeholder with DHT values
String processor(const String& var){ // Função que subistitui os espaços reservados em HTML
  //Serial.println(var);
  if(var == "TEMPERATURE"){
    return String(t);
  }
  else if(var == "HUMIDITY"){
    return String(h);
  }else if(var == "POTENCIOMETRO"){
    return  String(p);;
  }
  return String();
}

void setup(){
  // Serial port for debugging purposes
  Serial.begin(115200);
  dht.begin();
  
  // Connect to Wi-Fi
  WiFi.begin(ssid, password);
  Serial.println("Connecting to WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println(".");
  }

  // Print ESP8266 Local IP Address
  Serial.println(WiFi.localIP());

  // Route for root / web page
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send_P(200, "text/html", index_html, processor);
  });
  server.on("/temperature", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send_P(200, "text/plain", String(t).c_str());
  });
  server.on("/humidity", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send_P(200, "text/plain", String(h).c_str());
  });
  server.on("/potenciometro", HTTP_GET, [](AsyncWebServerRequest *request){
  request->send_P(200, "text/plain", String(p).c_str());
});
  
  // Start server
  server.begin();
}
 
void loop(){  
  unsigned long currentMillis = millis();
 // 
 // Serial.println(p);
  if (currentMillis - previousMillis >= interval) {
    // save the last time you updated the DHT values
    previousMillis = currentMillis;
    // Read temperature as Celsius (the default)
    float newT = dht.readTemperature();
    // Read temperature as Fahrenheit (isFahrenheit = true)
    //float newT = dht.readTemperature(true);
    // if temperature read failed, don't change t value
    if (isnan(newT)) {
      Serial.println("Failed to read from DHT sensor!");
    }
    else {
      t = newT;
      Serial.println(t);
    }
    // Read Humidity
    float newH = dht.readHumidity();
    // if humidity read failed, don't change h value 
    if (isnan(newH)) {
      Serial.println("Failed to read from DHT sensor!");
    }
    else {
      h = newH;
      Serial.println(h);
    }

     int newP = analogRead(A0);
    // if humidity read failed, don't change h value 
    if (isnan(newP)) {
      Serial.println("Failed to read from P sensor!");
    }
    else {
      p = newP;
      Serial.println(p);
    }
  }
}
