/*
 * Copyright (c) 2015, Freescale Semiconductor, Inc.
 * Copyright 2016-2017 NXP
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 * o Redistributions of source code must retain the above copyright notice, this list
 *   of conditions and the following disclaimer.
 *
 * o Redistributions in binary form must reproduce the above copyright notice, this
 *   list of conditions and the following disclaimer in the documentation and/or
 *   other materials provided with the distribution.
 *
 * o Neither the name of the copyright holder nor the names of its
 *   contributors may be used to endorse or promote products derived from this
 *   software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
 * ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

/*  Standard C Included Files */
#include <string.h>

/* FreeRTOS kernel includes. */
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "timers.h"
#include "semphr.h"

/*  SDK Included Files */
#include "board.h"
#include "fsl_debug_console.h"
#include "fsl_i2c.h"
#include "fsl_i2c_freertos.h"
#include "fsl_ftm.h"

#if ((defined FSL_FEATURE_SOC_INTMUX_COUNT) && (FSL_FEATURE_SOC_INTMUX_COUNT))
#include "fsl_intmux.h"
#endif

#include "pin_mux.h"
#include "clock_config.h"

/****Peter's Sample Library Includes****/
#include <SI7021.h>
#include <PID_Controller.h>
#include <IOT_Commands.h>

/*******************************************************************************
 * Definitions
 ******************************************************************************/
#define EXAMPLE_I2C_MASTER_BASE (I2C0_BASE)
#define EXAMPLE_I2C_MASTER_IRQN (I2C0_IRQn)
#define EXAMPLE_I2C_MASTER_CLK_SRC (I2C0_CLK_SRC)
#define EXAMPLE_I2C_MASTER_CLK_FREQ CLOCK_GetFreq((I2C0_CLK_SRC))

#define EXAMPLE_I2C_MASTER ((I2C_Type *)EXAMPLE_I2C_MASTER_BASE)

#define I2C_BAUDRATE (100000) /* 100K */
#define I2C_DATA_LENGTH (32)  /* MAX is 256 */

#if (__CORTEX_M >= 0x03)
#define I2C_NVIC_PRIO 5
#endif

//****PWM Definitions****//
/* The Flextimer base address/channel used for board */
#define BOARD_FTM_BASEADDR FTM0
#define BOARD_FTM_CHANNEL kFTM_Chnl_7

/* Get source clock for FTM driver */
#define FTM_SOURCE_CLOCK CLOCK_GetFreq(kCLOCK_BusClk)

/*******************************************************************************
 * Prototypes
 ******************************************************************************/
void init_I2C(void);
/*******************************************************************************
 * Variables
 ******************************************************************************/

ftm_config_t ftmInfo;
ftm_chnl_pwm_signal_param_t ftmParam;
ftm_pwm_level_select_t pwmLevel = kFTM_HighTrue;

SemaphoreHandle_t i2c_sem;

uint16_t logi = 0;
float temp_val;
float humidity_val;

bool temp_init = false;

/*******************************************************************************
 * Definitions
 ******************************************************************************/
/* Task priorities. */
//***Added by Peter***//
#define SI7021_task_PRIORITY (configMAX_PRIORITIES - 2)
#define PWM_task_PRIORITY (configMAX_PRIORITIES - 2)
#define IOT_task_PRIORITY (configMAX_PRIORITIES -1)
/*******************************************************************************
 * Prototypes
 ******************************************************************************/

//***Added by Peter***//
static void SI_7021(void *pvParameters);
static void Motor_PWM(void *pvParameters);
static void IOT_Task(void *pvParameters);
void read_hum(void);
void read_temp(void);

/*******************************************************************************
 * Code
 ******************************************************************************/

int main(void)
{
    /* Init board hardware. */
    BOARD_InitPins();
    BOARD_BootClockRUN();
    BOARD_InitDebugConsole();


    //***Added by Peter***//
    PRINTF("This example prints the humidity and temperature readings from the Si7021 humidity and temperature sensor.\r\n");

    if (xTaskCreate(SI_7021, "SI7021_task", configMINIMAL_STACK_SIZE + 90, NULL, SI7021_task_PRIORITY, NULL) != pdPASS)
    {
        PRINTF("Failed to create SI7021 task");
    }

    if (xTaskCreate(Motor_PWM, "MotorPWM_task", configMINIMAL_STACK_SIZE + 60, NULL, PWM_task_PRIORITY, NULL) != pdPASS)
    {
        PRINTF("Failed to create MotorPWM task");
    }

    if (xTaskCreate(IOT_Task, "IOT_task", configMINIMAL_STACK_SIZE + 60, NULL, IOT_task_PRIORITY, NULL) != pdPASS)
    {
        PRINTF("Failed to create IOT task");
    }

    vTaskStartScheduler();
    for (;;)
        ;
}


