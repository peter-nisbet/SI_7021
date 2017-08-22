#include <SI7021.h>


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

uint16_t makeMeasurement(uint8_t command)
{
	uint16_t nBytes = 3;
	//i2c_rtos_handle_t sensor_rtos_handle;
	status_t status;
	
	memset(&sensorXfer, 0, sizeof(sensorXfer));
	sensorXfer.slaveAddress = ADDRESS;
	sensorXfer.direction = kI2C_Write;
	sensorXfer.subaddress = 0;
	sensorXfer.subaddressSize = 0;
	sensorXfer.data = command;
	sensorXfer.dataSize = I2C_DATA_LENGTH;
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
