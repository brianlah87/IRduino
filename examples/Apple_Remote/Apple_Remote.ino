//
// Brian Lee
// 2013-04-25
// <garcondebanane@gmail.com>
//
// For Apple Remote
// IR transmits on PIN 3 (Arduino UNO)
//

#include <IRduino.h>

void setup()
{
  Serial.begin(9600);
  Serial.println("Setting up IRduino...");

  //38.4kHz carrier, LSB first, nibble chunk sizes
  IR.init(CARRIERMODE_38K, FIRSTBIT_LSB, CHUNKSIZE_BYTE);
  
  //a one consists of 13 on pulses, followed by 37 off
  static uint16_t ONESQ[] = {22,65};
  IR.oneSQ = ONESQ;
  IR.oneSQlen = 2;
  
  //a zero consists of 13 on pulses, followed by 15 off
  static uint16_t ZEROSQ[] = {22,22};
  IR.zeroSQ = ZEROSQ;
  IR.zeroSQlen = 2;
  
  //the start signal consists of 177 on pulses, followed by 96 off
  static uint16_t STARTSQ[] = {346,173};
  IR.startSQ = STARTSQ;
  IR.startSQlen = 2;
  
  //the stop signal consists of 13 on pulses, followed by 796 off and 148 on.
  static uint16_t STOPSQ[] = {22};
  IR.stopSQ = STOPSQ;
  IR.stopSQlen = 1;
}

void loop()
{
  if(Serial.read() == '\n')
  {
    IR.bufferReset();
    IR.bufferChunk(0xEE);
    IR.bufferChunk(0x87);
    IR.bufferChunk(0x5E);
    IR.bufferChunk(0x59);
    IR.bufferTransmit();
  }
}
