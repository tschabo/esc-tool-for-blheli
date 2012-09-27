

// baud rate for serial interface
#define BAUD 38400

#include <stdio.h>
#include <avr/io.h>
#include <util/setbaud.h>
#include <util/delay.h>

int uartPutchar(char c, FILE * f)
{
	loop_until_bit_is_set(UCSR0A, UDRE0); /* Wait until data register empty. */
	UDR0 = c;
	return 1;
}

int uartGetchar(FILE * f)
{
	loop_until_bit_is_set(UCSR0A, RXC0); /* Wait until data exists. */
	return UDR0;
}

// commands
#define GET_VERSION		0x01
#define BLOCK_READ		0x06
#define BLOCK_WRITE		0x07
#define PAGE_ERASE		0x08
#define DEVICE_ERASE	0x03

// registers
#define FPDAT			0xB4
#define FPCTL			0x02
#define DEVICEID		0x00
#define REVID			0x01

// status
#define STAT_OK         0x0D

#define C2CK_LOW PORTB &= ~(1 << PORTB3);__asm__ volatile("nop")
#define C2CK_HIGH PORTB |= (1 << PORTB3); __asm__ volatile("nop")
#define C2D_HIGH PORTB |= (1 << PORTB4);__asm__ volatile("nop")
#define C2D_LOW PORTB &= ~(1 << PORTB4);__asm__ volatile("nop")
#define C2D_INPUT DDRB &= ~(1 << DDB4); PORTB |= (1 << PORTB4);__asm__ volatile("nop") // input pullup disabled
#define C2D_OUTPUT DDRB |= (1 << DDB4);__asm__ volatile("nop")
#define READ_C2D ((PINB & (1 << PINB4)) ? 1 : 0)

#define POLL_OUT_READY while(!(c2ReadAR() & 0x01));
#define POLL_IN_BUSY while(c2ReadAR() & 0x02);

// constants
static const char fw_version[] = "D2PROG V0.1";
static const char termSeq[] = "\n>";
static const char ok[] = "ok";
static const char er[] = "er";

// C2 Interface
void c2ckPulse();
void c2Reset();
uint8_t c2GetDeviceId();
uint8_t c2GetRevision();
uint8_t c2ReadAR();
void c2WriteAR(uint8_t address);
uint8_t c2ReadDR();
void c2WriteDR(uint8_t data);

// Flash programming
void flashEnable();
uint8_t flashReadBlock();
uint8_t flashWriteBlock();
uint8_t flashPageErase(uint8_t page);
uint8_t flashErase();
// Flash helpers
uint8_t flashSetupCommand(uint8_t mode);
uint8_t flashSetupAddress();

// programming interface
char stringHexBuffer[5];
uint8_t byteCount;
uint16_t flashAddress;
uint8_t rwBuffer[128];
// helpers
unsigned int xtoi(const char *hexStr);
uint8_t blockRead();
uint8_t blockWrite();


void c2ckPulse()
{
	PORTB &= ~(1 << PORTB3);
	PORTB |= (1 << PORTB3);
}

void c2Reset()
{
	C2CK_LOW;

	// low time for a device reset
	_delay_us(20);

	C2CK_HIGH;

	// be shure that the Startbit is written at least after
	// a dealay of 2us (t_SD)
	_delay_us(2);
}

uint8_t c2GetDeviceId()
{
	c2WriteAR(DEVICEID);
	return c2ReadDR();
}

uint8_t c2GetRevision()
{
	c2WriteAR(REVID);
	return c2ReadDR();
}

uint8_t c2ReadAR()
{
	uint8_t address = 0;

	// start frame
	C2D_OUTPUT;
	C2D_LOW;
	c2ckPulse();

	// instruction frame
	// 10b (lsb first) for address read
	C2D_LOW;
	c2ckPulse();
	C2D_HIGH;
	c2ckPulse();

	// read address
	C2D_INPUT;
	for(uint8_t i = 0; i < 8; i++)
	{
		address >>= 1;
		c2ckPulse();
		if(READ_C2D)
		{
			// write higest bit
			// shift it to the rigt
			// in the next iteration
			address |= 0x80;
		}
	}

	// stop frame
	C2D_OUTPUT;
	C2D_LOW;
	c2ckPulse();

	return address;
}

void c2WriteAR(uint8_t address)
{
	// start frame
	C2D_OUTPUT;
	C2D_LOW;
	c2ckPulse();

	// instruction frame
	// 11b (lsb first) for address write
	C2D_HIGH;
	c2ckPulse();
	C2D_HIGH;
	c2ckPulse();

	// Write adress
	for(uint8_t i = 0; i < 8; i++)
	{
		if(address & 0x01)
		{
			C2D_HIGH;
		}
		else
		{
			C2D_LOW;
		}
		c2ckPulse();
		address >>= 1;
	}

	C2D_LOW;
	c2ckPulse();
}

