#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <ArduinoOTA.h>

// Pin Definitions
int in1 = D8;  // Motor A pin 1
int in2 = D7;  // Motor A pin 2
int in3 = D5;  // Motor B pin 1
int in4 = D6;  // Motor B pin 2
const int buzPin = D0;      // Buzzer pin
const int ledPin = D1;      // LED pin
const int wifiLedPin = D2;  // WiFi LED pin

// Global Variables
String command;  // Command from the controller
ESP8266WebServer server(80);  // Webserver object
String ap_ssid = "WiFi_RC_Car";       // Access Point SSID
String ap_password = "12345678";      // Access Point Password

void setup() {
  Serial.begin(115200);
  Serial.println("RC Car WiFi Mode");

  // Pin Setup
  pinMode(buzPin, OUTPUT);
  pinMode(ledPin, OUTPUT);
  pinMode(wifiLedPin, OUTPUT);
  pinMode(in1, OUTPUT);
  pinMode(in2, OUTPUT);
  pinMode(in3, OUTPUT);
  pinMode(in4, OUTPUT);
  
  // Initial States
  digitalWrite(buzPin, LOW);
  digitalWrite(ledPin, LOW);
  digitalWrite(wifiLedPin, HIGH);
  digitalWrite(in1, LOW);
  digitalWrite(in2, LOW);
  digitalWrite(in3, LOW);
  digitalWrite(in4, LOW);

  // WiFi Setup
  WiFi.mode(WIFI_AP);
  WiFi.softAP(ap_ssid.c_str(), ap_password.c_str());
  Serial.print("Access Point started. SSID: ");
  Serial.println(ap_ssid);
  Serial.print("AP IP address: ");
  Serial.println(WiFi.softAPIP());

  // Server Setup
  server.on("/", HTTP_handleRoot);
  server.onNotFound(handleNotFound);
  server.begin();
  Serial.println("Web server started.");

  // OTA Setup
  ArduinoOTA.begin();
  Serial.println("OTA Ready.");
}

void loop() {
  ArduinoOTA.handle();    // Handle OTA requests
  server.handleClient();  // Handle HTTP requests

  // Handle command from URL
  command = server.arg("State");
  if (command == "F") Backward(); //Backwards is the new forward
  else if (command == "B") Forward(); //Forward is the new backward
  else if (command == "R") TurnRight();
  else if (command == "L") TurnLeft();
  else if (command == "S") Stop();
  
}

// Motor Control Functions
void Forward() {
  Serial.println("Moving Forward");
  digitalWrite(in1, HIGH);
  digitalWrite(in2, LOW);
  digitalWrite(in3, HIGH);
  digitalWrite(in4, LOW);
}

void Backward() {
  Serial.println("Moving Backward");
  digitalWrite(in1, LOW);
  digitalWrite(in2, HIGH);
  digitalWrite(in3, LOW);
  digitalWrite(in4, HIGH);
}

void TurnRight() {
  Serial.println("Turning Right");
  digitalWrite(in1, LOW);
  digitalWrite(in2, HIGH);
  digitalWrite(in3, HIGH);
  digitalWrite(in4, LOW);
}

void TurnLeft() {
  Serial.println("Turning Left");
  digitalWrite(in1, HIGH);
  digitalWrite(in2, LOW);
  digitalWrite(in3, LOW);
  digitalWrite(in4, HIGH);
}

void Stop() {
  Serial.println("Stopping");
  digitalWrite(in1, LOW);
  digitalWrite(in2, LOW);
  digitalWrite(in3, LOW);
  digitalWrite(in4, LOW);
}


// HTML Handlers
void HTTP_handleRoot() {
  String html = "<html><body><h1>WiFi RC Car Controller</h1>";
  html += "<p>Use the buttons below to control the car:</p>";
  html += "<a href=\"?State=F\"><button>Forward</button></a>";
  html += "<a href=\"?State=B\"><button>Backward</button></a>";
  html += "<a href=\"?State=L\"><button>Left</button></a>";
  html += "<a href=\"?State=R\"><button>Right</button></a>";
  html += "<a href=\"?State=S\"><button>Stop</button></a>";
  html += "<p>AP IP Address: " + WiFi.softAPIP().toString() + "</p>";
  html += "</body></html>";
  server.send(200, "text/html", html);
}

void handleNotFound() {
  server.send(404, "text/plain", "Command not recognized");
}
