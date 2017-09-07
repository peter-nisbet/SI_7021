#include <stdbool.h>
#include <stdint.h>
#include "FreeRTOS.h"
#include "task.h"
#include "fsl_debug_console.h"

#define Forward true
#define Reverse false

//Definitions//
float last_error, integral_value, Output;
uint64_t lastTime;
float kp, ki, kd;

//Tuning settings for PID//
float Kp = 1;
float Ki = 1;
float Kd = 1;

float target_temp = 24;

uint64_t sampleTime = 1000;

uint8_t max_limit = 20;
int16_t min_limit = -20;

bool Direct = Forward;

//Function Prototypes//
unsigned int PID_Compute(float Input, float target_value, float up_limit, float low_limit, bool direction);
void set_tuning(void);
