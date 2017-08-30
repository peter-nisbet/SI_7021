#include <SI7021.h>

//***Initialize***//
void si7021_init(void)
{
	uint8_t ID_Temp_Hum = checkID();

	int x = 0;

	if(ID_Temp_Hum == 0x15){
		x = 1;
	}
	else
	{
		x = 0;
	}

	if (x == 1){
		PRINTF("Si7021 Found. \r\n");
	}
	else{
		PRINTF("Si7021 Not Found. Please make sure device is plugged in and functional.\r\n");
	}
}


float getRH()
{
	uint16_t RH_Code = makeMeasurement(HUMD_MEASURE_NOHOLD);
	float result = (125.0*RH_Code/65536)-6;
	return result;
}

float readTemp()
{
	uint16_t temp_Code = makeMeasurement(TEMP_PREV);
	float result = (175.72*temp_Code/65536)-46.85;
	return result;
}

float getTemp()
{
	uint16_t temp_Code = makeMeasurement(TEMP_MEASURE_NOHOLD);
	float result = (175.72*temp_Code/65536)-46.85;
	return result;
}

void heaterOn()
{
	uint8_t regVal = readReg();
	regVal |= _BV(HTRE);
	writeReg(regVal);
}

void heaterOff()
{
	uint8_t regVal = readReg();
	regVal &= ~_BV(HTRE);
	writeReg(regVal);
}

void changeResolution(uint8_t i)
{
	uint8_t regVal = readReg();
	
	regVal &= 0b011111110;
	
	switch(i){
		case 1:
			regVal |= 0b00000001;
			break;
		case 2:
			regVal |= 0b10000000;
			break;
		case 3:
			regVal |= 0b10000001;

		default:
			regVal |= 0b00000000;
			break;
		}
		writeReg(regVal);
}

void reset()
{
	writeReg(SOFT_RESET);
}

uint8_t checkID()
{
	status_t status;
	uint8_t ID_1;
	g_sen_buff[0] = 0xFC;
	g_sen_buff[1] = 0xC9;

	memset(&sensorXfer, 0, sizeof(sensorXfer));
	sensorXfer.slaveAddress = ADDRESS;
	sensorXfer.direction = kI2C_Write;
	sensorXfer.subaddress = 0;
	sensorXfer.subaddressSize = 0;
	sensorXfer.data = g_sen_buff;
	sensorXfer.dataSize = 2;
	sensorXfer.flags = kI2C_TransferDefaultFlag;
	//sensorXfer.flags = kI2C_TransferRepeatedStartFlag;

	status = I2C_RTOS_Transfer(&sensor_rtos_handle, &sensorXfer);
	if(status != kStatus_Success)
    {
        PRINTF("I2C master: error during write transaction, %d\r\n", status);
    }

	for(uint32_t i; i < I2C_DATA_LENGTH; i++)
	{
		g_sen_buff[i] = 0;
	}

	/*comd=0xC9;

	memset(&sensorXfer, 0, sizeof(sensorXfer));
	sensorXfer.slaveAddress = ADDRESS;
	sensorXfer.direction = kI2C_Write;
	sensorXfer.subaddress = 0;
	sensorXfer.subaddressSize = 0;
	sensorXfer.data = comd;
	sensorXfer.dataSize = sizeof(comd);
	sensorXfer.flags = kI2C_TransferDefaultFlag;

	status = I2C_RTOS_Transfer(&sensor_rtos_handle, &sensorXfer);
	if(status != kStatus_Success)
	{
	   PRINTF("I2C master: error during write transaction, %d", status);
	}*/

	memset(g_sen_buff, 0, sizeof(g_sen_buff));
	sensorXfer.slaveAddress = ADDRESS;
	sensorXfer.direction = kI2C_Read;
	sensorXfer.subaddress = 0;
	sensorXfer.subaddressSize = 0;
	sensorXfer.data = g_sen_buff;
	sensorXfer.dataSize = I2C_DATA_LENGTH;
	sensorXfer.flags = kI2C_TransferDefaultFlag;

	status = I2C_RTOS_Transfer(&sensor_rtos_handle, &sensorXfer);

	if(status != kStatus_Success)
	{
		PRINTF("I2C master: error during read transaction, %d", status);
	}

	ID_1 = g_sen_buff[0];

	for(uint32_t i; i < I2C_DATA_LENGTH; i++)
	{
		g_sen_buff[i] = 0;
	}

	return(ID_1);
}

