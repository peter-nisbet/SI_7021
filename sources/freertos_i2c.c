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

#if ((defined FSL_FEATURE_SOC_INTMUX_COUNT) && (FSL_FEATURE_SOC_INTMUX_COUNT))
#include "fsl_intmux.h"
#endif

#include "pin_mux.h"
#include "clock_config.h"

/****Peter's Sample Library Includes****/
#include <SI7021.h>

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
#else
#define I2C_NVIC_PRIO 2
#endif
#ifdef __GIC_PRIO_BITS
#define I2C_GIC_PRIO 25
#endif
/*******************************************************************************
 * Prototypes
 ******************************************************************************/
void init_I2C(void);
/*******************************************************************************
 * Variables
 ******************************************************************************/

SemaphoreHandle_t i2c_sem;

/*******************************************************************************
 * Definitions
 ******************************************************************************/
/* Task priorities. */
//***Added by Peter***//
#define SI7021_task_PRIORITY (configMAX_PRIORITIES - 1)
/*******************************************************************************
 * Prototypes
 ******************************************************************************/

//***Added by Peter***//
static void SI_7021(void *pvParameters);
void read_hum(void);

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

    if (xTaskCreate(SI_7021, "SI7021_task", configMINIMAL_STACK_SIZE + 60, NULL, SI7021_task_PRIORITY, NULL) != pdPASS)
    {
        PRINTF("Failed to create SI7021 task");
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
	float humidity_val = getRH();
	PRINTF("Value of Room humidity is: %4.1f%%\r\n", humidity_val);
}

void read_temp(void)
{
	float temp_val = getTemp();
	PRINTF("Value of Room Temperature is: %4.1fC\r\n", temp_val);
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
		vTaskDelay(10000);
	}

	vTaskSuspend(NULL);
}
