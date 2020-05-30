#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/pgmspace.h>
#include <util/delay.h>
#include <stdlib.h>

//Velicina prijemnog bafera (mora biti 2^n)
#define USART_RX_BUFFER_SIZE 64
#define BR_KORISNIKA 10

char Rx_Buffer[USART_RX_BUFFER_SIZE];			//prijemni FIFO bafer
volatile unsigned char Rx_Buffer_Size = 0;	//broj karaktera u prijemnom baferu
volatile unsigned char Rx_Buffer_First = 0;
volatile unsigned char Rx_Buffer_Last = 0;

ISR(USART_RX_vect)
{
  	Rx_Buffer[Rx_Buffer_Last++] = UDR0;		//ucitavanje primljenog karaktera
	Rx_Buffer_Last &= USART_RX_BUFFER_SIZE - 1;	//povratak na pocetak u slucaju prekoracenja
	if (Rx_Buffer_Size < USART_RX_BUFFER_SIZE)
		Rx_Buffer_Size++;					//inkrement brojaca primljenih karaktera
}

void usartInit(unsigned long baud)
{
	UCSR0A = 0x00;	//inicijalizacija indikatora
					//U2Xn = 0: onemogucena dvostruka brzina
					//MPCMn = 0: onemogucen multiprocesorski rezim

	UCSR0B = 0x98;	//RXCIEn = 1: dozvola prekida izavanog okoncanjem prijema
					//RXENn = 1: dozvola prijema
					//TXENn = 1: dozvola slanja

	UCSR0C = 0x06;	//UMSELn[1:0] = 00: asinroni rezim
					//UPMn[1:0] = 00: bit pariteta se ne koristi
					//USBSn = 0: koristi se jedan stop bit
					//UCSzn[2:0] = 011: 8bitni prenos

	UBRR0 = F_CPU / (16 * baud) - 1;

	sei();	//I = 1 (dozvola prekida)
}

unsigned char usartAvailable()
{
	return Rx_Buffer_Size;		//ocitavanje broja karaktera u prijemnom baferu
}

void usartPutChar(char c)
{
	while(!(UCSR0A & 0x20));	//ceka da se setuje UDREn (indikacija da je predajni bafer prazan)
	UDR0 = c;					//upis karaktera u predajni bafer
}

void usartPutString(char *s)
{
	while(*s != 0)				//petlja se izvrsava do nailaska na nul-terminator
	{
		usartPutChar(*s);		
		s++;					
	}
}

void usartPutString_P(const char *s)
{
	while (1)
	{
		char c = pgm_read_byte(s++);	
		if (c == '\0')					
			return;						
		usartPutChar(c);				
	}
}

char usartGetChar()
{
	char c;

	if (!Rx_Buffer_Size)						
		return -1;
  	c = Rx_Buffer[Rx_Buffer_First++];			
	Rx_Buffer_First &= USART_RX_BUFFER_SIZE - 1;	
	Rx_Buffer_Size--;							

	return c;
}

unsigned char usartGetString(char *s)
{
	unsigned char len = 0;

	while(Rx_Buffer_Size) 			
		s[len++] = usartGetChar();	

	s[len] = 0;						
	return len;						
}

char korisnici[BR_KORISNIKA][32] = {
	"Vladimir Marcis",
  	"Petar Ranic",
  	"Petar Petrovic",
  	"Josip Broz",
  	"Vladimir Ilic",
  	"Milan Lukic",
  	"Jovan Sotirov",
  	"Milos Ilic",
  	"Boris Prpos"
};

char PINOVI[BR_KORISNIKA][5] = {"3325", "2244", "7475", "6664", "6569", "4887", "5635", "3321", "3457", "5545"};

int main()
{
	usartInit(9600);

	while(1)
	{
    	usartPutString("Unesite ime i prezime:\r\n");
      	while(!usartAvailable()){}
      	_delay_ms(100);
      	char unos_ime[32];
      	usartGetString(unos_ime);

      	int provera = 0;
      	for(int i = 0; i < BR_KORISNIKA; i++){
      		if(!strcmp(unos_ime, korisnici[i]))
            	provera = 1;
      	}
	if(provera==1){
		usartPutString("Unesite svoj PIN:\r\n");
		char PIN[32];
		//if

	} else {
		usartPutString("Ne postojite u bazi korisnika.\r\n");
	}
      	
	}

	return 0;
}