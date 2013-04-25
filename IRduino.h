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

//DEFINES-------------------------------------------------------------------------

//Carrier Mode
#define CARRIERMODE_38K	38			//represents a 38kHz carrier frequency (26uS period)

//Bit Order
#define FIRSTBIT_LSB 	0b00000001	//represents a Least Significant Bit First order
#define FIRSTBIT_MSB 	0b10000000	//represents a Most Significant Bit First order

//Chunk Size
#define CHUNKSIZE_NIBBLE 4			//represents a Chunk Size of 4 bits
#define CHUNKSIZE_BYTE 8			//represents a Chunk Size of 8 bits

class IRduino {
public:

	//	SEQUENCES
	//
	//	Sequences are arrays containing pulse lengths. Pulse lengths are measured in
	//	periods of the carrier frequency.
	//
	//	For example, a 38kHz carrier frequency has a period of 26 microseconds, and a 0.5
	//	millisecond pulse could be represented by roughly 19 periods.
	//
	//	A sequence of {10,20,30} would be transmitted as 10 periods ON, 20 periods OFF,
	//	and 30 periods ON.
	//
	static uint16_t *oneSQ;
	static uint8_t oneSQlen;

	static uint16_t *zeroSQ;
	static uint8_t zeroSQlen;

	static uint16_t *startSQ;
	static uint8_t startSQlen;

	static uint16_t *stopSQ;
	static uint8_t stopSQlen;

	//	init()				Initialises the IR library.
	//
	//	Accepted parameters are explained above in the DEFINES section.
	//
	static void init(uint8_t carrierMode, uint8_t startBit, uint8_t chunkSize);

	//	bufferChunk()		Buffers a chunk of data.
	//
	//	Accepts an entire byte, but if CHUNKSIZE_NIBBLE is used, only the first
	//	4 bytes of each chunk is transmitted. CHUNKSIZE_BYTE transmits all 8 bytes of each
	//	chunk.
	//
	//	Returns the value of the chunk for checksum purposes.
	//
	static uint8_t bufferChunk(uint8_t chunk);

	//	bufferReset()		Empties the transmit buffer.
	//
	static void bufferReset();

	//	bufferTransmit()	Transmits chunks buffered so far.
	//
	//	Using FIRSTBIT_LSB  sends the Least Significant Bit first, 0b0111 sent as 1110.
	//	Using FIRSTBIT_MSB sends the Most Significant Bit first, 0b0111 sent as 0111.
	//
	static void bufferTransmit();

	//	bufferZeroChunks() Buffers count number of zero filled chunks.
	//
	//	Some protocols pad large number of zeroes between actual data. Use this to quickly
	//	fill those spaces when submitting chunks to the buffer.
	//
	//	Sends either a NIBBLE or a BYTE of zeroes, depending on CHUNKSIZE.
	//
	//	Returns the value 0 for checksum purposes.
	//
	static uint8_t bufferZeroChunks(uint8_t count);

	//	bufferChunk4Bits(), bufferChunk8Bits()	buffers a chunk with the exact bits given.
	//
	//	Some protocols use a single bit to represent a boolean setting. Use this to quickly
	//	set those bits and submit them as a chunk.
	//
	//	Returns the value of the chunk for checksum purposes.
	//
	static uint8_t bufferChunk4Bits(uint8_t a, uint8_t b, uint8_t c, uint8_t d);
	static uint8_t bufferChunk8Bits(uint8_t a, uint8_t b, uint8_t c, uint8_t d,
								 uint8_t e, uint8_t f, uint8_t g, uint8_t h);
};

extern IRduino IR;

#endif
