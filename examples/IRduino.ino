//Works with YORK air conds.

#include <IRduino.h>

void setup()
{
  Serial.begin(9600);
  Serial.println("Initializing...");

  IR.init(0,0,0);
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
