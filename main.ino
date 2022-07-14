#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>
#include <ESPAsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <SoftwareSerial.h>

SoftwareSerial mySerial(5, 4); //SIM800L Tx & Rx is connected to Wemos D1 & D2

IPAddress    apIP(72, 72, 72, 72);  // Defining a static IP address

const char* ssid     = "test";
const char* password = "123456789";

String webPage1,PhoneNumber;
bool PythonBoolReturn;

String IsNumber = "";

int output_value_lock = 0;
int socket_data = 0;
const int ledPin_lockPin = 2;
int password_state_typing = 0;
int password_state_wrong = 0;
int password_key = 0;
int Dcode = 654321;

//ESP8266WebServer server(80);
  
// Create AsyncWebServer object on port 80
AsyncWebServer server(80);
AsyncWebSocket ws("/ws");

////////////////////////////////////////////

const char index_html[] PROGMEM = R"rawliteral(
<!DOCTYPE HTML><html>
<head>
  <title>Wifi lock</title>
  <meta name="viewport" content="width=device-width, initial-scale=1">
  <link rel="icon" href="data:,">
  <style>
  html {
    font-family: Arial, Helvetica, sans-serif;
    text-align: center;
  }
  h1 {
    font-size: 1.8rem;
    color: white;
  }
  h2{
    font-size: 1.5rem;
    font-weight: bold;
    color: #143642;
  }
  .topnav {
    overflow: hidden;
    background-color: #143642;
  }
  body {
    margin: 0;
  }
  .content {
    padding: 30px;
    max-width: 600px;
    margin: 0 auto;
  }
  .card {
    background-color: #F8F7F9;;
    box-shadow: 2px 2px 12px 1px rgba(140,140,140,.5);
    padding-top:10px;
    padding-bottom:20px;
  }
  .button {
    padding: 15px 35px;
    font-size: 24px;
    text-align: center;
    outline: none;
    color: #fff;
    background-color: #0f8b8d;
    border: none;
    border-radius: 30px;
    -webkit-touch-callout: none;
    -webkit-user-select: none;
    -khtml-user-select: none;
    -moz-user-select: none;
    -ms-user-select: none;
    user-select: none;
    -webkit-tap-highlight-color: rgba(0,0,0,0);
   }
   .button2 {
    padding: 15px 30px;
    font-size: 24px;
    text-align: center;
    outline: none;
    color: #fff;
    background-color: #FF2D00;
    border: none;
    border-radius: 30px;
    -webkit-touch-callout: none;
    -webkit-user-select: none;
    -khtml-user-select: none;
    -moz-user-select: none;
    -ms-user-select: none;
    user-select: none;
    -webkit-tap-highlight-color: rgba(0,0,0,0);
   }
   /*.button:hover {background-color: #0f8b8d}*/
   .button:active {
     background-color: #0f8b8d;
     box-shadow: 2 2px #CDCDCD;
     transform: translateY(8px);
   }
   .button2:active {
     background-color: #0f8b8d;
     box-shadow: 2 2px #CDCDCD;
     transform: translateY(8px);
   }
   .state {
     font-size: 1.5rem;
     color:#8c8c8c;
     font-weight: bold;
   }
  </style>
