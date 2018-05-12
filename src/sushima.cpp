// Copyright 2018 Immanuel/EManuele
// Code to emulate IR on Sushima/Guquiao
// Tested only on a Sushima KFR-34WA
// * Supports:
//    - All AC modes: Auto, Heat, Cool, Dry
//    - Temperature set (min: 16C - max: 31C)
//    - All flap modes: Auto, Highest, High, Mid, Low, Lowest, Swing
//    - All fan speeds: Auto, Slow, Mid, Fast, Sleep
// * Unsupported:
//    - Timer


//  #####
// #     # #    #  ####  #    # # #    #   ##
// #       #    # #      #    # # ##  ##  #  #
//  #####  #    #  ####  ###### # # ## # #    #
//       # #    #      # #    # # #    # ######
// #     # #    # #    # #    # # #    # #    #
//  #####   ####   ####  #    # # #    # #    #



#include <Arduino.h>
#include "../include/sushima.h"
#include <algorithm>
#include "IRremoteESP8266.h"
#include "IRSend.h"
#include "IRUtils.h"

Sushima::Sushima(uint16_t pin) : _irsend(pin) {
  stateReset();
}

void Sushima::stateReset() {
  _temp=24;
  _fanSpeed=FAN_AUTO;
  _mode=MODE_AUTO;
  _flap=FLAP_AUTO;
  _on_off=SUSHIMA_OFF;
  buildState();
}

void Sushima::buildState() {
  // bytes from 0 to 4 are constant
  _state[0]=0b11000100U;
  _state[1]=0b11010011U;
  _state[2]=0b01100100U;
  _state[3]=0b10000000U;
  _state[4]=0b00000000U;
  _state[ON_OFF_POS]=_on_off;
  setTemp(_temp);
  setFanSpeed(_fanSpeed);
  setFlap(_flap);
  setMode(_mode);
  // These bytes are set to 0 but are used by timer [not implemented yet]
  _state[9]=0b00000000U;
  _state[10]=0b00000000U;
  _state[11]=0b00000000U;
  _state[12]=0b00000000U;
  // Checksum set to 0, it will be computed before sending command
  _state[CHECKSUM_POS]=0b00000000U;
}

// Temp in Celsius, min:16, max:31
void Sushima::setTemp(uint8_t temp) {
  _temp=temp;
  uint8_t tmp_temp=(abs(temp-31));
  uint8_t tcc=0b00000000;
  for(int i=0;i<4;i++) {
    if( ((tmp_temp>>i)%2)==1 ) {
      tcc|=0b00001000>>i;
    }
  }
  _state[TEMP_POS]=tcc<<4;
}

void Sushima::setFanSpeed(uint8_t fan_speed) {
  // Set Fan Speed BYTE 8 bits[7-5]
  _fanSpeed=fan_speed;
  // Reset fan_speed bits
  _state[FANSPEED_FLAP_POS]&=SUSHIMA_FAN_SPEED_MASK;
  switch (fan_speed) {
  case FAN_AUTO:
    _state[FANSPEED_FLAP_POS]|=SUSHIMA_FAN_SPEED_AUTO;
    break;
  case FAN_SLOW:
    _state[FANSPEED_FLAP_POS]|=SUSHIMA_FAN_SPEED_SLOW;
    break;
  case FAN_MEDIUM:
    _state[FANSPEED_FLAP_POS]|=SUSHIMA_FAN_SPEED_MEDIUM;
    break;
  case FAN_FAST:
    _state[FANSPEED_FLAP_POS]|=SUSHIMA_FAN_SPEED_FAST;
    break;
  case FAN_SLEEP:
    _state[FANSPEED_FLAP_POS]|=SUSHIMA_FAN_SPEED_SLEEP;
    break;
  }
}

void Sushima::setFlap(uint8_t flap) {
  // Set Direction BYTE 8 bits[4-2]
  _flap=flap;
  //Reset flap bits
  _state[FANSPEED_FLAP_POS]&=SUSHIMA_FLAP_MASK;
  switch (flap) {
    case FLAP_AUTO:
      _state[FANSPEED_FLAP_POS]|=SUSHIMA_FLAP_AUTO;
      break;
    case FLAP_HIGHEST:
      _state[FANSPEED_FLAP_POS]|=SUSHIMA_FLAP_HIGHEST;
      break;
    case FLAP_HIGH:
      _state[FANSPEED_FLAP_POS]|=SUSHIMA_FLAP_HIGH;
      break;
    case FLAP_MID:
      _state[FANSPEED_FLAP_POS]|=SUSHIMA_FLAP_MID;
      break;
    case FLAP_LOW:
      _state[FANSPEED_FLAP_POS]|=SUSHIMA_FLAP_LOW;
      break;
    case FLAP_LOWEST:
      _state[FANSPEED_FLAP_POS]|=SUSHIMA_FLAP_LOWEST;
      break;
    case FLAP_SWING:
      _state[FANSPEED_FLAP_POS]|=SUSHIMA_FLAP_SWING;
      break;
  }
}

void Sushima::setMode(uint8_t mode) {
  _mode=mode;
  switch(mode) {
    case MODE_HEAT:
      _state[ACMODE_POS]=SUSHIMA_MODE_HEAT;
      break;
    case MODE_COOL:
      _state[ACMODE_POS]=SUSHIMA_MODE_COOL;
      break;
    case MODE_AUTO:
      _state[ACMODE_POS]=SUSHIMA_MODE_AUTO;
      break;
    case MODE_DRY:
      _state[ACMODE_POS]=SUSHIMA_MODE_DRY;
      break;
  }
}

void Sushima::computeChecksum() {
  uint8_t sum=0;
  for(int i=0; i<13;i++) {
    sum+=reverseBits(_state[i], 8);
  }
  _state[CHECKSUM_POS]=reverseBits(sum, 8);
}

uint8_t Sushima::getChecksum() {
  uint8_t sum=0;
  for(int i=0; i<13;i++) {
    sum+=reverseBits(_state[i], 8);
  }
  return(reverseBits(sum, 8));
}

void Sushima::off() {
  _on_off=SUSHIMA_OFF;
  _state[ON_OFF_POS]=SUSHIMA_OFF;
}

void Sushima::on() {
  _on_off=SUSHIMA_ON;
  _state[ON_OFF_POS]=SUSHIMA_ON;
}

void Sushima::begin() {
  _irsend.begin();
}
void Sushima::send() {
  // If mode is AUTO or dry then we must set temperature to SUSHIMA_MODE_AUTO_DRY_TEMP
  if(_mode==MODE_AUTO || _mode==MODE_DRY) {
    _state[TEMP_POS]=SUSHIMA_MODE_AUTO_DRY_TEMP;
  }
  computeChecksum();
  _irsend.sendGeneric(HDR_MARK, HDR_SPACE, ONE_MARK, ONE_SPACE, ZERO_MARK, ZERO_SPACE, FOOTER_MARK, GAP, _state, 14, 38, true, 0, 50);
}
