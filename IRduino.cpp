//
// Brian Lee
// 2013-04-25
// <garcondebanane@gmail.com>
//
// IR transmits on PIN 3 (Arduino UNO)
//

#include <IRduino.h>

//PROTOCOL SEQUENCES-----------------------------------------------------------------

uint16_t *IRduino::oneSQ;
uint8_t IRduino::oneSQlen;

uint16_t *IRduino::zeroSQ;
uint8_t IRduino::zeroSQlen;

uint16_t *IRduino::startSQ;
uint8_t IRduino::startSQlen;

uint16_t *IRduino::stopSQ;
uint8_t IRduino::stopSQlen;

//INTERRUPT COUNTING TIMER-----------------------------------------------------------

volatile uint16_t irPulseCount = 0;
ISR(TIMER2_COMPB_vect)
{
	irPulseCount++;	//increment counter every tick
}

void irSequenceTransmit(const uint16_t *sequence, uint8_t length)
{
	uint8_t pulseIndex = 0;
	while(pulseIndex < length)
	{
		if(pulseIndex%2 == 0)
			TCCR2A |= _BV(COM2B1);	//ON for every even pulse of sequence
		else
			TCCR2A &= ~_BV(COM2B1);	//OFF for every odd pulse of sequence

		//reset pulse counter
		irPulseCount = 0;

		//block until the counter exceeds pulse duration
		while(irPulseCount <= sequence[pulseIndex])
		{}

		//next pulse in sequence
		pulseIndex += 1;
	}

	//turn pulse OFF after sequence complete.
	TCCR2A &= ~_BV(COM2B1);
}

//PUBLIC FUNCTIONS--------------------------------------------------------------------

uint8_t irChunkBuffer[255];
uint8_t irChunkBufferEnd = 0;

uint8_t irCarrierMode;
uint8_t irFirstBit;
uint8_t irChunkSize;

void IRduino::init(uint8_t carrierMode, uint8_t firstBit, uint8_t chunkSize)
{
	irCarrierMode = carrierMode;
	irFirstBit = firstBit;
	irChunkSize = chunkSize;

	pinMode(3, OUTPUT);		//set pin to output mode
	TCCR2A = _BV(WGM21) | _BV(WGM20); // Just enable output on Pin 3 and disable it on Pin 11
	TCCR2B = _BV(WGM22) | _BV(CS22);

	switch(irCarrierMode)
	{
		case CARRIERMODE_38K:
		default:
			OCR2A = 51; // defines the frequency 51 = 38.4 KHz, 54 = 36.2 KHz, 58 = 34 KHz, 62 = 32 KHz
			OCR2B = 26;  // deines the duty cycle - Half the OCR2A value for 50%
			TCCR2B = TCCR2B & 0b00111000 | 0x2; // select a prescale value of 8:1 of the system clock
			break;
	}

	TIMSK2 = _BV(OCIE2B);	// Output Compare Match B Interrupt Enable
}

uint8_t IRduino::bufferChunk(uint8_t chunk)
{
	if(irChunkBufferEnd < 255)
	{
		irChunkBuffer[irChunkBufferEnd] = chunk;
		irChunkBufferEnd++;
	}

	return chunk;
}

void IRduino::bufferReset()
{
	irChunkBufferEnd = 0;
}

void IRduino::bufferTransmit()
{
	//transmit start
	irSequenceTransmit(startSQ, startSQlen);

	//transmit everything in the buffer
	uint8_t chunkIndex = 0;
	while(chunkIndex < irChunkBufferEnd)
	{
		//transmit all bits in this chunk
		uint8_t chunkMask = irFirstBit;		//init the mask to the first bit to send
		uint8_t bitIndex = 0;
		while(bitIndex < irChunkSize)
		{
			//transmit this bit
			if(irChunkBuffer[chunkIndex] & chunkMask)
				irSequenceTransmit(oneSQ, oneSQlen);
			else
				irSequenceTransmit(zeroSQ, zeroSQlen);

			//shift the mask for the next bit
			if(irFirstBit == FIRSTBIT_LSB)
				chunkMask <<= 1;
			else
				chunkMask >>= 1;

			bitIndex++;
		}
		chunkIndex += 1;
	}

	//transmit end
	irSequenceTransmit(stopSQ, stopSQlen);
}

//CONVENIENCE FUNCTIONS----------------------------------------------------------------

uint8_t IRduino::bufferZeroChunks(uint8_t count)
{
  for(;count > 0; count--)
  {
    bufferChunk(0);
  }

  return 0;
}

uint8_t IRduino::bufferChunk4Bits(uint8_t a, uint8_t b, uint8_t c, uint8_t d)
{
  uint8_t data = 0;

  data |= d;
  data <<= 1;
  data |= c;
  data <<= 1;
  data |= b;
  data <<= 1;
  data |= a;

  return bufferChunk(data);
}

uint8_t IRduino::bufferChunk8Bits(uint8_t a, uint8_t b, uint8_t c, uint8_t d,
								 uint8_t e, uint8_t f, uint8_t g, uint8_t h)
{
  uint8_t data = 0;

  data |= h;
  data <<= 1;
  data |= g;
  data <<= 1;
  data |= f;
  data <<= 1;
  data |= e;
  data <<= 1;
  data |= d;
  data <<= 1;
  data |= c;
  data <<= 1;
  data |= b;
  data <<= 1;
  data |= a;

  return bufferChunk(data);
}