uint16_t makeMeasurement(uint8_t command)
{
	uint16_t nBytes = 3;
	status_t status;
	
	g_sen_buff[0] = command;

	memset(&sensorXfer, 0, sizeof(sensorXfer));
	sensorXfer.slaveAddress = ADDRESS;
	sensorXfer.direction = kI2C_Write;
	sensorXfer.subaddress = 0;
	sensorXfer.subaddressSize = 0;
	sensorXfer.data = g_sen_buff;
	sensorXfer.dataSize = 1;
	sensorXfer.flags = kI2C_TransferDefaultFlag;

	if(command == 0xE0) nBytes = 2;
	
	status = I2C_RTOS_Transfer(&sensor_rtos_handle, &sensorXfer);
	if(status != kStatus_Success)
    {
        PRINTF("I2C master: error during write transaction, %d", status);
    }
	for(uint32_t i; i < I2C_DATA_LENGTH; i++)
	{
		g_sen_buff[i] = 0;
	}

	vTaskDelay(100);

	sensorXfer.slaveAddress = ADDRESS;
	sensorXfer.direction = kI2C_Read;
	sensorXfer.subaddress = 0;
	sensorXfer.subaddressSize = 0;
	sensorXfer.data = g_sen_buff;
	sensorXfer.dataSize = 2;
	//sensorXfer.dataSize = I2C_DATA_LENGTH;
	sensorXfer.flags = kI2C_TransferDefaultFlag;

	status = I2C_RTOS_Transfer(&sensor_rtos_handle, &sensorXfer);

	if(status != kStatus_Success)
	{
		PRINTF("I2C master: error during read transaction, %d", status);
	}


	unsigned int msb = g_sen_buff[0];
	unsigned int lsb = g_sen_buff[1];
	
	unsigned int measurement = msb << 8 | lsb;
	
	return measurement;
}

void writeReg(uint8_t value)
{
	//i2c_rtos_handle_t sensor_rtos_handle;
	status_t status;

	memset(&sensorXfer, 0, sizeof(sensorXfer));
	sensorXfer.slaveAddress = ADDRESS;
	sensorXfer.direction = kI2C_Write;
	sensorXfer.subaddress = 0;
	sensorXfer.subaddressSize = 0;
	sensorXfer.data = WRITE_USER_REG;
	sensorXfer.dataSize = I2C_DATA_LENGTH;
	sensorXfer.flags = kI2C_TransferDefaultFlag;

	status = I2C_RTOS_Transfer(&sensor_rtos_handle, &sensorXfer);

	if(status != kStatus_Success)
	{
		PRINTF("I2C master: error during read transaction, %d", status);
	}

	for (uint32_t i; i < I2C_DATA_LENGTH; i++)
	{
		g_sen_buff[i] = 0;
	}

	sensorXfer.slaveAddress = ADDRESS;
	sensorXfer.direction = kI2C_Write;
	sensorXfer.subaddress = 0;
	sensorXfer.subaddressSize = 0;
	sensorXfer.data = value;
	sensorXfer.dataSize = I2C_DATA_LENGTH;
	sensorXfer.flags = kI2C_TransferDefaultFlag;

	status = I2C_RTOS_Transfer(&sensor_rtos_handle, &sensorXfer);

	if(status != kStatus_Success)
	{
		PRINTF("I2C master: error during read transaction, %d", status);
	}

	for (uint32_t i; i < I2C_DATA_LENGTH; i++)
	{
		g_sen_buff[i] = 0;
	}

}

uint8_t readReg()
{
	status_t status;

	memset(&sensorXfer, 0, sizeof(sensorXfer));
	sensorXfer.slaveAddress = ADDRESS;
	sensorXfer.direction = kI2C_Write;
	sensorXfer.subaddress = 0;
	sensorXfer.subaddressSize = 0;
	sensorXfer.data = READ_USER_REG;
	sensorXfer.dataSize = I2C_DATA_LENGTH;
	sensorXfer.flags = kI2C_TransferDefaultFlag;

	status = I2C_RTOS_Transfer(&sensor_rtos_handle, &sensorXfer);

	if(status != kStatus_Success)
	{
		PRINTF("I2C master: error during read transaction, %d", status);
	}

	for (uint32_t i; i < I2C_DATA_LENGTH; i++)
	{
		g_sen_buff[i] = 0;
	}

	sensorXfer.slaveAddress = ADDRESS;
	sensorXfer.direction = kI2C_Read;
	sensorXfer.subaddress = 0;
	sensorXfer.subaddressSize = 0;
	sensorXfer.data = g_sen_buff;
	sensorXfer.dataSize = I2C_DATA_LENGTH;
	sensorXfer.flags = kI2C_TransferDefaultFlag;

	status = I2C_RTOS_Transfer(&sensor_rtos_handle, &sensorXfer);

	if(status != kStatus_Success)
	{
		PRINTF("I2C master: error during read transaction, %d", status);
	}

	uint8_t regVal = g_sen_buff[0];
	return regVal;

}