//***I2C Init function***//
void init_I2C(void)
{
    uint32_t sourceClock;
    status_t status;

    NVIC_SetPriority(EXAMPLE_I2C_MASTER_IRQN, I2C_NVIC_PRIO + 1);

    I2C_MasterGetDefaultConfig(&sensorConfig);
    sensorConfig.baudRate_Bps = I2C_BAUDRATE;
    sourceClock = EXAMPLE_I2C_MASTER_CLK_FREQ;

    status = I2C_RTOS_Init(&sensor_rtos_handle, EXAMPLE_I2C_MASTER, &sensorConfig, sourceClock);
    if (status != kStatus_Success)
    {
        PRINTF("I2C master: error during init, %d", status);
    }
    g_sen_handle = &sensor_rtos_handle.drv_handle;
}

//***Function to get humidity***//
void read_hum(void)
{
	logi++;
	//float humidity_val = getRH();
	humidity_val = getRH();
	PRINTF("Log Entry: %d\r\n", logi);
	PRINTF("Value of Room humidity is: %4.1f%%\r\n", humidity_val);
}

//***Function to get Temperature***//
void read_temp(void)
{
	//float temp_val = getTemp();
	temp_val = getTemp();
	PRINTF("Value of Room Temperature is: %4.1fC\r\n\r\n\r\n", temp_val);
}

//***Task for SI7021***//
static void SI_7021(void *pvParameters)
{
	init_I2C();
	//si7021_init();

	while(1)
	{
		read_hum();
		read_temp();

    	strcpy(IOT_Comd, "Temp:");
    	sprintf(IOT_Data, "%4.1f?", temp_val);

		PRINTF("Command to be sent: %s\r\n", IOT_Comd);
		PRINTF("Data to be sent: %s\r\n", IOT_Data);

		IOT_Send(IOT_Data, IOT_Comd);

		vTaskDelay(5000);
		temp_init = true;
	}

	vTaskSuspend(NULL);
}

//***Function to initalize PWM***//

void init_PWM()
{
    ftmParam.chnlNumber = BOARD_FTM_CHANNEL;
    ftmParam.level = pwmLevel;
    ftmParam.dutyCyclePercent = 100;
    ftmParam.firstEdgeDelayPercent = 0U;

    FTM_GetDefaultConfig(&ftmInfo);

    FTM_Init(BOARD_FTM_BASEADDR, &ftmInfo);
    FTM_SetupPwm(BOARD_FTM_BASEADDR, &ftmParam, 1U, kFTM_EdgeAlignedPwm, 10000U, FTM_SOURCE_CLOCK);
    FTM_StartTimer(BOARD_FTM_BASEADDR, kFTM_SystemClock);

	FTM_UpdatePwmDutycycle(BOARD_FTM_BASEADDR, BOARD_FTM_CHANNEL, kFTM_EdgeAlignedPwm, 0);
	FTM_SetSoftwareTrigger(BOARD_FTM_BASEADDR, true);
}

