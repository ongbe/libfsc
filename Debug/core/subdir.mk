################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../core/Cfg.cpp \
../core/Fsc.cpp \
../core/FscLog.cpp \
../core/FscMisc.cpp \
../core/FscStat.cpp \
../core/Fworker.cpp 

OBJS += \
./core/Cfg.o \
./core/Fsc.o \
./core/FscLog.o \
./core/FscMisc.o \
./core/FscStat.o \
./core/Fworker.o 

CPP_DEPS += \
./core/Cfg.d \
./core/Fsc.d \
./core/FscLog.d \
./core/FscMisc.d \
./core/FscStat.d \
./core/Fworker.d 


# Each subdirectory must supply rules for building sources it contributes
core/%.o: ../core/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -std=c++1y -DLIBFSC -DLINUX -I"/root/c/project/ethex/libmisc-c" -I"/root/c/project/dev/libsrv-comm-c" -O0 -g3 -Wall -c -fmessage-length=0 -fPIC -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


