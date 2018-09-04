#include"p24Exxxx.h"
#include "RN171.h"
#include <string.h>

#define FCY 5988125LL            //Órajel definiálása
#include <libpic30.h>


char *ptr;        
char RN171_RxData[256] = {"\0"};					//Enough large buffer for receiving string from Wifly
unsigned char RxDataCounter = 0;
unsigned char RN171_CommandReceived;
int timeout = 0;

RN171_STATUS wifiStatus;

char RN171_WakeUp();


void UARTInit()
{
    U1BRG = 389;                	//Baudrate 9600 
	U1MODEbits.UARTEN = 1;      	//UART Enable
    U1STAbits.UTXEN = 1;        	//UART TX Enable
	RPOR1bits.RP36R = 0b000001;		//UART TX RP36/RB4
	RPINR18bits.U1RXR = 0b0010100;	//UART RX 
	TRISAbits.TRISA4 = 1;			//Input RX pin RP20/RA4
	IPC2bits.U1RXIP = 1;			//Interrupt priority
	IFS0bits.U1RXIF = 0;			//Clear interrupt flag
	IEC0bits.U1RXIE = 1;			//Enable receive interrupt
	
	wifiStatus = RN171_NORMAL;
}



void RN171_SendData(void* Data, int size)
{
	char* ptr = (char*)Data;
	int i;
	for(i = 0; i < size; i++)       
    {
        while(U1STAbits.UTXBF);
        U1TXREG = *ptr;
        ptr++;    
    }

}

void RN171_SendStr(char* Data)
{
	char* ptr = Data;
	while(*ptr != 0)     
    {
        while(U1STAbits.UTXBF);
        U1TXREG = *ptr;
        ptr++;    
    }

}

char RN171_CMDMode()
{
	char *n;
	if(wifiStatus == RN171_SLEEP)
	{
		RN171_WakeUp();
	}
	RxDataCounter = 0;
	memset(RN171_RxData, 0, sizeof(RN171_RxData));
	if(wifiStatus != RN171_CMD)
	{
		__delay_ms(500);
		RN171_SendStr("$$$");
		do
		{
			__delay_ms(10);
			n = strstr(RN171_RxData, "$$$");
			if(n > 0)	//Already in CMD mode
			{
				RN171_SendStr("\r\n");
				wifiStatus = RN171_CMD;
				return RN171_OK;
			}
			n = strstr(RN171_RxData, "CMD\r\n");
			timeout++;
			if(timeout == 500)
				return RN171_TIMEOUT;
		}while(n == 0);
	}
	wifiStatus = RN171_CMD;
	return RN171_OK;
}


char RN171_Join()
{
	timeout = 0;
	RxDataCounter = 0;
	memset(RN171_RxData, 0, sizeof(RN171_RxData));
	char *n;
	if(RN171_CMDMode() != RN171_OK)
		return RN171_TIMEOUT;
	RN171_SendStr("join\r");
	//Wait for join to network
	do
	{
		__delay_ms(10);
		n = strstr(RN171_RxData, "Associated!\r");
		timeout++;
		if(timeout == 2000)	//20sec
			return RN171_TIMEOUT;
	}while(n == 0);
	timeout = 0;	
	//Wait for DHCP
	do
	{
		__delay_ms(10);
		n = strstr(RN171_RxData, "Listen");
		timeout++;
		if(timeout == 2000)	//20sec
			return RN171_TIMEOUT;
	}while(n == 0);
	memset(RN171_RxData, 0, sizeof(RN171_RxData));
	RxDataCounter = 0;
	timeout = 0;
	RN171_SendStr("exit\r");
	do
	{
		__delay_ms(10);
		n = strstr(RN171_RxData, "EXIT\r\n");
		timeout++;
		if(timeout == 500)
		{
			Nop();
			return RN171_TIMEOUT;
		}
	}while(n == 0);
	wifiStatus = RN171_NORMAL;
	return RN171_OK;
}

char RN171_WakeUp()
{
	char* n;
	timeout = 0;
	RxDataCounter = 0;
	memset(RN171_RxData, 0, sizeof(RN171_RxData));
	RxDataCounter = 0;
	//Send dummy data to wake up
	U1TXREG = 255;
	while(U1STAbits.UTXBF);
	
	do
	{
		__delay_ms(1);
		n = strstr(RN171_RxData, "*READY*");
		timeout++;
		if(timeout == 2000)
			return RN171_TIMEOUT;
	}while(n == 0);
	wifiStatus = RN171_NORMAL;
	return RN171_OK;
}


char RN171_Sleep()
{
	char *n;
	
	if(RN171_CMDMode() != RN171_OK)
		return RN171_TIMEOUT;
	timeout = 0;
	RxDataCounter = 0;
	memset(RN171_RxData, 0, sizeof(RN171_RxData));
	RN171_SendStr("sleep\r");
	do
	{
		__delay_ms(10);
		n = strstr(RN171_RxData, "sleep");
		timeout++;
		if(timeout == 500)
		{	
			wifiStatus = RN171_UNKNOWN;		//Fix already in CMD mode error when sleep command return incorrectly from wifi
			return RN171_TIMEOUT;
		}
	}while(n == 0);
	wifiStatus = RN171_SLEEP;
	return RN171_OK;
}



