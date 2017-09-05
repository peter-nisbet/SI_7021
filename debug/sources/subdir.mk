################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../sources/PID_Controller.c \
../sources/SI7021.c \
../sources/board.c \
../sources/clock_config.c \
../sources/freertos_i2c.c \
../sources/pin_mux.c 

OBJS += \
./sources/PID_Controller.o \
./sources/SI7021.o \
./sources/board.o \
./sources/clock_config.o \
./sources/freertos_i2c.o \
./sources/pin_mux.o 

C_DEPS += \
./sources/PID_Controller.d \
./sources/SI7021.d \
./sources/board.d \
./sources/clock_config.d \
./sources/freertos_i2c.d \
./sources/pin_mux.d 


# Each subdirectory must supply rules for building sources it contributes
sources/PID_Controller.o: C:/Users/pnisbet/Documents/Freescale_Projects/SI_7021/sources/PID_Controller.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross ARM C Compiler'
	arm-none-eabi-gcc -mcpu=cortex-m4 -mthumb -mfloat-abi=hard -mfpu=fpv4-sp-d16 -O0 -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -fno-common -ffreestanding -fno-builtin -Wall  -g -DDEBUG -DCPU_MK22FN512VLH12 -DFSL_RTOS_FREE_RTOS -DFRDM_K22F -DFREEDOM -I../CMSIS/Include -I../devices -I../drivers -I../freertos/Source/include -I../freertos/Source/portable/GCC/ARM_CM4F -I../freertos/Source -I../sources -I../utilities -std=gnu99 -mapcs -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

sources/SI7021.o: C:/Users/pnisbet/Documents/Freescale_Projects/SI_7021/sources/SI7021.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross ARM C Compiler'
	arm-none-eabi-gcc -mcpu=cortex-m4 -mthumb -mfloat-abi=hard -mfpu=fpv4-sp-d16 -O0 -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -fno-common -ffreestanding -fno-builtin -Wall  -g -DDEBUG -DCPU_MK22FN512VLH12 -DFSL_RTOS_FREE_RTOS -DFRDM_K22F -DFREEDOM -I../CMSIS/Include -I../devices -I../drivers -I../freertos/Source/include -I../freertos/Source/portable/GCC/ARM_CM4F -I../freertos/Source -I../sources -I../utilities -std=gnu99 -mapcs -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

sources/%.o: ../sources/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross ARM C Compiler'
	arm-none-eabi-gcc -mcpu=cortex-m4 -mthumb -mfloat-abi=hard -mfpu=fpv4-sp-d16 -O0 -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -fno-common -ffreestanding -fno-builtin -Wall  -g -DDEBUG -DCPU_MK22FN512VLH12 -DFSL_RTOS_FREE_RTOS -DFRDM_K22F -DFREEDOM -I../CMSIS/Include -I../devices -I../drivers -I../freertos/Source/include -I../freertos/Source/portable/GCC/ARM_CM4F -I../freertos/Source -I../sources -I../utilities -std=gnu99 -mapcs -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