<title>Wifi lock</title>
<meta name="viewport" content="width=device-width, initial-scale=1">
<link rel="icon" href="data:,">
</head>
<body>
  <div class="topnav">
    <h1>Engineer2you Home</h1>
  </div>
  <div class="content">
    <div class="card">
      <h2>DOOR PASSWORD</h2>
      <p class="state"><span id="state_password">%STATE1%</span></p>
      <button id="button1" class="button">1</button>     
      <button id="button2" class="button">2</button>
      <button id="button3" class="button">3</button>
      <p></p>
      <button id="button4" class="button">4</button>
      <button id="button5" class="button">5</button>
      <button id="button6" class="button">6</button>
      <p></p>
      <button id="button7" class="button">7</button>
      <button id="button8" class="button">8</button>
      <button id="button9" class="button">9</button>
      <p></p>
      <button id="button_clear" class="button">X</button>
      <button id="button0" class="button">0</button>
      <button id="button_enter" class="button2">|] [|</button>
      <p class="state"><span id="state_door">%STATE2%</span></p>

      
    </div>
  </div>
<script>
  var gateway = `ws://${window.location.hostname}/ws`;
  var websocket;
  window.addEventListener('load', onLoad);
  function initWebSocket() {
    console.log('Trying to open a WebSocket connection...');
    websocket = new WebSocket(gateway);
    websocket.onopen    = onOpen;
    websocket.onclose   = onClose;
    websocket.onmessage = onMessage;

  }
  function onOpen(event) {
    console.log('Connection opened');
  }
  function onClose(event) {
    console.log('Connection closed');
    setTimeout(initWebSocket, 2000);
  }
  function onMessage(event) {
    //-----------------------------------------use for update state of button to web when press button
    // this function is called when "void notify_to_Clients()" is run
    var password_state_out_html;
    var socket_data_receive;
    var password_status;
    var password_status_wrong;
    var door_status;
    var door_state_out_html;

    socket_data_receive = event.data; //get data from "void notify_to_Clients()"

    password_status = event.data%(10);
    password_status_wrong = (event.data/(1000)).toFixed(0);
    door_status = (event.data/(100)).toFixed(0);
    
    switch (password_status){
      case 0: password_state_out_html = "[ ][ ][ ][ ][ ][ ]";
      break;
      case 1: password_state_out_html =  "[*][ ][ ][ ][ ][ ]";
      break;
      case 2: password_state_out_html =  "[*][*][ ][ ][ ][ ]";
      break;
      case 3: password_state_out_html =  "[*][*][*][ ][ ][ ]";
      break;
      case 4: password_state_out_html =  "[*][*][*][*][ ][ ]";
      break;
      case 5: password_state_out_html =  "[*][*][*][*][*][ ]";
      break;
      case 6: password_state_out_html =  "[*][*][*][*][*][*]";
      break;
      default: password_state_out_html =  "[*][*][*][*][*][*]>";
      break;
    }

    if (door_status == 0){
      door_state_out_html = "OPEN";
    }
    else {
      door_state_out_html = "CLOSE";
    }

    if (password_status_wrong == 1){
      document.getElementById('state_password').innerHTML = "WRONG PASSWORD!";
    }
    else {
      document.getElementById('state_password').innerHTML = password_state_out_html;
    }

    
    document.getElementById('state_door').innerHTML = door_state_out_html;
  }

  function onLoad(event) {
    initWebSocket();
    initButton1();
    initButton2();
    initButton3();
    initButton4();
    initButton5();
    initButton6();
    initButton7();
    initButton8();
    initButton9();
    initButton0();
    initButton_clear();
    initButton_enter();
  }
  function initButton1() {
    document.getElementById('button1').addEventListener('click', click1);
  }
  function click1(){
    websocket.send('click_command1');
  }
  function initButton2() {
    document.getElementById('button2').addEventListener('click', click2);
  }
  function click2(){
    websocket.send('click_command2');
  }
  function initButton3() {
    document.getElementById('button3').addEventListener('click', click3);
  }
  function click3(){
    websocket.send('click_command3');
  }
  function initButton4() {
    document.getElementById('button4').addEventListener('click', click4);
  }
  function click4(){
    websocket.send('click_command4');
  }
  function initButton5() {
    document.getElementById('button5').addEventListener('click', click5);
  }
  function click5(){
    websocket.send('click_command5');
  }
  function initButton6() {
    document.getElementById('button6').addEventListener('click', click6);
  }
  function click6(){
    websocket.send('click_command6');
  }
  function initButton7() {
    document.getElementById('button7').addEventListener('click', click7);
  }
  function click7(){
    websocket.send('click_command7');
  }
  function initButton8() {
    document.getElementById('button8').addEventListener('click', click8);
  }
  function click8(){
    websocket.send('click_command8');
  }
  function initButton9() {
    document.getElementById('button9').addEventListener('click', click9);
  }
  function click9(){
    websocket.send('click_command9');
  }
  function initButton0() {
    document.getElementById('button0').addEventListener('click', click0);
  }
  function click0(){
    websocket.send('click_command0');
  }
  function initButton_clear() {
    document.getElementById('button_clear').addEventListener('click', click_clear);
  }
  function click_clear(){
    websocket.send('click_command_clear');
  }
  function initButton_enter() {
    document.getElementById('button_enter').addEventListener('click', click_enter);
  }
  function click_enter(){
    websocket.send('click_command_enter');
  }
</script>
</body>
</html>
)rawliteral";

