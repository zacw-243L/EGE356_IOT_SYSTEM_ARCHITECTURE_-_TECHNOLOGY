// Adafruit IO Analog In Example
// Tutorial Link: https://learn.adafruit.com/adafruit-io-basics-analog-input
//
// Adafruit invests time and resources providing this open source code.
// Please support Adafruit and open source hardware by purchasing
// products from Adafruit!
//
// Written by Todd Treece for Adafruit Industries
// Copyright (c) 2016 Adafruit Industries
// Licensed under the MIT license.
//
// All text above must be included in any redistribution.

/************************** Configuration ***********************************/

// edit the config.h tab and enter your Adafruit IO credentials
// and any additional configuration needed for WiFi, cellular,
// or ethernet clients.
#include "config.h"
#include <M5StickCPlus.h>
/************************ Example Starts Here *******************************/

// analog pin 0
#define LED_PIN 26

float temp = 0.00;
float last_temp = -1.00;
float speed;

float alert_temp = 55.0;
float last_alert_temp = 55.0;

int alert1 = 0;
int last_alert1 = 0;

int current = 0;
int last = -1;

// set up the 'analog' feed
AdafruitIO_Feed *analog_ltemp = io.feed("lab3_ltemp");
AdafruitIO_Feed *analog_led = io.feed("lab2_ledctrl");
AdafruitIO_Feed *analog_lgage = io.feed("lab2_lingage");
AdafruitIO_Feed *d_alert1 = io.feed("lab3_alert1");
AdafruitIO_Feed *set_th = io.feed("lab3_set_th");

void setup() {
  M5.begin();
  M5.IMU.Init();
  M5.Lcd.setRotation(3);
  M5.Lcd.fillScreen(BLACK);
  M5.Lcd.setTextSize(1);
  M5.Lcd.setCursor(40, 15);
  M5.IMU.getTempData(&temp);
  
  // set up led pin as an analog output
  #if defined(ARDUINO_ARCH_ESP32)
    #if ESP_IDF_VERSION >= ESP_IDF_VERSION_VAL(5, 1, 1)
      // New ESP32 LEDC API
      ledcAttach(LED_PIN, 12000, 8); // 12 kHz PWM, 8-bit resolution
    #else
      // Legacy ESP32 LEDC API
      ledcAttachPin(LED_PIN, 1);
      ledcSetup(1, 1200, 8);
    #endif
  #else
    pinMode(LED_PIN, OUTPUT);
  #endif
  
  // start the serial connection
  Serial.begin(115200);
  Serial.println();
  Serial.print("ESP Board MAC Address:  ");
  Serial.println(WiFi.macAddress());

  // wait for serial monitor to open
  while(! Serial);

  // connect to io.adafruit.com
  Serial.print("Connecting to Adafruit IO");
  io.connect();
  
  // set up a message handler for the 'analog' feed.
  // the handleMessage function (defined below)
  // will be called whenever a message is
  // received from adafruit io.
  analog_led->onMessage(handleMessage);
  set_th->onMessage(set_th_pt);
  d_alert1->onMessage(ALERT);

  // wait for a connection
  while(io.status() < AIO_CONNECTED) {
    Serial.print(".");
    delay(500);
  }

  M5.Lcd.setCursor(30,40,2);
  M5.Lcd.print(WiFi.localIP()); //display ip address
  
  // we are connected
  Serial.println();
  Serial.println(io.statusText());
  Serial.println(WiFi.localIP());
  analog_led->get();

}

void loop(){
  // io.run(); is required for all sketches.
  // it should always be present at the top of your loop
  // function. it keeps the client connected to
  // io.adafruit.com, and processes any incoming data.
  io.run();
  M5.IMU.getTempData(&temp);
    
  // return if the value hasn't changed
  if(temp == last_temp)
    return;
  last_temp = temp;

  Serial.print("sending temperature -> ");
  Serial.println(temp);
  analog_ltemp->save(temp);
  // wait three seconds (1000 milliseconds == 1 second)
  
  // because there are no active subscriptions, we can use delay()
  // instead of tracking millis()
  M5.Lcd.setCursor(30, 95);
  M5.Lcd.printf("Temperature : %.2f C", temp);
  
  delay(6000);
}

void handleMessage(AdafruitIO_Data *data) {
  // convert the data to integer
  int reading = data->toInt();

  Serial.print("received <- ");
  Serial.println(reading);
  current = reading; 
  d_alert1->save(current);
  
  float speed = current/255.0*100;
  
  analog_lgage->save(speed);
  Serial.print("sending <- ");
  Serial.println(speed);
 
  // write the current 'reading' to the led
  analogWrite(LED_PIN, reading);
}

void set_th_pt(AdafruitIO_Data *data){
  int new_set_th = data->toInt();
  
  Serial.print("received <- ");
  Serial.println(new_set_th);
  alert_temp = new_set_th;
  Serial.print("setting temp threshold -> ");
  Serial.println(alert_temp);
  
}

void ALERT (AdafruitIO_Data *data){
  int new_ALERT = data->toInt();

  if (speed > 80.0 && temp > alert_temp) 
    analog_led->save(80);
    int a = 80;
    Serial.print("Ledctrl-> ");
    Serial.println(a);
    // alert1 = 1;
    // Serial.print("sending Alert1 -> ");
    // Serial.println(alert1);
    d_alert1->save(current);
    
  if (speed > 80.0 && temp > alert_temp && alert1 == 1)
    d_alert1->save(90);
    alert1 = 0;
    Serial.print("sending Alert1 -> ");
    Serial.println(alert1);
  
  if (temp < alert_temp)
    alert1 = 0;
    Serial.print("sending Alert1 -> ");
    Serial.println(alert1);
    d_alert1->save(current);
}
