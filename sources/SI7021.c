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
			break;	// if there is an issue remove as this wasn't in the arduino code.
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
	
	if(command == 0xE0) nBytes = 2;
	
	unsigned int msb;
	unsigned int lsb;
	
	unsigned int measurement = msb << 8 | lsb;
	
	return measurement;
}

void writeReg(uint8_t value)
{

}

uint8_t readReg()
{

	uint8_t regVal;
	return regVal;
}
