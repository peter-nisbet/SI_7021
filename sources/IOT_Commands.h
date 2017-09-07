#include <fsl_uart_freertos.h>

/****Definitions****/
#define IOT_UART UART0
#define IOT_UART_CLKSRC SYS_CLK
#define IOT_UART_CLK_FREQ CLOCK_GetFreq(SYS_CLK)
#define IOT_UART_RX_TX_IRQn UART0_RX_TX_IRQn

/****Variables*****/
uart_rtos_handle_t handle;

uint8_t background_buffer[32];
uint8_t send_buffer[32];
char recv_buffer[1];
char databuf[32];
uint8_t datacnt = 0;

struct _uart_handle t_handle;

uart_rtos_config_t uart_config = {
		.baudrate = 115200,
		.parity = kUART_ParityDisabled,
		.stopbits = kUART_OneStopBit,
		.buffer = background_buffer,
		.buffer_size = sizeof(background_buffer),
};

/****Prototypes****/
void uart_init(void);
void init_IOT(void);
unsigned int IOT_Send(char *dataU, char *commandU);
void IOT_Receive(void);
unsigned int IOT_CommandParse(char *commandU);


/****IOT Definitions****/
#define HIGH 4
#define MED	3
#define LOW 2
#define AUTO 0
#define OFF	1

/****IOT_Variables****/
uint8_t fanControl;

char IOT_Comd[10];
char IOT_Data[6];
