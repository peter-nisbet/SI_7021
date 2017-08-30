/**Header file for SI7021 humidity and temperature sensor**/
	

/****I2C Slave Address****/
#include <FreeRTOS.h>
#include <fsl_i2c_freertos.h>
#include <fsl_debug_console.h>

#define ADDRESS		0x40

/****Command Definitions****/

#define TEMP_MEASURE_HOLD	0xE3
#define	HUMD_MEASURE_HOLD	0xE5
#define TEMP_MEASURE_NOHOLD	0xF3
#define HUMD_MEASURE_NOHOLD	0xF5
#define TEMP_PREV	0xE0

#define WRITE_USER_REG	0xE6
#define READ_USER_REG	0xE7
#define SOFT_RESET	0xFE

#define HTRE 0x02
#define _BV(bit) (1<<(bit))

/****I2C Definitions****/

#define I2C_DATA_LENGTH (32)

/****Function Prototypes****/

void si7021_init(void);

float getRH();
float readTemp();
float getTemp();

void heaterOn(void);
void heaterOff(void);
void changeResolution(uint8_t i);
void reset();
uint8_t checkID();

uint16_t makeMeasurement(uint8_t command);
void writeReg(uint8_t value);
uint8_t readReg();


/***Global Definitions***/

i2c_rtos_handle_t sensor_rtos_handle; //might not be needed as global variable
i2c_master_handle_t *g_sen_handle;
i2c_master_config_t	sensorConfig;
i2c_master_transfer_t sensorXfer;

uint8_t g_sen_buff[I2C_DATA_LENGTH];

