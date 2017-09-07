#include<PID_Controller.h>

void set_tuning(void)
{
	float SampleTimeInSec = sampleTime / 1000;

	kp = Kp;
	kd = Kd / SampleTimeInSec;
	ki = Ki * SampleTimeInSec;
}


unsigned int PID_Compute(float Input, float target_value, float up_limit, float low_limit, bool direction)
{
	float current_value, error, derivative_value;

	//Get current time//
	uint64_t now = xTaskGetTickCount();

	//Get difference in time from previous call//
	uint64_t timeChange = (now - lastTime);

	if(timeChange >= sampleTime)
	{
		//Get current value from target input//
		current_value = Input;

		//Calculate proportional term//
		error = target_value - current_value;

		//Calculate integral term//
		integral_value = integral_value + error;

		//Add limits to integral term//
		if (integral_value > up_limit)
		{
			integral_value = up_limit;
		}
		else if(integral_value < low_limit)
		{
			integral_value = low_limit;
		}

		//Calculate derivative term//
		derivative_value = error - last_error;

		//Store last error value//
		last_error = error;

		//Store last time value//
		lastTime = now;

		//Calculate output through PID equation//
		Output = (Kp * error) + (Ki * integral_value) +  (Kd * derivative_value);

		//Limit output values//
		if(Output > up_limit)
		{
			Output = up_limit;
		}
		else if(Output < low_limit)
		{
			Output = low_limit;
		}

		if (direction == Forward)
		{
			Output = -(Output);
		}
	}

	PRINTF("Value of current value is: %4.2fC\r\n", current_value);
	PRINTF("Value of error is: %4.2f\r\n", error);
	PRINTF("Value of integral is: %4.2f\r\n", integral_value);
	PRINTF("Value of derivative is: %4.2f\r\n", derivative_value);

	return Output;
}