uint8_t c2ReadDR()
{
	uint8_t data = 0;

	// start frame
	C2D_OUTPUT;
	C2D_LOW;
	c2ckPulse();

	// instruction frame
	// 00b (lsb first) for data read
	C2D_LOW;
	c2ckPulse();
	C2D_LOW;
	c2ckPulse();

	// size 00b = 1
	C2D_LOW;
	c2ckPulse();
	C2D_LOW;
	c2ckPulse();

	// read from c2d
	C2D_INPUT;

	// wait
	do
	{
		c2ckPulse();
	}
	while(!READ_C2D);

	for(uint8_t i = 0; i < 8; i++)
	{
		data >>= 1;
		c2ckPulse();
		if(READ_C2D)
		{
			data |= 0x80;
		}
	}

	// stop frame
	C2D_OUTPUT;
	C2D_LOW;
	c2ckPulse();

	return data;
}

void c2WriteDR(uint8_t data)
{
	// start frame
	C2D_OUTPUT;
	C2D_LOW;
	c2ckPulse();

	// instruction frame
	// 01b (lsb first) for data write
	C2D_HIGH;
	c2ckPulse();
	C2D_LOW;
	c2ckPulse();

	// size 00b = 1
	C2D_LOW;
	c2ckPulse();
	C2D_LOW;
	c2ckPulse();

	// write data
	for(uint8_t i = 0; i < 8; i++)
	{
		if(data & 0x01)
		{
			C2D_HIGH;
		}
		else
		{
			C2D_LOW;
		}
		c2ckPulse();
		data >>= 1;
	}

	// wait
	C2D_INPUT;
	do
	{
		c2ckPulse();
	}
	while(!READ_C2D);

	// stop frame
	C2D_OUTPUT;
	C2D_LOW;
	c2ckPulse();
}

void flashEnable()
{
	// flash program register
	c2WriteAR(FPCTL);

	// enable flash programming
	c2WriteDR(0x02);
	c2WriteDR(0x01);

	// wait 20 ms
	_delay_us(20000);
}

uint8_t flashSetupCommand(uint8_t mode)
{
	// choose flash prog register
	c2WriteAR(FPDAT);

	// modus block read
	c2WriteDR(mode);

	//poll
	POLL_IN_BUSY;
	//poll
	POLL_OUT_READY;

	// read status
	if (c2ReadDR() != STAT_OK)
	{
		return 0;
	}
	return 1;
}

uint8_t flashSetupAddress()
{
	// set flash address
	c2WriteDR(flashAddress >> 8);
	POLL_IN_BUSY;
	c2WriteDR(flashAddress & 0x00ff);
	POLL_IN_BUSY;

	// set bytes count
	c2WriteDR(byteCount);
	POLL_IN_BUSY;

	// read status
	if (c2ReadDR() != STAT_OK)
	{
		return 0;
	}

	return 1;
}


uint8_t flashReadBlock()
{
	// setting up read modus
	if(!flashSetupCommand(BLOCK_READ))
	{
		return 0;
	}

	// set address and length
	if(!flashSetupAddress())
	{
		return 0;
	}

	for(uint8_t i = 0; i < byteCount; i++)
	{
		POLL_OUT_READY;
		rwBuffer[i] = c2ReadDR();
	}

	return 1;
}

uint8_t flashWriteBlock()
{
	// setting up write modus
	if(!flashSetupCommand(BLOCK_WRITE))
	{
		return 0;
	}

	// set address and length
	if(!flashSetupAddress())
	{
		return 0;
	}

	for(uint8_t i = 0; i < byteCount; i++)
	{
		c2WriteDR(rwBuffer[i]);
		POLL_IN_BUSY;
	}

	return 1;
}

uint8_t flashPageErase(uint8_t page)
{
	// set up page erase modus
	if(!flashSetupCommand(PAGE_ERASE))
	{
		return 0;
	}

	POLL_IN_BUSY;

	// target page
	c2WriteDR(page);

	POLL_OUT_READY;

	// read status
	if(c2ReadDR() != STAT_OK)
	{
		return 0;
	}

	// write dummy
	// initiate page erase
	c2WriteDR(0x00);

	POLL_OUT_READY;

	// read status
	if(c2ReadDR() != STAT_OK)
	{
		return 0;
	}

	return 1;
}

uint8_t flashErase()
{
	// setup erase modus
	if(!flashSetupCommand(DEVICE_ERASE))
	{
		return 0;
	}

	// arming sequence
	c2WriteDR(0xDE);
	POLL_IN_BUSY;
	c2WriteDR(0xAD);
	POLL_IN_BUSY;
	c2WriteDR(0xA5);
	POLL_IN_BUSY;

	POLL_OUT_READY;

	// read status
	if(c2ReadDR() != STAT_OK)
	{
		return 0;
	}

	return 1;
}

