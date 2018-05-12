// Copyright 2018 Immanuel/EManuele
// Sushima/Guquiao AC
#ifndef SUSHIMA_H
#define SUSHIMA_H
#define __STDC_LIMIT_MACROS
#include <stdint.h>
#ifndef UNIT_TEST
#include <Arduino.h>
#else
#include <string>
#endif
#include "IRremoteESP8266.h"
#include "IRSend.h"


//IR settings
#define HDR_MARK                              3500U
#define HDR_SPACE                             1700U
#define ONE_MARK                               360U
#define ONE_SPACE                             1400U
#define ZERO_MARK                              360U
#define ZERO_SPACE                             500U
#define FOOTER_MARK                            500U
#define GAP                                      0U

// ON/OFF command
#define SUSHIMA_OFF                     0b00000100U
#define SUSHIMA_ON                      0b00100100U

// Constants
#define SUSHIMA_MODE_AUTO               0b00010000U //16
#define SUSHIMA_MODE_HEAT               0b10000000U //128
#define SUSHIMA_MODE_COOL               0b11000000U //192
#define SUSHIMA_MODE_DRY                0b01000000U //64
#define SUSHIMA_MODE_AUTO_DRY_TEMP      0b11100000U //224


#define SUSHIMA_FLAP_AUTO               0b00000000U //0
#define SUSHIMA_FLAP_HIGHEST            0b00010000U //16
#define SUSHIMA_FLAP_HIGH               0b00001000U //8
#define SUSHIMA_FLAP_MID                0b00011000U //24
#define SUSHIMA_FLAP_LOW                0b00000100U //4
#define SUSHIMA_FLAP_LOWEST             0b00010100U //20
#define SUSHIMA_FLAP_SWING              0b00011100U //
#define SUSHIMA_FLAP_MASK               0b11100000U

#define SUSHIMA_FAN_SPEED_AUTO          0b00000000U
#define SUSHIMA_FAN_SPEED_SLOW          0b01000000U
#define SUSHIMA_FAN_SPEED_MEDIUM        0b11000000U
#define SUSHIMA_FAN_SPEED_FAST          0b10100000U
#define SUSHIMA_FAN_SPEED_SLEEP         0b10000000U
#define SUSHIMA_FAN_SPEED_MASK          0b00011111U

// Human understandable op code for all AC modes (except timer)
// AC op mode
#define MODE_AUTO                                1U
#define MODE_HEAT                                2U
#define MODE_COOL                                3U
#define MODE_DRY                                 4U

// AC fan speed mode
#define FAN_AUTO                                 1U
#define FAN_SLOW                                 2U
#define FAN_MEDIUM                               3U
#define FAN_FAST                                 4U
#define FAN_SLEEP                                5U

// AC  flap  direction + swing
#define FLAP_AUTO                                1U
#define FLAP_HIGHEST                             2U
#define FLAP_HIGH                                3U
#define FLAP_MID                                 4U
#define FLAP_LOW                                 5U
#define FLAP_LOWEST                              6U
#define FLAP_SWING                               7U

// Positions of settings in in _state[]
#define ON_OFF_POS                                5
#define ACMODE_POS                                6
#define TEMP_POS                                  7
#define FANSPEED_FLAP_POS                         8
#define CHECKSUM_POS                             13



class Sushima {
  private :
    IRsend _irsend;
    uint8_t _temp;
    uint8_t _mode;
    uint8_t _fanSpeed;
    uint8_t _flap;
    uint8_t _on_off;
    uint8_t _state[14];
    void buildState();
    void computeChecksum();
  public :
    explicit Sushima(uint16_t pin);
    uint8_t getTemp(void);
    uint8_t getMode(void);
    uint8_t getFanSpeed(void);
    uint8_t getFlap(void);
    uint8_t getChecksum(void);
    void setTemp(uint8_t temp);
    void setFlap(uint8_t flap);
    void setFanSpeed(uint8_t speed);
    void setMode(uint8_t acmode);
    void off();
    void on();
    void stateReset();
    void send();
    void begin();
};
#endif