void notify_to_Clients() {
  ws.textAll(String(socket_data));
  //this function will call "function onMessage(event)" to run
}

void handleWebSocketMessage(void *arg, uint8_t *data, size_t len) {
  AwsFrameInfo *info = (AwsFrameInfo*)arg;
  if (info->final && info->index == 0 && info->len == len && info->opcode == WS_TEXT) {
    data[len] = 0;
    if (strcmp((char*)data, "click_command1") == 0) {
      Serial.println("just press button1");
      password_state_typing += 1;
      password_key += 1*pow(10, password_state_typing-1);
      password_state_wrong = 0;
    }
    if (strcmp((char*)data, "click_command2") == 0) {
      Serial.println("just press button2");
      password_state_typing += 1;
      password_key += 2*pow(10, password_state_typing-1);
      password_state_wrong = 0;
    }
    if (strcmp((char*)data, "click_command3") == 0) {
      Serial.println("just press button3");
      password_state_typing += 1;
      password_key += 3*pow(10, password_state_typing-1);
      password_state_wrong = 0;
    }
    if (strcmp((char*)data, "click_command4") == 0) {
      Serial.println("just press button4");
      password_state_typing += 1;
      password_key += 4*pow(10, password_state_typing-1);
      password_state_wrong = 0;
    }
    if (strcmp((char*)data, "click_command5") == 0) {
      Serial.println("just press button5");
      password_state_typing += 1;
      password_key += 5*pow(10, password_state_typing-1);
      password_state_wrong = 0;
    }
    if (strcmp((char*)data, "click_command6") == 0) {
      Serial.println("just press button6");
      password_state_typing += 1;
      password_key += 6*pow(10, password_state_typing-1);
      password_state_wrong = 0;
    }
    if (strcmp((char*)data, "click_command7") == 0) {
      Serial.println("just press button7");
      password_state_typing += 1;
      password_key += 7*pow(10, password_state_typing-1);
      password_state_wrong = 0;
    }
    if (strcmp((char*)data, "click_command8") == 0) {
      Serial.println("just press button8");
      password_state_typing += 1;
      password_key += 8*pow(10, password_state_typing-1);
      password_state_wrong = 0;
    }
    if (strcmp((char*)data, "click_command9") == 0) {
      Serial.println("just press button9");
      password_state_typing += 1;
      password_key += 9*pow(10, password_state_typing-1);
      password_state_wrong = 0;
    }
    if (strcmp((char*)data, "click_command0") == 0) {
      Serial.println("just press button0");
      password_state_typing += 1;
      password_key += 0*pow(10, password_state_typing-1);
      password_state_wrong = 0;
    }
    if (strcmp((char*)data, "click_command_clear") == 0) {
      Serial.println("just press button0");
      password_state_typing = 0;
      password_key = 0;
      password_state_wrong = 0;
    }
    if (strcmp((char*)data, "click_command_enter") == 0) {
      Serial.println("just press enter");
      if (output_value_lock == 0) {
        output_value_lock = 1;
        password_state_typing = 0;
        password_key = 0;
        password_state_wrong = 0;
      }
      else{    
        if (password_key == Dcode){
          output_value_lock = 0;
          password_state_typing = 0;
          password_key = 0;
          password_state_wrong = 0;
        }
        else{
          password_state_wrong = 1;
          password_key = 0;
        }
      }
    }

    if (password_state_typing > 9) password_state_typing = 0;

    socket_data = 0;
    socket_data += password_state_typing;
    socket_data += output_value_lock*100;
    socket_data += password_state_wrong*1000;
    Serial.println(socket_data);
    Serial.println(password_key);
    
    notify_to_Clients(); 
  }
}

void onEvent(AsyncWebSocket *server, AsyncWebSocketClient *client, AwsEventType type,
             void *arg, uint8_t *data, size_t len) {
    switch (type) {
      case WS_EVT_CONNECT:
        Serial.printf("WebSocket client #%u connected from %s\n", client->id(), client->remoteIP().toString().c_str());
        break;
      case WS_EVT_DISCONNECT:
        Serial.printf("WebSocket client #%u disconnected\n", client->id());
        break;
      case WS_EVT_DATA:
        handleWebSocketMessage(arg, data, len);
        break;
      case WS_EVT_PONG:
      case WS_EVT_ERROR:
        break;
  }
}

