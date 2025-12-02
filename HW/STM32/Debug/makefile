################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (13.3.rel1)
################################################################################

-include ../makefile.init

RM := rm -rf

# All of the sources participating in the build are defined here
-include sources.mk
-include Edge-AI/tflite-model/subdir.mk
-include Edge-AI/edge-impulse-sdk/tensorflow/lite/micro/memory_planner/subdir.mk
-include Edge-AI/edge-impulse-sdk/tensorflow/lite/micro/kernels/subdir.mk
-include Edge-AI/edge-impulse-sdk/tensorflow/lite/micro/subdir.mk
-include Edge-AI/edge-impulse-sdk/tensorflow/lite/kernels/internal/subdir.mk
-include Edge-AI/edge-impulse-sdk/tensorflow/lite/kernels/custom/subdir.mk
-include Edge-AI/edge-impulse-sdk/tensorflow/lite/kernels/subdir.mk
-include Edge-AI/edge-impulse-sdk/tensorflow/lite/core/api/subdir.mk
-include Edge-AI/edge-impulse-sdk/tensorflow/lite/c/subdir.mk
-include Edge-AI/edge-impulse-sdk/dsp/kissfft/subdir.mk
-include Edge-AI/edge-impulse-sdk/dsp/image/subdir.mk
-include Edge-AI/edge-impulse-sdk/dsp/dct/subdir.mk
-include Edge-AI/edge-impulse-sdk/dsp/subdir.mk
-include Edge-AI/edge-impulse-sdk/classifier/subdir.mk
-include Edge-AI/edge-impulse-sdk/CMSIS/NN/Source/SoftmaxFunctions/subdir.mk
-include Edge-AI/edge-impulse-sdk/CMSIS/NN/Source/SVDFunctions/subdir.mk
-include Edge-AI/edge-impulse-sdk/CMSIS/NN/Source/ReshapeFunctions/subdir.mk
-include Edge-AI/edge-impulse-sdk/CMSIS/NN/Source/PoolingFunctions/subdir.mk
-include Edge-AI/edge-impulse-sdk/CMSIS/NN/Source/NNSupportFunctions/subdir.mk
-include Edge-AI/edge-impulse-sdk/CMSIS/NN/Source/FullyConnectedFunctions/subdir.mk
-include Edge-AI/edge-impulse-sdk/CMSIS/NN/Source/ConvolutionFunctions/subdir.mk
-include Edge-AI/edge-impulse-sdk/CMSIS/NN/Source/ConcatenationFunctions/subdir.mk
-include Edge-AI/edge-impulse-sdk/CMSIS/NN/Source/BasicMathFunctions/subdir.mk
-include Edge-AI/edge-impulse-sdk/CMSIS/NN/Source/ActivationFunctions/subdir.mk
-include Edge-AI/edge-impulse-sdk/CMSIS/DSP/Source/TransformFunctions/subdir.mk
-include Edge-AI/edge-impulse-sdk/CMSIS/DSP/Source/SupportFunctions/subdir.mk
-include Edge-AI/edge-impulse-sdk/CMSIS/DSP/Source/StatisticsFunctions/subdir.mk
-include Edge-AI/edge-impulse-sdk/CMSIS/DSP/Source/SVMFunctions/subdir.mk
-include Edge-AI/edge-impulse-sdk/CMSIS/DSP/Source/QuaternionMathFunctions/subdir.mk
-include Edge-AI/edge-impulse-sdk/CMSIS/DSP/Source/MatrixFunctions/subdir.mk
-include Edge-AI/edge-impulse-sdk/CMSIS/DSP/Source/InterpolationFunctions/subdir.mk
-include Edge-AI/edge-impulse-sdk/CMSIS/DSP/Source/FilteringFunctions/subdir.mk
-include Edge-AI/edge-impulse-sdk/CMSIS/DSP/Source/FastMathFunctions/subdir.mk
-include Edge-AI/edge-impulse-sdk/CMSIS/DSP/Source/DistanceFunctions/subdir.mk
-include Edge-AI/edge-impulse-sdk/CMSIS/DSP/Source/ControllerFunctions/subdir.mk
-include Edge-AI/edge-impulse-sdk/CMSIS/DSP/Source/ComplexMathFunctions/subdir.mk
-include Edge-AI/edge-impulse-sdk/CMSIS/DSP/Source/CommonTables/subdir.mk
-include Edge-AI/edge-impulse-sdk/CMSIS/DSP/Source/BayesFunctions/subdir.mk
-include Edge-AI/edge-impulse-sdk/CMSIS/DSP/Source/BasicMathFunctions/subdir.mk
-include Drivers/STM32F4xx_HAL_Driver/subdir.mk
-include Core/Startup/subdir.mk
-include Core/Src/subdir.mk
-include objects.mk

