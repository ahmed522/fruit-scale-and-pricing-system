<<<<<<< HEAD
################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
=======
################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
>>>>>>> edc6387c1d585a5baaf460e549f88eece77f0548
C_SRCS += \
../src/app_data.c \
../src/eeprom.c \
../src/hx711.c \
../src/keypad.c \
../src/lcd.c \
<<<<<<< HEAD
../src/main.c 

=======
../src/main.c 

>>>>>>> edc6387c1d585a5baaf460e549f88eece77f0548
OBJS += \
./src/app_data.o \
./src/eeprom.o \
./src/hx711.o \
./src/keypad.o \
./src/lcd.o \
<<<<<<< HEAD
./src/main.o 

=======
./src/main.o 

>>>>>>> edc6387c1d585a5baaf460e549f88eece77f0548
C_DEPS += \
./src/app_data.d \
./src/eeprom.d \
./src/hx711.d \
./src/keypad.d \
./src/lcd.d \
<<<<<<< HEAD
./src/main.d 


# Each subdirectory must supply rules for building sources it contributes
src/%.o: ../src/%.c src/subdir.mk
	@echo 'Building file: $<'
	@echo 'Invoking: AVR Compiler'
	avr-gcc -Wall -g2 -gstabs -Os -fpack-struct -fshort-enums -ffunction-sections -fdata-sections -Os -ffunction-sections -fdata-sections -Wl,--gc-sections -std=gnu99 -funsigned-char -funsigned-bitfields -Wl,--relax -Wl,--gc-sections -mmcu=atmega328p -DF_CPU=1000000UL -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


=======
./src/main.d 


# Each subdirectory must supply rules for building sources it contributes
src/%.o: ../src/%.c src/subdir.mk
	@echo 'Building file: $<'
	@echo 'Invoking: AVR Compiler'
	avr-gcc -Wall -g2 -gstabs -Os -fpack-struct -fshort-enums -ffunction-sections -fdata-sections -Os -ffunction-sections -fdata-sections -Wl,--gc-sections -std=gnu99 -funsigned-char -funsigned-bitfields -Wl,--relax -Wl,--gc-sections -mmcu=atmega328p -DF_CPU=1000000UL -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


>>>>>>> edc6387c1d585a5baaf460e549f88eece77f0548
