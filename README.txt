Code to emulate IR on Sushima/Guquiao for ESP8266.
Tested only on a Sushima KFR-34WA.
Req: IRremoteESP8266.
A Sushima AC command is 14 bytes.
* Bytes from 0 to 4 are fixed:
  byte[0]=0b11000100
  byte[1]=0b11010011
  byte[2]=0b01100100
  byte[3]=0b10000000
  byte[4]=0b00000000
* Byte 5 is used to turn on/off the AC:
  byte[5]=0b00000100 OFF
  byte[5]=0b00100100 ON
* Byte 6 contains AC mode, available modes:
  AUTO    0b00010000
  HEAT    0b10000000
  COOL    0b11000000
  DRY     0b01000000
* Byte 7, first for 4 bits set the temperature:
  min: 16C, max: 31C Temperature in Celsius.
  byte[7]=xxxx0000
  Where:
          11110000 16C
          01110000 17C
          ...
          ..
          10000000 30C
          00000000 31C
* Byte 8: Fan Speed and Flap:
  byte[8]=xxxyyy00
  Where:
    xxx sets Fan Speed:
      AUTO    0b00000000
      SLOW    0b01000000
      MID     0b11000000
      FAST    0b10100000
      SLEEP   0b10000000
    yyy sets Flap:
      AUTO    0b00000000
      HIGHEST 0b00010000
      HIGH    0b00001000
      MID     0b00011000
      LOW     0b00000100
      LOWEST  0b00010100
      SWING   0b00011100
* Bytes from 9 to 12 should be used to set timer (not implemented).
* Byte 13 contains checksum.
  Checksum: (sum % 256 of all bytes from 0 to 12 reversed) reversed.
IR settings:
  Freq          38kHz
  HDR_MARK      3500
  HDR_SPACE     1700
  ONE_MARK      360
  ONE_SPACE     1400
  ZERO_MARK     360
  ZERO_SPACE    500
  FOOTER_MARK   500
  GAP           0
