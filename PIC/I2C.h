//I2C.h


void I2CWait ();
void initI2C();
void I2CStart(unsigned char Address);
void I2CReStart(unsigned char Address);
void I2CSend(unsigned char Data);
void I2CStop (void);

