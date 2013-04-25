//
// Brian Lee
// 2013-04-25
// <garcondebanane@gmail.com>
//
// IR transmits on PIN 3 (Arduino UNO)
//

#define __PROG_TYPES_COMPAT__

#ifndef IRduino_h
#define IRduino_h

#if ARDUINO >= 100
  #include <Arduino.h> // Arduino 1.0
#else
  #include <WProgram.h> // Arduino 0022
#endif

class IRduino {
public:
	//init
	static void init(uint8_t carrierMode, uint8_t chunkOrder, uint8_t chunkSize);

	//main functions
	static uint8_t bufferChunk(uint8_t chunk);
	static void bufferReset();
	static void bufferTransmit();

	//convenience functions
	static uint8_t bufferZeroChunks(uint8_t count);
	static uint8_t bufferChunk4Bits(uint8_t a, uint8_t b, uint8_t c, uint8_t d);
	static uint8_t bufferChunk8Bits(uint8_t a, uint8_t b, uint8_t c, uint8_t d,
								 uint8_t e, uint8_t f, uint8_t g, uint8_t h);
};

extern IRduino IR;

#endif
