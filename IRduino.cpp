//
// Brian Lee
// 2013-04-25
// <garcondebanane@gmail.com>
//
// IR transmits on PIN 3 (Arduino UNO)
//

#include <IRduino.h>

//PROTOCOL SEQUENCES-----------------------------------------------------------------

const uint16_t oneSQ[] = {13,34};
const uint8_t oneSQLen = 2;

const uint16_t zeroSQ[] = {13,15};
const uint8_t zeroSQLen = 2;

const uint16_t startSQ[] = {177,96};
const uint8_t startSQLen = 2;

const uint16_t stopSQ[] = {13,796,148};
const uint8_t stopSQLen = 3;

uint8_t chunkBuffer[255];
uint8_t chunkBufferEnd = 0;

volatile uint8_t pulseRunning = false;
volatile uint16_t pulseCount = 0;

ISR(TIMER2_COMPB_vect)
{
	pulseCount++;	//increment counter every tick
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
		pulseCount = 0;

		//block until the counter exceeds pulse duration
		while(pulseCount <= sequence[pulseIndex])
		{}

		//next pulse in sequence
		pulseIndex += 1;
	}

	//turn pulse OFF after sequence complete.
	TCCR2A &= ~_BV(COM2B1);
}

//PUBLIC FUNCTIONS--------------------------------------------------------------------

void IRduino::init(uint8_t carrierMode, uint8_t chunkOrder, uint8_t chunkSize)
{
	pinMode(3, OUTPUT);		//set pin to output mode

	TCCR2A = _BV(WGM21) | _BV(WGM20); // Just enable output on Pin 3 and disable it on Pin 11
	TCCR2B = _BV(WGM22) | _BV(CS22);
	OCR2A = 51; // defines the frequency 51 = 38.4 KHz, 54 = 36.2 KHz, 58 = 34 KHz, 62 = 32 KHz
	OCR2B = 26;  // deines the duty cycle - Half the OCR2A value for 50%
	TCCR2B = TCCR2B & 0b00111000 | 0x2; // select a prescale value of 8:1 of the system clock

	TIMSK2 = _BV(OCIE2B);	// Output Compare Match B Interrupt Enable
}

uint8_t IRduino::bufferChunk(uint8_t chunk)
{
	if(chunkBufferEnd < 255)
	{
		chunkBuffer[chunkBufferEnd] = chunk;
		chunkBufferEnd++;
	}

	return chunk;
}

void IRduino::bufferReset()
{
	chunkBufferEnd = 0;
}

void IRduino::bufferTransmit()
{
	//transmit start
	irSequenceTransmit(startSQ, startSQLen);

	//transmit the buffer
	uint8_t chunkIndex = 0;
	while(chunkIndex < chunkBufferEnd)
	{
		uint8_t chunkMask = 0b00000001;
		while(chunkMask <= 0b00001000)
		{
			if(chunkBuffer[chunkIndex] & chunkMask)
			{
				//send 1
				irSequenceTransmit(oneSQ, oneSQLen);
			}
			else
			{
				//send 0
				irSequenceTransmit(zeroSQ, zeroSQLen);
			}

			//next bit
			chunkMask <<= 1;
		}

		//next chunk
		chunkIndex += 1;
	}

	//transmit end
	irSequenceTransmit(stopSQ, stopSQLen);
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
