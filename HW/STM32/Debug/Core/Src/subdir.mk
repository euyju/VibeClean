################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (13.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../Core/Src/edge_ai_wrapper.cpp \
../Core/Src/ei_classifier_porting.cpp 

C_SRCS += \
../Core/Src/ESP8266_HAL.c \
../Core/Src/UartRingbuffer_multi.c \
../Core/Src/main.c \
../Core/Src/mpu6050.c \
../Core/Src/stm32f4xx_hal_msp.c \
../Core/Src/stm32f4xx_it.c \
../Core/Src/syscalls.c \
../Core/Src/sysmem.c \
../Core/Src/system_stm32f4xx.c 

C_DEPS += \
./Core/Src/ESP8266_HAL.d \
./Core/Src/UartRingbuffer_multi.d \
./Core/Src/main.d \
./Core/Src/mpu6050.d \
./Core/Src/stm32f4xx_hal_msp.d \
./Core/Src/stm32f4xx_it.d \
./Core/Src/syscalls.d \
./Core/Src/sysmem.d \
./Core/Src/system_stm32f4xx.d 

OBJS += \
./Core/Src/ESP8266_HAL.o \
./Core/Src/UartRingbuffer_multi.o \
./Core/Src/edge_ai_wrapper.o \
./Core/Src/ei_classifier_porting.o \
./Core/Src/main.o \
./Core/Src/mpu6050.o \
./Core/Src/stm32f4xx_hal_msp.o \
./Core/Src/stm32f4xx_it.o \
./Core/Src/syscalls.o \
./Core/Src/sysmem.o \
./Core/Src/system_stm32f4xx.o 

CPP_DEPS += \
./Core/Src/edge_ai_wrapper.d \
./Core/Src/ei_classifier_porting.d 


