#include <Arduino.h>
#include "../include/sushima.h"
#include "IRUtils.h"
#include <ESP8266WiFi.h>
#include <IRSend.h>
#include <string.h>

#define BAUD_RATE 115200
#define IR_PIN 4 // IR led is connected to PIN D4
Sushima sushima(IR_PIN);


void setup() {
  sushima.begin();
  Serial.begin(BAUD_RATE, SERIAL_8N1, SERIAL_TX_ONLY);
  while (!Serial) { // Wait for the serial connection to be establised.
    delay(50);
  }
  sushima.setFanSpeed(FAN_FAST);
  sushima.setFlap(FLAP_HIGH);
  sushima.setMode(MODE_HEAT);
  sushima.setTemp(27);
  sushima.on();

}

void loop() {
  Serial.println("Sending IR command to AC..");
  sushima.send();
  delay(5000);
}
