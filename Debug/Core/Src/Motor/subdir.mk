################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (13.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Core/Src/Motor/Motor.c 

OBJS += \
./Core/Src/Motor/Motor.o 

C_DEPS += \
./Core/Src/Motor/Motor.d 


# Each subdirectory must supply rules for building sources it contributes
Core/Src/Motor/%.o Core/Src/Motor/%.su Core/Src/Motor/%.cyclo: ../Core/Src/Motor/%.c Core/Src/Motor/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F446xx -c -I../Core/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -I../Middlewares/Third_Party/FreeRTOS/Source/include -I../Middlewares/Third_Party/FreeRTOS/Source/CMSIS_RTOS_V2 -I../Middlewares/Third_Party/FreeRTOS/Source/portable/GCC/ARM_CM4F -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../Drivers/CMSIS/Include -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-Core-2f-Src-2f-Motor

clean-Core-2f-Src-2f-Motor:
	-$(RM) ./Core/Src/Motor/Motor.cyclo ./Core/Src/Motor/Motor.d ./Core/Src/Motor/Motor.o ./Core/Src/Motor/Motor.su

.PHONY: clean-Core-2f-Src-2f-Motor

