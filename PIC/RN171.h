
void UARTInit();
void RN171_SendData(void* Data, int size);
void RN171_SendStr(char* Data);
char RN171_Join();
char RN171_Sleep();

extern char RN171_RxData[256];					//Enough large buffer for receiving string
extern unsigned char RN171_CommandReceived;
extern unsigned char RxDataCounter;
typedef enum
{
	RN171_OK = 0,
	RN171_TIMEOUT,

} RN171_STATES;


typedef enum
{
	RN171_SLEEP = 0,
	RN171_NORMAL,
	RN171_CMD,
	RN171_UNKNOWN
} RN171_STATUS;