unsigned int xtoi(const char *hexStr)
{
	unsigned int result = 0;

	if (hexStr)
	{
		char c;
		while ((c = *hexStr++) != '\0')
		{
			if ((c >= '0') && (c <= '9'))
				c -= '0';
			else if ((c >= 'A') && (c <= 'F'))
				c -= 'A' - 10;
			else if ((c >= 'a') && (c <= 'f'))
				c -= 'a' - 10;
			else
				break;
			result = (result * 16) + c;
		}
	}
	return(result);
}

uint8_t blockRead()
{
	printf("br");
	unsigned char actual = getchar();
	unsigned char ihex = 0;
	if(actual == ':')
	{
		ihex = actual;
		actual = getchar();
		printf(":");
	}
	stringHexBuffer[0] = actual;
	stringHexBuffer[1] = getchar();
	stringHexBuffer[2] = 0;

	byteCount = xtoi(stringHexBuffer);

	printf(stringHexBuffer);
	uint8_t i;
	for(i = 0; i < 4; i++)
	{
		stringHexBuffer[i] = getchar();
	}
	stringHexBuffer[4] = 0;
	flashAddress = xtoi(stringHexBuffer);

	printf(stringHexBuffer);

	if(ihex == ':')
	{
		stringHexBuffer[0] = getchar();
		stringHexBuffer[1] = getchar();
		stringHexBuffer[2] = 0;
	}
	if(!flashReadBlock())
	{
		printf(er);
		printf(termSeq);
		return 0;
	}
	else
	{
		printf(ok);
	}
	for(i = 0; i < byteCount; i++)
	{
		printf("%02X", rwBuffer[i]);
	}
	printf(termSeq);
	return 1;
}

uint8_t blockWrite()
{
	unsigned char actual;
	unsigned char ihex = 0;

	printf("bw");
	actual = getchar();
	if(actual == ':')
	{
		ihex = actual;
		actual = getchar();
		printf(":");
	}
	stringHexBuffer[0] = actual;
	stringHexBuffer[1] = getchar();
	stringHexBuffer[2] = 0;

	byteCount = xtoi(stringHexBuffer);

	printf(stringHexBuffer);

	for(unsigned char i=0; i < 4; i++)
	{
		stringHexBuffer[i] = getchar();
	}
	stringHexBuffer[4] = 0;
	flashAddress = xtoi(stringHexBuffer);
	printf(stringHexBuffer);
	if(ihex == ':')
	{
		printf("%c", getchar());
		printf("%c", getchar());
	}
	for(uint8_t i = 0; i < byteCount; i++)
	{
		printf("%c", stringHexBuffer[0] = getchar());
		printf("%c", stringHexBuffer[1] = getchar());
		stringHexBuffer[2] = 0;
		rwBuffer[i] = xtoi(stringHexBuffer);
	}

	if(ihex == ':')
	{
		printf("%c", getchar());
		printf("%c", getchar());
	}


	if(!flashWriteBlock())
	{
		printf(er);
		printf(termSeq);
		return 0;
	}
	printf(ok);
	printf(termSeq);
	return 1;
}

int main(void)
{
	// initial settings for C2 interface
	DDRB |= (1 << DDB3); // C2CK as Output
	PORTB |= (1 << PORTB3); // C2CK signal HIGH

	// init serial interface
	UBRR0H = UBRRH_VALUE;
	UBRR0L = UBRRL_VALUE;
#if USE_2X
	UCSR0A |= _BV(U2X0);
#else
	UCSR0A &= ~(_BV(U2X0));
#endif
	UCSR0C = _BV(UCSZ01) | _BV(UCSZ00); /* 8-bit data */
	UCSR0B = _BV(RXEN0) | _BV(TXEN0);   /* Enable RX and TX */

	fdevopen(&uartPutchar, &uartGetchar);

	char in;
	while(1)
	{
		in = getchar();
		if(in == 'v')
		{
			printf(fw_version);
			printf(termSeq);
		}
		else if(in == 'r')
		{
			c2Reset();
			printf("r");
			printf(ok);
			printf(termSeq);
		}
		else if(in == 'd')
		{
			printf("d");
			printf(ok);
			printf("%02X", c2GetDeviceId());
			printf(termSeq);
		}
		else if(in == 'i')
		{
			flashEnable();
			printf("i");
			printf(ok);
			printf(termSeq);
		}
		else if(in == 'e')
		{
			printf("e");
			if(flashErase())
			{
				printf(ok);
			}
			else
			{
				printf(er);
			}
			printf(termSeq);
		}
		else if(in == 'p')
		{
			printf("p");
			printf("%c", stringHexBuffer[0] = getchar());
			printf("%c", stringHexBuffer[1] = getchar());
			stringHexBuffer[2] = 0;
			if(!flashPageErase(xtoi(stringHexBuffer)))
			{
				printf(er);
			}
			else
			{
				printf(ok);
			}
			printf(termSeq);
		}
		else if(in == 'b')
		{
			in = getchar();
			if(in == 'r')
			{
				blockRead();
			}
			else if(in == 'w')
			{
				blockWrite();
			}
		}
	}
	return (1);
}
