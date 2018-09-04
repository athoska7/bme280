#include "BME280.h"
#include"p24Exxxx.h"
#include"I2C.h"

#define FCY 5988125LL            //Define clock for delay_ms library
#include <libpic30.h>            //This is need to use delay_ms function, #define FCY must be under #include <libpic.h> otherwise it's not working


long t_fine;
TempCompParams TempCompData;
PressCompParams PressCompData;
HumCompParams HumCompData;
float Temperature;
float Pressure;
float Humidity;
unsigned char CompData [42]; 

long BMP280_ConvertTemperature(long adc_T)
{
    long var1, var2, T;
    var1 = ((((adc_T>>3) -((long)TempCompData.dig_T1<<1))) * ((long)TempCompData.dig_T2)) >> 11;
    var2 = (((((adc_T>>4) - ((long)TempCompData.dig_T1)) * ((adc_T>>4) - ((long)TempCompData.dig_T1))) >> 12) * ((long)TempCompData.dig_T3)) >> 14;
    t_fine = var1 + var2;
    T = (t_fine * 5 + 128) >> 8;
    return T;
}

// Returns pressure in Pa as unsigned 32 bit integer. Output value of ?96386? equals 96386 Pa = 963.86 hPa
unsigned long BMP280_ConvertPressure(long adc_P)
{
    long var1, var2;
    unsigned long p;
    var1 = (((long)t_fine)>>1) - (long)64000;
    var2 = (((var1>>2) * (var1>>2)) >> 11 ) * ((long)PressCompData.dig_P6);
    var2 = var2 + ((var1*((long)PressCompData.dig_P5))<<1);
    var2 = (var2>>2)+(((long)PressCompData.dig_P4)<<16);
    var1 = (((PressCompData.dig_P3 * (((var1>>2) * (var1>>2)) >> 13 )) >> 3) + ((((long)PressCompData.dig_P2) * var1)>>1))>>18;
    var1 =((((32768+var1))*((long)PressCompData.dig_P1))>>15);
    if (var1 == 0) {
      return 0; // avoid exception caused by division by zero
    }
    p = (((unsigned long)(((long)1048576)-adc_P)-(var2>>12)))*3125;
    if (p < 0x80000000) {
      p = (p << 1) / ((unsigned long)var1);
    } else {
      p = (p / (unsigned long)var1) * 2;
    }
    var1 = (((long)PressCompData.dig_P9) * ((long)(((p>>3) * (p>>3))>>13)))>>12;
    var2 = (((long)(p>>2)) * ((long)PressCompData.dig_P8))>>13;
    p = (unsigned long)((long)p + ((var1 + var2 + PressCompData.dig_P7) >> 4));
    return p;
}

// Returns humidity in %RH as unsigned 32 bit integer in Q22.10 format (22 integer and 10 fractional bits).
// Output value of ?47445? represents 47445/1024 = 46.333 %RH
unsigned long BMP280_ConvertHumidity(long adc_H)
{
    long v_x1_u32r;
    v_x1_u32r = (t_fine - ((long)76800));
    v_x1_u32r = (((((adc_H << 14) - (((long)HumCompData.dig_H4) << 20) - (((long)HumCompData.dig_H5) * v_x1_u32r)) +
      ((long)16384)) >> 15) * (((((((v_x1_u32r * ((long)HumCompData.dig_H6)) >> 10) * (((v_x1_u32r *
      ((long)HumCompData.dig_H3)) >> 11) + ((long)32768))) >> 10) + ((long)2097152)) *
      ((long)HumCompData.dig_H2) + 8192) >> 14));
    v_x1_u32r = (v_x1_u32r - (((((v_x1_u32r >> 15) * (v_x1_u32r >> 15)) >> 7) * ((long)HumCompData.dig_H1)) >> 4));
    v_x1_u32r = (v_x1_u32r < 0 ? 0 : v_x1_u32r);
    v_x1_u32r = (v_x1_u32r > 419430400 ? 419430400 : v_x1_u32r);
    return (unsigned long)(v_x1_u32r>>12);
}


void BMP280_Read_ID()
{
	unsigned char temp[6];
    I2CStart(0xEC); 	
	I2CSend(0xF4);
	I2CReStart(0xEC | 0b00000001);	//When read gyro sens, set BIT0 !
	I2C1CONbits.RCEN = 1;
	I2CWait();
	temp[0]= I2C1RCV;
	I2C1CONbits.ACKDT = 1;	//Send NACK
	I2C1CONbits.ACKEN = 1;
	I2CWait();
	I2C1CONbits.ACKDT = 0;
	I2CStop();    
}