void initWebSocket() {
  ws.onEvent(onEvent);
  server.addHandler(&ws);
}

String processor(const String& var){
  //use for update state of LED at first connection
  Serial.println(var);
  if(var == "STATE1"){
    switch (password_state_typing){
      case 0: return "[ ][ ][ ][ ][ ][ ]";
      break;
      case 1: return "[*][ ][ ][ ][ ][ ]";
      break;
      case 2: return "[*][*][ ][ ][ ][ ]";
      break;
      case 3: return "[*][*][*][ ][ ][ ]";
      break;
      case 4: return "[*][*][*][*][ ][ ]";
      break;
      case 5: return "[*][*][*][*][*][ ]";
      break;
      case 6: return "[*][*][*][*][*][*]";
      break;
      default: return "[*][*][*][*][*][*]>";
      break;
    }
  }

  if(var == "STATE2"){
    if (output_value_lock){
      return "CLOSE";
    }
    else{
      return "OPEN";
    }
  }
}

////////////////////////////////////////////



const char htmlPage1[]PROGMEM=R"=====(
  <!DOCTYPE html>
<html>
<body>
<h3>Smart-Door-Lock</h3>

<FORM METHOD="POST" action= 72.72.72.72/unlock>

<input type="text" name="myText" value="Write your Phone Number...">

<p>Click the "Submit" button to verify your phone number :) </p>

<input type="submit" value="Submit" onclick="myFunction()">

<p id="demo"></p>

<script>
var x;
function myFunction() {
     x = document.getElementById("myText").value  
     
}
)=====";

void handleRoot(AsyncWebServerRequest *request)
{
  webPage1=htmlPage1;
  PhoneNumber = request->arg("myText");
  srand(millis());
  int code=rand()%1000000;
  Dcode = code;
  if(IsNumber == "1") SendSMS(PhoneNumber,String(code));
  Serial.print("Argument Received");
}

void handleVerify(AsyncWebServerRequest *request)
{
  request->send(200, "text/plain", PhoneNumber);
}

void handleNotFound(AsyncWebServerRequest *request)
{
    request->send(404, "text/plain", "Not Found!");
}

void manageAPI(){

  server.on("/",handleRoot);  //get phone number frome user
  
  server.on("/unlock", HTTP_GET, [](AsyncWebServerRequest *request){
    if(IsNumber == "1") request->send_P(200, "text/html", index_html, processor);
    else request->send(200, "text/plane","forbiden!");
  });
  
  server.on("/verify", handleVerify); //number page
  
  server.on("/IsNumber",[](AsyncWebServerRequest *request) {      //get details from data base
    request->send(200, "text/plane","Succeed");
    IsNumber = request->arg("0");
  });
  
  server.onNotFound(handleNotFound);
}


void SendSMS(String number,String text)
{
    
    // Can take up to 60 seconds
    mySerial.print(F("AT+CMGF=1\r"));   //set sms to text mode
    ReceiveFromSim();
    mySerial.println("AT+CSMP=17,167,0,0");
    ReceiveFromSim();
    
    mySerial.print(F("AT+CMGS=\""));    // command to send sms
    mySerial.print(number);
    mySerial.print(F("\"\r"));
    ReceiveFromSim();
    
    mySerial.print(text);
    mySerial.print("\r");
    ReceiveFromSim();
    
    mySerial.print((char)26);
    // try to read result from sim800 (max response 60s)
}

String ReceiveFromSim()
{
  int _timeout=0;
  while  (!mySerial.available() && _timeout < 1200  ) 
  {
    delay(10);
    _timeout++;
  }
  if (mySerial.available()) {
    return mySerial.readString();   
  }  
}




void setup()
{
  Serial.begin(9600);

  pinMode(ledPin_lockPin, OUTPUT);
  digitalWrite(ledPin_lockPin, LOW);
  
  WiFi.mode(WIFI_AP_STA);
  WiFi.softAPConfig(apIP, apIP, IPAddress(255, 255, 255, 0));
  
  WiFi.softAP(ssid, password);

  IPAddress myIP = WiFi.softAPIP();
  Serial.print("AP IP address: ");
  Serial.println(myIP);

  initWebSocket();
  manageAPI();

  server.begin();
  Serial.println("HTTP Server Started");    

}
void loop() {
   //server.handleClient();
   ws.cleanupClients();
  // put your main code here, to run repeatedly:

}
