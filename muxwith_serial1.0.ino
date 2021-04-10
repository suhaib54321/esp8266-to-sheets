/*
 * components used
 * nodemcu
 * dht11
 * cd74hc4067
 *
 * ESP822 temprature logging to Google Sheet      
 * CircuitDigest(www.circuitdigest.com)
*/
#include <Wire.h>
#include <ESP8266WiFi.h>
#include "HTTPSRedirect.h"
#include "DebugMacros.h"
#include <DHT.h>

#define analogpin A0
#define DHTPIN D3                                                           // what digital pin we're connected to
#define DHTTYPE DHT11                                                       // select dht type as DHT 11 or DHT22
DHT dht(DHTPIN, DHTTYPE);

float h;
float t;
float m1;
float m2;
float m3;
float m4;

String sheetHumid = "";
String sheetTemp = "";
String sheetMoisture = "";

const char* ssid = "ssid";                //replace with our wifi ssid
const char* password = "password";         //replace with your wifi password

const char* host = "script.google.com";//https://script.google.com/macros/s/AKfycbzRkQgpoXkttRHW31ARGf0CkuDdrAGny5Uiw/exec
const char *GScriptId = "AKfycbzRkQgpoXkttRHW31ARGf0CkuDdrAGny5Uiw"; // Replace with your own google script id
const int httpsPort = 443; //the https port is sameM

// echo | openssl s_client -connect script.google.com:443 |& openssl x509 -fingerprint -noout
const char* fingerprint = "";

//const uint8_t fingerprint[20] = {};

//String url = String("/macros/s/") + GScriptId + "/exec?value=Date";  // Write Teperature to Google Spreadsheet at cell A1
// Fetch Google Calendar events for 1 week ahead
String url2 = String("/macros/s/") + GScriptId + "/exec?cal";  // Write to Cell A continuosly

//replace with sheet name not with spreadsheet file name taken from google
String payload_base =  "{\"command\": \"appendRow\", \
                    \"sheet_name\": \"TempSheet\", \
                       \"values\": ";
String payload = "";

HTTPSRedirect* client = nullptr;

// used to store the values of free stack and heap before the HTTPSRedirect object is instantiated
// so that they can be written to Google sheets upon instantiation

void setup() {
  pinMode(D1, OUTPUT);
  pinMode(D2, OUTPUT);
  //delay(1000);
  Serial.begin(115200);
  dht.begin();     
  Serial.println();
  Serial.print("Connecting to wifi: ");
  Serial.println(ssid);
  
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());

  // Use HTTPSRedirect class to create a new TLS connection
  client = new HTTPSRedirect(httpsPort);
  client->setInsecure();
  client->setPrintResponseBody(true);
  client->setContentTypeHeader("application/json");
  Serial.print("Connecting to ");
  Serial.println(host);          //try to connect with "script.google.com"

  // Try to connect for a maximum of 5 times then exit
  bool flag = false;
  for (int i = 0; i < 5; i++) {
    int retval = client->connect(host, httpsPort);
    if (retval == 1) {
      flag = true;
      break;
    }
    else
      Serial.println("Connection failed. Retrying...");
  }

  if (!flag) {
    Serial.print("Could not connect to server: ");
    Serial.println(host);
    Serial.println("Exiting...");
    return;
  }
// Finish setup() function in 1s since it will fire watchdog timer and will reset the chip.
//So avoid too many requests in setup()

 /* Serial.println("\nWrite into cell 'A1'");
  Serial.println("------>");
  // fetch spreadsheet data
  client->GET(url, host);
  
  Serial.println("\nGET: Fetch Google Calendar Data:");
  Serial.println("------>");
  // fetch spreadsheet data
  client->GET(url2, host);

 Serial.println("\nStart Sending Sensor Data to Google Spreadsheet");
*/
  
  // delete HTTPSRedirect object
  delete client;
  client = nullptr;
}

void loop() {
  
  h = dht.readHumidity();                                              // Reading temperature or humidity takes about 250 milliseconds!
  t = dht.readTemperature();  // Read temperature as Celsius (the default)
  digitalWrite(D1, LOW); 
  digitalWrite(D2, LOW);
  delay(1);
  m1 = analogRead(A0);
  delay(1);
  digitalWrite(D1, LOW); 
  digitalWrite(D2, HIGH);
  delay(1);
  m2 = analogRead(A0);
  delay(1);
  digitalWrite(D1, HIGH); 
  digitalWrite(D2, LOW);
  delay(1);
  m3 = analogRead(A0);
  delay(1);
  digitalWrite(D1, HIGH); 
  digitalWrite(D2, HIGH);
  delay(1);
  m4 = analogRead(A0);
  delay(1);

  
 /* if (isnan(h) || isnan(t)) {                                                // Check if any reads failed and exit early (to try again).
    Serial.println(F("Failed to read from DHT sensor!"));
    return;
  }*/
  Serial.print("Humidity: ");  Serial.print(h);
  sheetHumid = String(h) ;                                         //convert integer humidity to string humidity
  Serial.print("%  Temperature: ");  Serial.print(t);  Serial.println("°C ");
  sheetTemp = String(t) ;

  static int error_count = 0;
  static int connect_count = 0;
  const unsigned int MAX_CONNECT = 20;
  static bool flag = false;

  payload = payload_base + "\""+ +","+ +"," + h + "," + t + "," + m1 + "," + m2 + "," + m3 + "," + m4 + "\"}";   //json formatting
  Serial.println(m1); Serial.println(m2); Serial.println(m3); Serial.println(m4);

  if (!flag) {
    client = new HTTPSRedirect(httpsPort);
    client->setInsecure();
    flag = true;
    client->setPrintResponseBody(true);
    client->setContentTypeHeader("application/json");
  }

  
   if (client != nullptr) {
    if (!client->connected()) {
      client->connect(host, httpsPort);
      client->POST(url2, host, payload, false);
      Serial.print("Sent : ");  Serial.println("Temp Humid and Moist");
    }
  }
  else {
    DPRINTLN("Error creating client object!");
    error_count = 5;
  }

  if (connect_count > MAX_CONNECT) {
    connect_count = 0;
    flag = false;
    delete client;
    return;
  }

//  Serial.println("GET Data from cell 'A1':");
//  if (client->GET(url3, host)) {
//    ++connect_count;
//  }
//  else {
//    ++error_count;
//    DPRINT("Error-count while connecting: ");
//    DPRINTLN(error_count);
//  }

  Serial.println("SENDing Sensor data to Google Spreadsheet:");
  
 if (client->POST(url2, host, payload)) {
    ;
  }
  
  else {
    ++error_count;
    DPRINT("Error-count while connecting: ");
    DPRINTLN(error_count);
  } 

  if (error_count > 3) {   //comment out this portion if you don't want deepsleep after 3
    Serial.println("Halting processor...");
    delete client;
    client = nullptr;
    Serial.printf("Final free heap: %u\n", ESP.getFreeHeap());
    Serial.printf("Final stack: %u\n", ESP.getFreeContStack());
    Serial.flush();
    ESP.deepSleep(3000); 
  }
  
  //delay(3000);    // keep delay of minimum 2 seconds as dht allow reading after 2 seconds interval and also for google sheet
 Serial.flush();
// ESP.deepSleep(3000);  uncomment to repat the process after deep sleep
}