void BMP280_Init()
{
     I2CStart(0xEC); 	
     I2CSend(0xF2);
     I2CSend(0x01);
     I2CStop();
     __delay_ms(1);
     I2CStart(0xEC); 	
     I2CSend(0x88);
     I2CReStart(0xEC | 0b00000001);
     int count;
     for (count = 0; count < 25; count++)
     {
        I2C1CONbits.RCEN = 1;
        I2CWait();
        CompData[count]= I2C1RCV;
        I2C1CONbits.ACKDT = 0;	//Send ACK
        I2C1CONbits.ACKEN = 1;
        I2CWait();
     }
    I2C1CONbits.RCEN = 1;
    I2CWait();
    CompData[25]= I2C1RCV;
    I2C1CONbits.ACKDT = 1;	//Send NACK
	I2C1CONbits.ACKEN = 1;
	I2CWait();
	I2C1CONbits.ACKDT = 0;
	I2CStop();
    __delay_ms(1);
     I2CStart(0xEC); 	
     I2CSend(0xE1);
     I2CReStart(0xEC | 0b00000001);
     for (count = 26; count < 41; count++)
     {
        I2C1CONbits.RCEN = 1;
        I2CWait();
        CompData[count]= I2C1RCV;
        I2C1CONbits.ACKDT = 0;	//Send ACK
        I2C1CONbits.ACKEN = 1;
        I2CWait();
     }
    I2C1CONbits.RCEN = 1;
    I2CWait();
    CompData[41]= I2C1RCV;
    I2C1CONbits.ACKDT = 1;	//Send NACK
	I2C1CONbits.ACKEN = 1;
	I2CWait();
	I2C1CONbits.ACKDT = 0;
	I2CStop();
    
    memcpy(&TempCompData,CompData,6);
    memcpy(&PressCompData,&CompData[6],18);
    //memcpy(&HumCompData,&CompData[24],10);
    HumCompData.dig_H1 = CompData[25];
    HumCompData.dig_H2 = CompData[26];
    HumCompData.dig_H2 |= (uint16_t)CompData[27] << 8;
    HumCompData.dig_H3 = CompData[28];
    HumCompData.dig_H4 = (uint16_t)CompData[29] << 4; // bits 11:4
    HumCompData.dig_H4 |= CompData[30] & 0b00001111; // bits 11:4
    HumCompData.dig_H5 = CompData[30] >> 4;
    HumCompData.dig_H5 |= CompData[31] << 4;
    HumCompData.dig_H6 = CompData[32];
  
}

void BMP280_StartForcedMode()
{
     I2CStart(0xEC); 	
     I2CSend(0xF4);
     I2CSend(0x25);
     I2CStop();    
}

char BMP280_IsMeasuring()
{
    unsigned char temp[1];
    I2CStart(0xEC); 	
	I2CSend(0xF3);
	I2CReStart(0xEC | 0b00000001);	//When read gyro sens, set BIT0 !
	I2C1CONbits.RCEN = 1;
	I2CWait();
	temp[0]= I2C1RCV;
	I2C1CONbits.ACKDT = 1;	//Send NACK
	I2C1CONbits.ACKEN = 1;
	I2CWait();
	I2C1CONbits.ACKDT = 0;
	I2CStop();    
    if (temp[0] & 0b00001000)
    {  
        return 1;
    } 
    else
    {
      return 0;
    }
}

void BMP280_Read_AllData()
{
	unsigned char temp[8];
    I2CStart(0xEC); 	
	I2CSend(0xF7);
	I2CReStart(0xEC | 0b00000001);	//When read gyro sens, set BIT0 !
	I2C1CONbits.RCEN = 1;
	I2CWait();
	temp[0]= I2C1RCV;
    I2C1CONbits.ACKEN = 1;
	I2CWait();
	I2C1CONbits.RCEN = 1;
	I2CWait();
	temp[1]= I2C1RCV;
    I2C1CONbits.ACKEN = 1;
	I2CWait();
	I2C1CONbits.RCEN = 1;
	I2CWait();
	temp[2]= I2C1RCV;
    I2C1CONbits.ACKEN = 1;
	I2CWait();
	I2C1CONbits.RCEN = 1;
	I2CWait();
	temp[3]= I2C1RCV;
    I2C1CONbits.ACKEN = 1;
	I2CWait();
	I2C1CONbits.RCEN = 1;
	I2CWait();
	temp[4]= I2C1RCV;
    I2C1CONbits.ACKEN = 1;
	I2CWait();
	I2C1CONbits.RCEN = 1;
	I2CWait();
	temp[5]= I2C1RCV;
    I2C1CONbits.ACKEN = 1;
	I2CWait();
	I2C1CONbits.RCEN = 1;
	I2CWait();
	temp[6]= I2C1RCV;
    I2C1CONbits.ACKEN = 1;
	I2CWait();
	I2C1CONbits.RCEN = 1;
	I2CWait();
	temp[7]= I2C1RCV;
	I2C1CONbits.ACKDT = 1;	//Send NACK
	I2C1CONbits.ACKEN = 1;
	I2CWait();
	I2C1CONbits.ACKDT = 0;
	I2CStop();  
    unsigned long adc_t = 0;
    unsigned long adc_p = 0;
    unsigned long adc_h = 0;
    adc_t  = (unsigned long)temp[5] >> 4;
    adc_t |= (unsigned long)temp[4] << 4;
    adc_t |= (unsigned long)temp[3] << 12;
    adc_p  = (uint32_t)temp[2] >> 4;
    adc_p |= (uint32_t)temp[1] << 4;
    adc_p |= (uint32_t)temp[0] << 12;
    adc_h = temp[7];
    adc_h |= (uint32_t)temp[6] << 8;
    Temperature = (float)BMP280_ConvertTemperature(adc_t) / 100;
    Pressure = (float)BMP280_ConvertPressure(adc_p) / 100;
    Humidity = (float)BMP280_ConvertHumidity(adc_h) / 1024;
    Nop();
}

