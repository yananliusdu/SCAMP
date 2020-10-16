################################################################################
# Automatically-generated file. Do not edit!
################################################################################

-include ../makefile.init

RM := rm -rf

# All of the sources participating in the build are defined here
-include sources.mk
-include src/subdir.mk
-include subdir.mk
-include objects.mk

ifneq ($(MAKECMDGOALS),clean)
ifneq ($(strip $(CC_DEPS)),)
-include $(CC_DEPS)
endif
ifneq ($(strip $(C++_DEPS)),)
-include $(C++_DEPS)
endif
ifneq ($(strip $(C_UPPER_DEPS)),)
-include $(C_UPPER_DEPS)
endif
ifneq ($(strip $(CXX_DEPS)),)
-include $(CXX_DEPS)
endif
ifneq ($(strip $(CPP_DEPS)),)
-include $(CPP_DEPS)
endif
ifneq ($(strip $(C_DEPS)),)
-include $(C_DEPS)
endif
endif

-include ../makefile.defs

# Add inputs and outputs from these tool invocations to the build variables 

# All Target
all: Hector_visual_route_recgnition.axf

# Tool invocations
Hector_visual_route_recgnition.axf: $(OBJS) $(USER_OBJS)
	@echo 'Building target: $@'
	@echo 'Invoking: MCU C++ Linker'
	arm-none-eabi-c++ -nostdlib -L"D:\ScampWorkspace4\s5d_m0\Release" -L"D:\ScampWorkspace4\lpc_chip_43xx_m0\Release" -Xlinker -Map="Hector_visual_route_recgnition.map" -Xlinker --gc-sections -Xlinker -print-memory-usage -mcpu=cortex-m0 -mthumb -T "example_5_motion_detection_Release.ld" -o "Hector_visual_route_recgnition.axf" $(OBJS) $(USER_OBJS) $(LIBS) && arm-none-eabi-objcopy --target elf32-littlearm --verbose --strip-all --remove-section=.ARM.attributes --redefine-sym __vectors_start__=__vectors_start___core_m0app --keep-symbol __vectors_start___core_m0app --redefine-sym _data=__start_data_core_m0app --keep-symbol __start_data_core_m0app --redefine-sym _edata=__end_data_core_m0app --keep-symbol __end_data_core_m0app --remove-section=".bss*" --remove-section=".noinit*" --rename-section .ARM.exidx=".core_m0app.ARM.exidx" --rename-section .ARM.extab=".core_m0app.ARM.extab" --rename-section .text=".core_m0app" --rename-section .data=".core_m0app.data" "Hector_visual_route_recgnition.axf" "Hector_visual_route_recgnition.axf.o"
	@echo 'Finished building target: $@'
	@echo ' '
	$(MAKE) --no-print-directory post-build

# Other Targets
clean:
	-$(RM) $(CC_DEPS)$(C++_DEPS)$(EXECUTABLES)$(C_UPPER_DEPS)$(CXX_DEPS)$(OBJS)$(CPP_DEPS)$(C_DEPS) Hector_visual_route_recgnition.axf
	-@echo ' '

post-build:
	-@echo 'Performing post-build steps'
	-arm-none-eabi-size "Hector_visual_route_recgnition.axf" ; arm-none-eabi-objcopy -v -O binary "Hector_visual_route_recgnition.axf" "Hector_visual_route_recgnition.bin" ; checksum -p LPC4357_M0 -d "Hector_visual_route_recgnition.bin"
	-@echo ' '

.PHONY: all clean dependents
.SECONDARY: post-build

-include ../makefile.targets
