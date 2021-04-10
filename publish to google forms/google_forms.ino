

#include <FCGF.h>
#include <DHT.h>
#include <Wire.h>
#define analogpin A0
String h;
String t;
String m1;
String m2;
String m3;
String m4;
#define DHTPIN D3                                                           // what digital pin we're connected to
#define DHTTYPE DHT11                                                       // select dht type as DHT 11 or DHT22
DHT dht(DHTPIN, DHTTYPE);
//IPAddress ip(192,168,137,14); 
//IPAddress gateway(192,168,137,1);
//IPAddress subnet(255,255,255,0);

GoogleForm gf;


const char* your_wifi_ssid = "ssid";
const char* your_wifi_password = "password";

int num_of_inputs = 6;
String myform_privateid = "1GceRLagPXevsnww2XSICW7zGgzzgcbwunY";
String myform_inputs[] = {"entry.136476213","entry.9624772","entry.4397790","entry.13880923","entry.1920479","entry.709276"};

void setup() {
 
  FCGF_DEBUG = true;

  pinMode(D1, OUTPUT);
  pinMode(D2, OUTPUT);
  //delay(1000);
  Serial.begin(115200);
  dht.begin();     
  
  Serial.begin(115200); 
  
  //Start Wi-Fi and Secure client for your specific board and inizialize Google Form Lib

  gf.beginWiFi(your_wifi_ssid,your_wifi_password);
}

void loop() {   
   h = dht.readHumidity();                                            
  t = dht.readTemperature();  
  digitalWrite(D1, LOW); 
  digitalWrite(D2, LOW);
  delay(10);
  m1 = analogRead(A0);
  delay(0);
  digitalWrite(D1, LOW); 
  digitalWrite(D2, HIGH);
  delay(10);
  m2 = analogRead(A0);
  delay(0);
  digitalWrite(D1, HIGH); 
  digitalWrite(D2, LOW);
  delay(10);
  m3 = analogRead(A0);
  delay(0);
  digitalWrite(D1, HIGH); 
  digitalWrite(D2, HIGH);
  delay(10);
  m4 = analogRead(A0);
  delay(0);
  String myform_values[] = {t,h,m1,m2,m3,m4};
  
  
  gf.submit(myform_privateid,myform_inputs,myform_values,num_of_inputs);
  
  //Finish code
}