# Each subdirectory must supply rules for building sources it contributes
Core/Src/%.o Core/Src/%.su Core/Src/%.cyclo: ../Core/Src/%.c Core/Src/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DEI_CLASSIFIER_TFLITE_ENABLE_CMSIS_NN=1 -DEI_PORTING_STM32_CUBEAI=1 -DARM_MATH_CM4 -D__FPU_PRESENT=1 -DTF_LITE_STATIC_MEMORY=1 -DUSE_HAL_DRIVER -DSTM32F446xx -c -I../Core/Inc -I"C:/Users/taejeong/STM32CubeIDE/iot/NewIot1123/VibeClean/HW/STM32/Edge-AI" -I"C:/Users/taejeong/STM32CubeIDE/iot/NewIot1123/VibeClean/HW/STM32/Edge-AI/edge-impulse-sdk" -I"C:/Users/taejeong/STM32CubeIDE/iot/NewIot1123/VibeClean/HW/STM32/Edge-AI/model-parameters" -I"C:/Users/taejeong/STM32CubeIDE/iot/NewIot1123/VibeClean/HW/STM32/Edge-AI/tflite-model" -I"C:/Users/taejeong/STM32CubeIDE/iot/NewIot1123/VibeClean/HW/STM32/Edge-AI/edge-impulse-sdk/porting" -I"C:/Users/taejeong/STM32CubeIDE/iot/NewIot1123/VibeClean/HW/STM32/Edge-AI/edge-impulse-sdk/classifier" -I"C:/Users/taejeong/STM32CubeIDE/iot/NewIot1123/VibeClean/HW/STM32/Edge-AI/edge-impulse-sdk/CMSIS/Core/Include" -I"C:/Users/taejeong/STM32CubeIDE/iot/NewIot1123/VibeClean/HW/STM32/Edge-AI/edge-impulse-sdk/CMSIS/DSP/Include" -I"C:/Users/taejeong/STM32CubeIDE/iot/NewIot1123/VibeClean/HW/STM32/Edge-AI/edge-impulse-sdk/dsp" -IC:/Users/taejeong/STM32Cube/Repository/STM32Cube_FW_F4_V1.28.3/Drivers/STM32F4xx_HAL_Driver/Inc -IC:/Users/taejeong/STM32Cube/Repository/STM32Cube_FW_F4_V1.28.3/Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -IC:/Users/taejeong/STM32Cube/Repository/STM32Cube_FW_F4_V1.28.3/Drivers/CMSIS/Device/ST/STM32F4xx/Include -IC:/Users/taejeong/STM32Cube/Repository/STM32Cube_FW_F4_V1.28.3/Drivers/CMSIS/Include -IC:/Users/82109/STM32Cube/Repository/STM32Cube_FW_F4_V1.28.3/Drivers/STM32F4xx_HAL_Driver/Inc -IC:/Users/82109/STM32Cube/Repository/STM32Cube_FW_F4_V1.28.3/Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -IC:/Users/82109/STM32Cube/Repository/STM32Cube_FW_F4_V1.28.3/Drivers/CMSIS/Device/ST/STM32F4xx/Include -IC:/Users/82109/STM32Cube/Repository/STM32Cube_FW_F4_V1.28.3/Drivers/CMSIS/Include -IC:/Users/goldl/STM32Cube/Repository/STM32Cube_FW_F4_V1.28.3/Drivers/STM32F4xx_HAL_Driver/Inc -IC:/Users/goldl/STM32Cube/Repository/STM32Cube_FW_F4_V1.28.3/Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -IC:/Users/goldl/STM32Cube/Repository/STM32Cube_FW_F4_V1.28.3/Drivers/CMSIS/Device/ST/STM32F4xx/Include -IC:/Users/goldl/STM32Cube/Repository/STM32Cube_FW_F4_V1.28.3/Drivers/CMSIS/Include -O2 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"
Core/Src/%.o Core/Src/%.su Core/Src/%.cyclo: ../Core/Src/%.cpp Core/Src/subdir.mk
	arm-none-eabi-g++ "$<" -mcpu=cortex-m4 -std=gnu++14 -g3 -DDEBUG -DEI_CLASSIFIER_TFLITE_ENABLE_CMSIS_NN=1 -DEI_PORTING_STM32_CUBEAI=1 -DARM_MATH_CM4 -D__FPU_PRESENT=1 -DTF_LITE_STATIC_MEMORY=1 -DUSE_HAL_DRIVER -DSTM32F446xx -c -I"C:/Users/taejeong/STM32CubeIDE/iot/NewIot1123/VibeClean/HW/STM32/Edge-AI" -I"C:/Users/taejeong/STM32CubeIDE/iot/NewIot1123/VibeClean/HW/STM32/Edge-AI/edge-impulse-sdk" -I"C:/Users/taejeong/STM32CubeIDE/iot/NewIot1123/VibeClean/HW/STM32/Edge-AI/edge-impulse-sdk/CMSIS/DSP/Include" -I"C:/Users/taejeong/STM32CubeIDE/iot/NewIot1123/VibeClean/HW/STM32/Edge-AI/edge-impulse-sdk/CMSIS/Core/Include" -I"C:/Users/taejeong/STM32CubeIDE/iot/NewIot1123/VibeClean/HW/STM32/Edge-AI/edge-impulse-sdk/classifier" -I"C:/Users/taejeong/STM32CubeIDE/iot/NewIot1123/VibeClean/HW/STM32/Edge-AI/edge-impulse-sdk/dsp" -I"C:/Users/taejeong/STM32CubeIDE/iot/NewIot1123/VibeClean/HW/STM32/Edge-AI/edge-impulse-sdk/porting" -I"C:/Users/taejeong/STM32CubeIDE/iot/NewIot1123/VibeClean/HW/STM32/Edge-AI/tflite-model" -I"C:/Users/taejeong/STM32CubeIDE/iot/NewIot1123/VibeClean/HW/STM32/Edge-AI/model-parameters" -I../Core/Inc -IC:/Users/taejeong/STM32Cube/Repository/STM32Cube_FW_F4_V1.28.3/Drivers/STM32F4xx_HAL_Driver/Inc -IC:/Users/taejeong/STM32Cube/Repository/STM32Cube_FW_F4_V1.28.3/Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -IC:/Users/taejeong/STM32Cube/Repository/STM32Cube_FW_F4_V1.28.3/Drivers/CMSIS/Device/ST/STM32F4xx/Include -IC:/Users/taejeong/STM32Cube/Repository/STM32Cube_FW_F4_V1.28.3/Drivers/CMSIS/Include -IC:/Users/82109/STM32Cube/Repository/STM32Cube_FW_F4_V1.28.3/Drivers/STM32F4xx_HAL_Driver/Inc -IC:/Users/82109/STM32Cube/Repository/STM32Cube_FW_F4_V1.28.3/Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -IC:/Users/82109/STM32Cube/Repository/STM32Cube_FW_F4_V1.28.3/Drivers/CMSIS/Device/ST/STM32F4xx/Include -IC:/Users/82109/STM32Cube/Repository/STM32Cube_FW_F4_V1.28.3/Drivers/CMSIS/Include -IC:/Users/goldl/STM32Cube/Repository/STM32Cube_FW_F4_V1.28.3/Drivers/STM32F4xx_HAL_Driver/Inc -IC:/Users/goldl/STM32Cube/Repository/STM32Cube_FW_F4_V1.28.3/Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -IC:/Users/goldl/STM32Cube/Repository/STM32Cube_FW_F4_V1.28.3/Drivers/CMSIS/Device/ST/STM32F4xx/Include -IC:/Users/goldl/STM32Cube/Repository/STM32Cube_FW_F4_V1.28.3/Drivers/CMSIS/Include -O0 -ffunction-sections -fdata-sections -fno-exceptions -fno-rtti -fno-use-cxa-atexit -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-Core-2f-Src

