################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../src/image_comparison.cpp \
../src/image_preprocessing.cpp \
../src/load_data.cpp \
../src/scamp5_main.cpp \
../src/sequence_recognition.cpp \
../src/visual_recognition.cpp 

OBJS += \
./src/image_comparison.o \
./src/image_preprocessing.o \
./src/load_data.o \
./src/scamp5_main.o \
./src/sequence_recognition.o \
./src/visual_recognition.o 

CPP_DEPS += \
./src/image_comparison.d \
./src/image_preprocessing.d \
./src/load_data.d \
./src/scamp5_main.d \
./src/sequence_recognition.d \
./src/visual_recognition.d 


# Each subdirectory must supply rules for building sources it contributes
src/%.o: ../src/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: MCU C++ Compiler'
	arm-none-eabi-c++ -std=gnu++11 -D__NEWLIB__ -DDEBUG -D__CODE_RED -DCORE_M0 -D__USE_LPCOPEN -DNO_BOARD_LIB -DCPP_USE_HEAP -D__LPC43XX__ -D__MULTICORE_M0APP -DCORE_M0APP -I"D:\ScampWorkspace4\lpc_chip_43xx_m0\inc" -I"D:\ScampWorkspace4\s5d_m0\inc" -I"D:\ScampWorkspace4\s5d_m4\inc" -Og -fno-common -g3 -Wall -c -fmessage-length=0 -fno-builtin -ffunction-sections -fdata-sections -fno-rtti -fno-exceptions -mcpu=cortex-m0 -mthumb -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.o)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


