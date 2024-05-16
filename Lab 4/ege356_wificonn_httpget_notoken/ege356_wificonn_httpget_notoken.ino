#include <Arduino.h>
#include <WiFi.h>
#include <M5StickCPlus.h>
#include <HTTPClient.h>


const int buttonA = 37;  //Button-B =39, Button-A = 37
const int buttonB = 39;
int last_valueA = 0;
int last_valueB = 0;
int cur_valueA = 0;
int cur_valueB = 0;

const char* ssid = " ";
const char* password = " ";
const char* devname = " ";
String locIPaddr;

HTTPClient http;

void setup() {
  // init lcd
  M5.begin();
  M5.Lcd.setRotation(3);
  pinMode(buttonA, INPUT);
  pinMode(buttonB, INPUT);
  M5.Lcd.setTextColor(YELLOW);
  M5.Lcd.setCursor(60, 0, 2); M5.Lcd.println("Button example");
  Serial.println("Button example: ");
  M5.Lcd.setTextColor(WHITE);
}

void loop() {
  cur_valueA = digitalRead(buttonA);// read the value of BUTTON_A
  cur_valueB = digitalRead(buttonB);// read the value of BUTTON_B
  
  M5.Lcd.setCursor(40,25, 2); M5.Lcd.print("A-Status: ");
  M5.Lcd.setCursor(40,45, 2); M5.Lcd.print("Value: ");
  M5.Lcd.setCursor(40,65, 2); M5.Lcd.print("B-Status: ");
  M5.Lcd.setCursor(40,85, 2); M5.Lcd.print("Value: ");
  if(cur_valueA != last_valueA){
    M5.Lcd.fillRect(135,25,100,25,BLACK);
    M5.Lcd.fillRect(135,45,100,25,BLACK);
    if(cur_valueA==0){
      M5.Lcd.setCursor(135,25, 2); M5.Lcd.print("pressed");// display the status
      M5.Lcd.setCursor(135,45, 2); M5.Lcd.print("0");
      Serial.println("Button A Status: pressed");
      Serial.println("       value:  0");
      connectToNetwork();
    }
    else{
      M5.Lcd.setCursor(135,25, 2); M5.Lcd.print("released");// display the status
      M5.Lcd.setCursor(135,45, 2); M5.Lcd.print("1");
      Serial.println("Button A Status: released");
      Serial.println("       value:  1");
    }
  }
  if(cur_valueB != last_valueB){
    
    M5.Lcd.fillRect(135,65,100,25,BLACK);
    M5.Lcd.fillRect(135,85,100,25,BLACK);
    if(cur_valueB==0){
      M5.Lcd.setCursor(135,65, 2); M5.Lcd.print("pressed");// display the status
      M5.Lcd.setCursor(135,85, 2); M5.Lcd.print("0");
      Serial.println("Button B Status: pressed");
      Serial.println("       value:  0");
      connectToWeb();
    }
    else{
      M5.Lcd.setCursor(135,65, 2); M5.Lcd.print("released");// display the status
      M5.Lcd.setCursor(135,85, 2); M5.Lcd.print("1");
      Serial.println("Button B Status: released");
      Serial.println("       value:  1");
    }
  }
 
 last_valueA = cur_valueA;
 last_valueB = cur_valueB;
}

void connectToNetwork() {
  WiFi.begin(ssid, password);
  
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Establishing connection to WiFi..");
  }
  M5.Lcd.fillRect(40,105,100,25,BLACK);
  Serial.println("Connected to network");
  Serial.println(WiFi.macAddress());
  Serial.println(WiFi.localIP());
  
  M5.Lcd.setCursor(40,105, 2); M5.Lcd.print(WiFi.localIP());// display the status
}

void connectToWeb(){
    
  String serverAPI = "http://10.100.2.104:8000/devices";

  String serverPath = String(serverAPI + "/" + devname); //"?temperature=24.37";

  http.begin(serverPath.c_str());
      
  // Send HTTP GET request
  int httpResponseCode = http.GET();
      
  if (httpResponseCode>0) {
    Serial.print("HTTP Response code: ");
    Serial.println(httpResponseCode);
    String payload = http.getString();
    Serial.println(payload);
  }
  else {
    Serial.print("Error code: ");
    Serial.println(httpResponseCode);
  }
  // Free resources
  http.end();
}