ifneq ($(MAKECMDGOALS),clean)
ifneq ($(strip $(CC_DEPS)),)
-include $(CC_DEPS)
endif
ifneq ($(strip $(C++_DEPS)),)
-include $(C++_DEPS)
endif
ifneq ($(strip $(CCM_DEPS)),)
-include $(CCM_DEPS)
endif
ifneq ($(strip $(C_UPPER_DEPS)),)
-include $(C_UPPER_DEPS)
endif
ifneq ($(strip $(CXX_DEPS)),)
-include $(CXX_DEPS)
endif
ifneq ($(strip $(S_DEPS)),)
-include $(S_DEPS)
endif
ifneq ($(strip $(S_UPPER_DEPS)),)
-include $(S_UPPER_DEPS)
endif
ifneq ($(strip $(CXXM_DEPS)),)
-include $(CXXM_DEPS)
endif
ifneq ($(strip $(C++M_DEPS)),)
-include $(C++M_DEPS)
endif
ifneq ($(strip $(C_DEPS)),)
-include $(C_DEPS)
endif
ifneq ($(strip $(CPP_DEPS)),)
-include $(CPP_DEPS)
endif
endif

-include ../makefile.defs

OPTIONAL_TOOL_DEPS := \
$(wildcard ../makefile.defs) \
$(wildcard ../makefile.init) \
$(wildcard ../makefile.targets) \


BUILD_ARTIFACT_NAME := SoundTest
BUILD_ARTIFACT_EXTENSION := elf
BUILD_ARTIFACT_PREFIX :=
BUILD_ARTIFACT := $(BUILD_ARTIFACT_PREFIX)$(BUILD_ARTIFACT_NAME)$(if $(BUILD_ARTIFACT_EXTENSION),.$(BUILD_ARTIFACT_EXTENSION),)

# Add inputs and outputs from these tool invocations to the build variables 
EXECUTABLES += \
SoundTest.elf \

MAP_FILES += \
SoundTest.map \

SIZE_OUTPUT += \
default.size.stdout \

OBJDUMP_LIST += \
SoundTest.list \


# All Target
all: main-build

# Main-build Target
main-build: SoundTest.elf secondary-outputs

# Tool invocations
SoundTest.elf SoundTest.map: $(OBJS) $(USER_OBJS) C:\Users\taejeong\STM32CubeIDE\RealSad2DMapping\VibeClean\HW\STM32\STM32F446RETX_FLASH.ld makefile objects.list $(OPTIONAL_TOOL_DEPS)
	arm-none-eabi-g++ -o "SoundTest.elf" @"objects.list" $(USER_OBJS) $(LIBS) -mcpu=cortex-m4 -T"C:\Users\taejeong\STM32CubeIDE\RealSad2DMapping\VibeClean\HW\STM32\STM32F446RETX_FLASH.ld" -Wl,-Map="SoundTest.map" -Wl,--gc-sections -static -u _printf_float --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -Wl,--start-group -lc -lm -lstdc++ -lsupc++ -Wl,--end-group
	@echo 'Finished building target: $@'
	@echo ' '

default.size.stdout: $(EXECUTABLES) makefile objects.list $(OPTIONAL_TOOL_DEPS)
	arm-none-eabi-size  $(EXECUTABLES)
	@echo 'Finished building: $@'
	@echo ' '

SoundTest.list: $(EXECUTABLES) makefile objects.list $(OPTIONAL_TOOL_DEPS)
	arm-none-eabi-objdump -h -S $(EXECUTABLES) > "SoundTest.list"
	@echo 'Finished building: $@'
	@echo ' '

# Other Targets
clean:
	-$(RM) SoundTest.elf SoundTest.list SoundTest.map default.size.stdout
	-@echo ' '

secondary-outputs: $(SIZE_OUTPUT) $(OBJDUMP_LIST)

fail-specified-linker-script-missing:
	@echo 'Error: Cannot find the specified linker script. Check the linker settings in the build configuration.'
	@exit 2

warn-no-linker-script-specified:
	@echo 'Warning: No linker script specified. Check the linker settings in the build configuration.'

.PHONY: all clean dependents main-build fail-specified-linker-script-missing warn-no-linker-script-specified

-include ../makefile.targets
