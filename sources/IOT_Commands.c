#include <IOT_Commands.h>

void init_IOT(void)
{
	uart_init();
}

unsigned int IOT_Send(char *dataU, char *commandU)
{
	memset(send_buffer, 0, sizeof(send_buffer));
	strcpy(send_buffer, commandU);

	if(kStatus_Success > UART_RTOS_Send(&handle, send_buffer, strlen(send_buffer)))
	{
		vTaskSuspend(NULL);
	}

	memset(send_buffer, 0, sizeof(send_buffer));
	strcpy(send_buffer, dataU);

	if(kStatus_Success > UART_RTOS_Send(&handle, send_buffer, strlen(send_buffer)))
	{
		vTaskSuspend(NULL);
	}

	return 0;
}

void IOT_Receive(void)
{
	int error;
	size_t n;

	error = UART_RTOS_Receive(&handle, recv_buffer, sizeof(recv_buffer), &n);

	if(error != kStatus_Success)
	{
		vTaskSuspend(NULL);
	}


	databuf[datacnt] = *recv_buffer;
	//strncpy(databuf, recv_buffer, sizeof(recv_buffer));

	//asm("nop");
}

unsigned int IOT_CommandParse(char *commandU)
{
	if(strncmp(commandU, "Fan Speed:High", 14) == 0)
	{
		fanControl = HIGH;
	}
	else if(strncmp(commandU, "Fan Speed:Medium", 16) == 0)
	{
		fanControl = MED;
	}
	else if(strncmp(commandU, "Fan Speed:Low", 13) == 0)
	{
		fanControl = LOW;
	}
	else if(strncmp(commandU, "Fan Speed:Auto", 14) == 0)
	{
		fanControl = AUTO;
	}
	else if(strncmp(commandU, "Fan Speed:Off", 13) == 0)
	{
		fanControl = OFF;
	}
	return 0;
}

/****Init UART****/

void uart_init(void)
{
	uart_config.srcclk = IOT_UART_CLK_FREQ;
	uart_config.base = IOT_UART;
	NVIC_SetPriority(IOT_UART_RX_TX_IRQn, 5);

	if (0 > UART_RTOS_Init(&handle, &t_handle, &uart_config))
	{
		vTaskSuspend(NULL);
	}

}
