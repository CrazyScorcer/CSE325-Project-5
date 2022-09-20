################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../source/ColorSenor.c \
../source/LineSensor.c \
../source/MotorControls.c \
../source/Project\ 5\ Navigation.c \
../source/ServoControl.c \
../source/SpeedControl.c \
../source/Utilities.c \
../source/mtb.c \
../source/semihost_hardfault.c 

OBJS += \
./source/ColorSenor.o \
./source/LineSensor.o \
./source/MotorControls.o \
./source/Project\ 5\ Navigation.o \
./source/ServoControl.o \
./source/SpeedControl.o \
./source/Utilities.o \
./source/mtb.o \
./source/semihost_hardfault.o 

C_DEPS += \
./source/ColorSenor.d \
./source/LineSensor.d \
./source/MotorControls.d \
./source/Project\ 5\ Navigation.d \
./source/ServoControl.d \
./source/SpeedControl.d \
./source/Utilities.d \
./source/mtb.d \
./source/semihost_hardfault.d 


# Each subdirectory must supply rules for building sources it contributes
source/%.o: ../source/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: MCU C Compiler'
	arm-none-eabi-gcc -D__REDLIB__ -DCPU_MKL46Z256VLL4 -DCPU_MKL46Z256VLL4_cm0plus -DSDK_OS_BAREMETAL -DFSL_RTOS_BM -DSDK_DEBUGCONSOLE=0 -DCR_INTEGER_PRINTF -DPRINTF_FLOAT_ENABLE=0 -D__MCUXPRESSO -D__USE_CMSIS -DDEBUG -I../board -I../source -I../ -I../drivers -I../CMSIS -I../utilities -I../startup -O0 -fno-common -g3 -Wall -c -ffunction-sections -fdata-sections -ffreestanding -fno-builtin -mcpu=cortex-m0plus -mthumb -D__REDLIB__ -fstack-usage -specs=redlib.specs -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.o)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

source/Project\ 5\ Navigation.o: ../source/Project\ 5\ Navigation.c
	@echo 'Building file: $<'
	@echo 'Invoking: MCU C Compiler'
	arm-none-eabi-gcc -D__REDLIB__ -DCPU_MKL46Z256VLL4 -DCPU_MKL46Z256VLL4_cm0plus -DSDK_OS_BAREMETAL -DFSL_RTOS_BM -DSDK_DEBUGCONSOLE=0 -DCR_INTEGER_PRINTF -DPRINTF_FLOAT_ENABLE=0 -D__MCUXPRESSO -D__USE_CMSIS -DDEBUG -I../board -I../source -I../ -I../drivers -I../CMSIS -I../utilities -I../startup -O0 -fno-common -g3 -Wall -c -ffunction-sections -fdata-sections -ffreestanding -fno-builtin -mcpu=cortex-m0plus -mthumb -D__REDLIB__ -fstack-usage -specs=redlib.specs -MMD -MP -MF"source/Project 5 Navigation.d" -MT"source/Project\ 5\ Navigation.d" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


