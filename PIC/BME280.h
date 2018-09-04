/* 
 * File:   BME280.h
 * Author: Viktor
 *
 * Created on 27 July 2016, 21:43
 */

#ifndef BME280_H
#define	BME280_H

#ifdef	__cplusplus
extern "C" {
#endif

    
#define uint16_t unsigned int
#define int16_t int
#define uint8_t unsigned char
#define uint32_t unsigned long    
    
    
extern float Temperature;
extern float Pressure;
extern float Humidity;
    
    
void BMP280_Init();
void BMP280_Read_ID();
void BMP280_Read_AllData();
void BMP280_StartForcedMode();
char BMP280_IsMeasuring();





typedef struct _compParams_ts {
  uint16_t dig_T1;
  int16_t  dig_T2;
  int16_t  dig_T3;

  uint16_t dig_P1;
  int16_t  dig_P2;
  int16_t  dig_P3;
  int16_t  dig_P4;
  int16_t  dig_P5;  
  int16_t  dig_P6;  
  int16_t  dig_P7; 
  int16_t  dig_P8;
  int16_t  dig_P9;

  uint8_t  dig_H1;
  int16_t  dig_H2;
  uint8_t  dig_H3;
  int16_t  dig_H4;
  int16_t  dig_H5;
  uint8_t  dig_H6;
  
} compParams_ts; 


typedef struct
{
  uint16_t dig_T1;
  int16_t  dig_T2;
  int16_t  dig_T3;
} TempCompParams;

typedef struct
{
  uint16_t dig_P1;
  int16_t  dig_P2;
  int16_t  dig_P3;
  int16_t  dig_P4;
  int16_t  dig_P5;  
  int16_t  dig_P6;  
  int16_t  dig_P7; 
  int16_t  dig_P8;
  int16_t  dig_P9;
} PressCompParams;

typedef struct
{
    uint8_t  dig_H1;
    int16_t  dig_H2;
    uint8_t  dig_H3;
    int16_t  dig_H4;
    int16_t  dig_H5;
    uint8_t  dig_H6;
} HumCompParams;


#ifdef	__cplusplus
}
#endif

#endif	/* BME280_H */