clean-Core-2f-Src:
	-$(RM) ./Core/Src/ESP8266_HAL.cyclo ./Core/Src/ESP8266_HAL.d ./Core/Src/ESP8266_HAL.o ./Core/Src/ESP8266_HAL.su ./Core/Src/UartRingbuffer_multi.cyclo ./Core/Src/UartRingbuffer_multi.d ./Core/Src/UartRingbuffer_multi.o ./Core/Src/UartRingbuffer_multi.su ./Core/Src/edge_ai_wrapper.cyclo ./Core/Src/edge_ai_wrapper.d ./Core/Src/edge_ai_wrapper.o ./Core/Src/edge_ai_wrapper.su ./Core/Src/ei_classifier_porting.cyclo ./Core/Src/ei_classifier_porting.d ./Core/Src/ei_classifier_porting.o ./Core/Src/ei_classifier_porting.su ./Core/Src/main.cyclo ./Core/Src/main.d ./Core/Src/main.o ./Core/Src/main.su ./Core/Src/mpu6050.cyclo ./Core/Src/mpu6050.d ./Core/Src/mpu6050.o ./Core/Src/mpu6050.su ./Core/Src/stm32f4xx_hal_msp.cyclo ./Core/Src/stm32f4xx_hal_msp.d ./Core/Src/stm32f4xx_hal_msp.o ./Core/Src/stm32f4xx_hal_msp.su ./Core/Src/stm32f4xx_it.cyclo ./Core/Src/stm32f4xx_it.d ./Core/Src/stm32f4xx_it.o ./Core/Src/stm32f4xx_it.su ./Core/Src/syscalls.cyclo ./Core/Src/syscalls.d ./Core/Src/syscalls.o ./Core/Src/syscalls.su ./Core/Src/sysmem.cyclo ./Core/Src/sysmem.d ./Core/Src/sysmem.o ./Core/Src/sysmem.su ./Core/Src/system_stm32f4xx.cyclo ./Core/Src/system_stm32f4xx.d ./Core/Src/system_stm32f4xx.o ./Core/Src/system_stm32f4xx.su

.PHONY: clean-Core-2f-Src

