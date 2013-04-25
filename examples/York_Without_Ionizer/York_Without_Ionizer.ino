//
// Brian Lee
// 2013-04-25
// <garcondebanane@gmail.com>
//
// For York AC without Ionizer
// IR transmits on PIN 3 (Arduino UNO)
//

#include <IRduino.h>

void setup()
{
  Serial.begin(9600);
  Serial.println("Setting up IRduino...");

  //38.4kHz carrier, LSB first, nibble chunk sizes
  IR.init(CARRIERMODE_38K, FIRSTBIT_LSB, CHUNKSIZE_NIBBLE);
  
  //a one consists of 13 on pulses, followed by 37 off
  static uint16_t ONESQ[] = {13,36};
  IR.oneSQ = ONESQ;
  IR.oneSQlen = 2;
  
  //a zero consists of 13 on pulses, followed by 15 off
  static uint16_t ZEROSQ[] = {13,15};
  IR.zeroSQ = ZEROSQ;
  IR.zeroSQlen = 2;
  
  //the start signal consists of 177 on pulses, followed by 96 off
  static uint16_t STARTSQ[] = {177,96};
  IR.startSQ = STARTSQ;
  IR.startSQlen = 2;
  
  //the stop signal consists of 13 on pulses, followed by 796 off and 148 on.
  static uint16_t STOPSQ[] = {13,796,148};
  IR.stopSQ = STOPSQ;
  IR.stopSQlen = 3;
}

void loop()
{
  if(Serial.read() == '\n')
  {
    IR.bufferReset();
    int checksum = 0;
    checksum += IR.bufferZeroChunks(5);  //pad zeroes
    checksum += IR.bufferChunk(0);  //timer in hours
    checksum += IR.bufferZeroChunks(23);  //pad zeroes
    checksum += IR.bufferChunk4Bits(1,0,0,0);  //swing, sleep, nothing, power
    checksum += IR.bufferChunk4Bits(0,1,0,0);  //auto, high, medium, low
    checksum += IR.bufferChunk4Bits(0,1,0,0);  //dry, cool, fan, nothing
    checksum += IR.bufferChunk(6);  //temp 1s
    checksum += IR.bufferChunk(1);  //temp 10s
    IR.bufferChunk(checksum);
    IR.bufferTransmit();
  }
}
