################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Sources/main.c 

OBJS += \
./Sources/main.o 

C_DEPS += \
./Sources/main.d 


# Each subdirectory must supply rules for building sources it contributes
Sources/%.o: ../Sources/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross ARM C Compiler'
	arm-none-eabi-gcc -mcpu=cortex-m4 -mthumb -mfloat-abi=hard -mfpu=fpv4-sp-d16 -O0 -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections  -g3 -I"C:/Users/PMcL/Documents/Subjects/48434 Embedded Software/4 Labs/Lab 4/Template/Lab4/Static_Code/PDD" -I"C:/Users/PMcL/Documents/Subjects/48434 Embedded Software/4 Labs/Lab 4/Template/Lab4/Static_Code/IO_Map" -I"C:/Users/PMcL/Documents/Subjects/48434 Embedded Software/4 Labs/Lab 4/Template/Lab4/Sources" -I"C:/Users/PMcL/Documents/Subjects/48434 Embedded Software/4 Labs/Lab 4/Template/Lab4/Generated_Code" -std=c99 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


