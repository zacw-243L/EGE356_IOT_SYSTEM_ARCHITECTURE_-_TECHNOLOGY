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
#define TRIMMER_PIN 33
#define LED_PIN 26

// photocell state
int current = 0;
int last = -1;

// set up the 'analog' feed
AdafruitIO_Feed *analog = io.feed("lab2_gauge");
AdafruitIO_Feed analog_led = io.feed("lab2_ledctrl");
AdafruitIO_Feed *analog_lgage = io.feed("lab2_linegage");

void setup() {
  
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

  // wait for a connection
  while(io.status() < AIO_CONNECTED) {
    Serial.print(".");
    delay(500);
  }

  // we are connected
  Serial.println();
  Serial.println(io.statusText());
  analog_led->get();
  Serial.println(WiFi.localIP());

}

void loop() {

  // io.run(); is required for all sketches.
  // it should always be present at the top of your loop
  // function. it keeps the client connected to
  // io.adafruit.com, and processes any incoming data.
  io.run();

  // grab the current state of the photocell
  current = analogRead(TRIMMER_PIN);

  // return if the value hasn't changed
  if(current == last)
    return;

  // save the current state to the analog feed
  Serial.print("sending -> ");
  Serial.println(current);
  analog->save(current);

  // store last photocell state
  last = current;

  // wait three seconds (1000 milliseconds == 1 second)
  //
  // because there are no active subscriptions, we can use delay()
  // instead of tracking millis()
  delay(3000);
}

void handleMessage(AdafruitIO_Data *data) {

  // convert the data to integer
  int reading = data->toInt();

  Serial.print("received <- ");
  Serial.println(reading);

  // write the current 'reading' to the led
  analogWrite(LED_PIN, reading);
}
