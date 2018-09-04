#include"p24Exxxx.h"
#include"I2C.h"




void I2CWait ()
{
	while (I2C1CONbits.SEN ==1)
	{
		Nop ();
	}
	while (I2C1CONbits.RSEN ==1)
	{
		Nop ();
	}
	while (I2C1CONbits.PEN ==1)
	{
		Nop ();
	}
	while (I2C1CONbits.ACKEN ==1)
	{
		Nop ();
	}
	while (I2C1CONbits.RCEN ==1)
	{
		Nop ();
	}
	while (I2C1STATbits.TRSTAT ==1)
	{
		Nop ();
	}
	if(I2C1STATbits.BCL == 1)
	{
		//Error handling
	}
	
}

void initI2C()	// I2C1 master mode enable
{
	I2C1RCV = 0;
	I2C1TRN = 0;
	I2C1STAT = 0;
	I2C1CON = 0b1000001000000000;
	I2C1BRG = 400;	//100kHz
	char dummy = I2C1RCV;
}

void I2CStart(unsigned char Address)
{
	I2C1CONbits.SEN = 1;	 
	I2CWait();

	I2C1TRN = Address;
	I2CWait();
}

void I2CReStart(unsigned char Address)
{
	I2C1CONbits.RSEN = 1;			 
	I2CWait();
	I2C1TRN = Address;
	I2CWait();
}

void I2CSend(unsigned char Data)
{
	I2C1TRN = Data;				
	I2CWait();
}


void I2CStop (void)
{
	I2C1CONbits.PEN = 1;			
	I2CWait();
}