//***Task for Motor Control PWM***//
static void Motor_PWM(void *pvParameters)
{
	int pwm_out;

	init_PWM();
	set_tuning();

    while(1)
    {
    	/*if(temp_val < 20.0)
    	{
    		FTM_UpdatePwmDutycycle(BOARD_FTM_BASEADDR, BOARD_FTM_CHANNEL, kFTM_EdgeAlignedPwm, 0);
    		FTM_SetSoftwareTrigger(BOARD_FTM_BASEADDR, true);
    		PRINTF("Current Fan Speed: OFF \r\n");
    		//vTaskDelay(5000);
    	}
    	else if(temp_val >= 20.0 && temp_val < 21.0)
    	{
    		FTM_UpdatePwmDutycycle(BOARD_FTM_BASEADDR, BOARD_FTM_CHANNEL, kFTM_EdgeAlignedPwm, 30);
    		FTM_SetSoftwareTrigger(BOARD_FTM_BASEADDR, true);
    		PRINTF("Current Fan Speed: 30%% \r\n");
    		//vTaskDelay(5000);
    	}
    	else if(temp_val >= 21.0 && temp_val < 22.0)
    	{
    		FTM_UpdatePwmDutycycle(BOARD_FTM_BASEADDR, BOARD_FTM_CHANNEL, kFTM_EdgeAlignedPwm, 50);
    		FTM_SetSoftwareTrigger(BOARD_FTM_BASEADDR, true);
    		PRINTF("Current Fan Speed: 50%% \r\n");
    	}
    	else if(temp_val >= 22.0 && temp_val < 23.0)
    	{
    		FTM_UpdatePwmDutycycle(BOARD_FTM_BASEADDR, BOARD_FTM_CHANNEL, kFTM_EdgeAlignedPwm, 70);
    		FTM_SetSoftwareTrigger(BOARD_FTM_BASEADDR, true);
    		PRINTF("Current Fan Speed: 70%% \r\n");
    	}
    	else if(temp_val >= 23.0 && temp_val < 25.0)
    	{
    		FTM_UpdatePwmDutycycle(BOARD_FTM_BASEADDR, BOARD_FTM_CHANNEL, kFTM_EdgeAlignedPwm, 90);
    		FTM_SetSoftwareTrigger(BOARD_FTM_BASEADDR, true);
    		PRINTF("Current Fan Speed: 90%% \r\n");
    	}
    	else
    	{
    		FTM_UpdatePwmDutycycle(BOARD_FTM_BASEADDR, BOARD_FTM_CHANNEL, kFTM_EdgeAlignedPwm, 100);
    		FTM_SetSoftwareTrigger(BOARD_FTM_BASEADDR, true);
    		PRINTF("Current Fan Speed: 100%% \r\n");
    	}

    	vTaskDelay(60000);*/
    	if(fanControl == AUTO)
    	{

    		if (temp_init == true)
    		{
    			PID_Compute(temp_val, target_temp, max_limit, min_limit, Direct);

    			pwm_out = Output *15;

    			if(pwm_out > 255)
    			{
    				pwm_out = 255;
    			}

    			else if(pwm_out < 0)
    			{
    				pwm_out = 0;
    			}

    			FTM_UpdatePwmDutycycle(BOARD_FTM_BASEADDR, BOARD_FTM_CHANNEL, kFTM_EdgeAlignedPwm, pwm_out);
    			FTM_SetSoftwareTrigger(BOARD_FTM_BASEADDR, true);

    	    	strcpy(IOT_Comd, "Fan Speed:");
    	    	sprintf(IOT_Data, "%d?", pwm_out);

    			PRINTF("Command to be sent: %s\r\n", IOT_Comd);
    			PRINTF("Data to be sent: %s\r\n", IOT_Data);

    			IOT_Send(IOT_Data, IOT_Comd);

    			PRINTF("Value of PWM Output is: %d\r\n", pwm_out);
    		}
    	}
    	else if(fanControl == LOW)
    	{
    		pwm_out = 80;
			FTM_UpdatePwmDutycycle(BOARD_FTM_BASEADDR, BOARD_FTM_CHANNEL, kFTM_EdgeAlignedPwm, pwm_out);
			FTM_SetSoftwareTrigger(BOARD_FTM_BASEADDR, true);

	    	strcpy(IOT_Comd, "Fan Speed:");
	    	strcpy(IOT_Data, "Low?");
			IOT_Send(IOT_Data, IOT_Comd);

			PRINTF("Value of PWM Output is: %d\r\n", pwm_out);
    	}
    	else if(fanControl == MED)
    	{
    		pwm_out = 160;
			FTM_UpdatePwmDutycycle(BOARD_FTM_BASEADDR, BOARD_FTM_CHANNEL, kFTM_EdgeAlignedPwm, pwm_out);
			FTM_SetSoftwareTrigger(BOARD_FTM_BASEADDR, true);

	    	strcpy(IOT_Comd, "Fan Speed:");
	    	strcpy(IOT_Data, "Medium?");
			IOT_Send(IOT_Data, IOT_Comd);

			PRINTF("Value of PWM Output is: %d\r\n", pwm_out);
    	}
    	else if(fanControl == HIGH)
    	{
    		pwm_out = 255;
			FTM_UpdatePwmDutycycle(BOARD_FTM_BASEADDR, BOARD_FTM_CHANNEL, kFTM_EdgeAlignedPwm, pwm_out);
			FTM_SetSoftwareTrigger(BOARD_FTM_BASEADDR, true);

	    	strcpy(IOT_Comd, "Fan Speed:");
	    	strcpy(IOT_Data, "High?");
			IOT_Send(IOT_Data, IOT_Comd);

			PRINTF("Value of PWM Output is: %d\r\n", pwm_out);
    	}
    	else if(fanControl == OFF)
    	{
    		pwm_out = 0;
			FTM_UpdatePwmDutycycle(BOARD_FTM_BASEADDR, BOARD_FTM_CHANNEL, kFTM_EdgeAlignedPwm, pwm_out);
			FTM_SetSoftwareTrigger(BOARD_FTM_BASEADDR, true);

	    	strcpy(IOT_Comd, "Fan Speed:");
	    	strcpy(IOT_Data, "Off?");
			IOT_Send(IOT_Data, IOT_Comd);

			PRINTF("Value of PWM Output is: %d\r\n", pwm_out);
    	}

		PRINTF("Command to be sent: %s\r\n", IOT_Comd);
		PRINTF("Data to be sent: %s\r\n", IOT_Data);

		vTaskDelay(7000);

    }
}

/****IOT Tasks****/
static void IOT_Task(void *pvParameters)
{
	//char IOT_Comd[10] = "Fan Speed:";
	//char IOT_Data[10] = "Off?";
	init_IOT();

	while(1)
	{
		//PRINTF("Command to be sent: %s\r\n", IOT_Comd);
		//PRINTF("Data to be sent: %s\r\n", IOT_Data);
		memset(recv_buffer, 0, sizeof(recv_buffer));
		memset(databuf, 0, sizeof(databuf));
		//IOT_Send(IOT_Data, IOT_Comd);
		//vTaskDelay(10);
		while(strcmp(recv_buffer, "?")!=0)
		{
			IOT_Receive();
			datacnt++;
		}
		datacnt = 0;
		PRINTF("Data Received is: %s\r\n", databuf);
		IOT_CommandParse(databuf);
		vTaskDelay(1000);
	}
}
