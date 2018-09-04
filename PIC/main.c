#include <p24Exxxx.h>
#include "RN171.h"
#include "BME280.h"
#include "I2C.h"
#include "base64.h"
#include <stdlib.h> 
#include <string.h>
#include <stdio.h>

//************************Definitions*********************************//
#define STATUS_LED LATBbits.LATB14

#define STATION_ID 0	//Weather station ID

#define FCY 59881250LL			//Define clock for delay_ms library
#include <libpic30.h>			//This is need to use delay_ms function, #define FCY must be under #include <libpic.h> otherwise it's not working

_FPOR(ALTI2C1_ON)				//I2C alternativ pin configuration  :RB8, RB9  
_FGS(GWRP_OFF & GCP_OFF)		//Write protect and code protect disabled
_FICD(ICS_PGD1 & JTAGEN_OFF)	//Debug pin select, disable JTAG
_FWDT(FWDTEN_ON & WDTPOST_PS1024 & WDTPRE_PR32 & WINDIS_OFF)		//Enable watchdog, timeout will be around 1,024S
_FOSCSEL(FNOSC_FRC & IESO_OFF);										// Select Internal FRC at POR
_FOSC(FCKSM_CSECMD & OSCIOFNC_OFF  & POSCMD_NONE);  				// Clock switching is enabled,Fail-safe Clock Monitor is disabled
																	// OSC2 Pin Function: OSC2 Clock Output disabled
																	// Primary Oscillator Mode: NONE
								


//************************Function  Declarations*********************************//
void SleepSec(unsigned int Sec);
void init();
void BlinkStatusLed(unsigned char Count);

//************************Global Variable Declarations*********************************//
unsigned char StartConversion = 0;
unsigned char TaskFinished = 0;
char SensorData[255] = "/0";
char* EncodedSensorData;
int EncodedLen;
char status;


//*********************************************************//
void init()
{
	CLKDIV = 0;
	// Configure PLL prescaler, PLL postscaler, PLL divisor, FCY will be 59,881250Mhz
	CLKDIVbits.PLLPOST=0b00; // N2=2
	CLKDIVbits.PLLPRE=0b00; // N1=2
	CLKDIVbits.DOZEN = 0;
	PLLFBD=63; // M=65

	// Initiate Clock Switch to FRC (7,37Mhz)oscillator with PLL (NOSC=0b001)
	__builtin_write_OSCCONH(0x01);
	__builtin_write_OSCCONL(OSCCON | 0x01);
	// Wait for Clock switch to occur
	while (OSCCONbits.COSC!= 0b001);
	// Wait for PLL to lock
	while (OSCCONbits.LOCK!= 1);

   	ANSELA = 0; 	//Disable analog pins
	ANSELB = 0;

	TRISB = 0;		//All ports are output
	TRISA = 0;
	
	//Timer1 init, it is used for clear WDT timer durring normal conditions
	T1CONbits.TON = 0; // Disable Timer
	T1CONbits.TCS = 0; // Select internal instruction cycle clock
	T1CONbits.TGATE = 0; // Disable Gated Timer mode
	T1CONbits.TCKPS = 0b11; // Select 1:256 Prescaler
	TMR1 = 0x00; // Clear timer register
	PR1 = 23391; // Load PR value for ~10mS
	IPC0bits.T1IP = 0x01; // Set Timer 1 Interrupt Priority Level
	IFS0bits.T1IF = 0; // Clear Timer 1 Interrupt Flag
	IEC0bits.T1IE = 1; // Enable Timer1 interrupt
	T1CONbits.TON = 1; // Start Timer

	UARTInit();
	initI2C();
}



int main()
{
	init();
	BlinkStatusLed(1);	//Indicate power on
	BMP280_Init();

	while(1)
	{
		status = RN171_Join();
		if(status == RN171_OK)
		{
			BlinkStatusLed(1);	//Indicate succesfull wifi connection
			StartConversion = 0;
			//Get Temp,Hum,Press
			BMP280_StartForcedMode();
			BMP280_StartForcedMode();
			while(BMP280_IsMeasuring() == 0)
			{
				__delay_ms(10);  
			}
			BMP280_Read_AllData();
			sprintf(SensorData,"{\"temperature\": %f ,\"pressure\": %f, \"humidity\" : %f, \"location\" : %d }", Temperature, Pressure, Humidity, STATION_ID);
			EncodedSensorData = base64_encode(SensorData, strlen(SensorData), &EncodedLen);
			Nop();
			RN171_SendData(EncodedSensorData, EncodedLen);
			free(EncodedSensorData);
		}
		else
		{
		 	BlinkStatusLed(2);
		}
		RN171_Sleep();
		TaskFinished = 1;
		SleepSec(60);
		
	}
	return 1;
}


void BlinkStatusLed(unsigned char Count)
{
	while(Count > 0)
	{
		STATUS_LED = 1;
		__delay_ms(250);
		STATUS_LED = 0;
		Count -= 1;
		if(Count > 0)
			__delay_ms(250);
	}

}


void SleepSec(unsigned int Sec)
{
	static unsigned int SleepCount;
	SleepCount = 0;
	do
	{
		Sleep();
		RCONbits.SLEEP = 0;
		SleepCount++;
	}while(SleepCount < Sec);
}


//*************************Interrupt functions********************************//

void __attribute__((__interrupt__, no_auto_psv)) _T1Interrupt(void)	
{
	IFS0bits.T1IF = 0;
	ClrWdt();
}

void __attribute__((__interrupt__, no_auto_psv)) _U1RXInterrupt(void)	
{
	RN171_RxData[RxDataCounter] = U1RXREG;
	if(RN171_RxData[RxDataCounter] == '\n')
	{
		RN171_CommandReceived = 1;
	}
	if(RN171_RxData[RxDataCounter] != 0)	//Drop zeros
		RxDataCounter++;
	IFS0bits.U1RXIF = 0;
}






