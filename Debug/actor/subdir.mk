################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../actor/Actor.cpp \
../actor/ActorBlocking.cpp \
../actor/ActorNet.cpp \
../actor/ActorSingleThread.cpp \
../actor/Fusr.cpp 

OBJS += \
./actor/Actor.o \
./actor/ActorBlocking.o \
./actor/ActorNet.o \
./actor/ActorSingleThread.o \
./actor/Fusr.o 

CPP_DEPS += \
./actor/Actor.d \
./actor/ActorBlocking.d \
./actor/ActorNet.d \
./actor/ActorSingleThread.d \
./actor/Fusr.d 


# Each subdirectory must supply rules for building sources it contributes
actor/%.o: ../actor/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -std=c++1y -DLIBFSC -DLINUX -I"/root/c/project/dev/libmisc-c" -I"/root/c/project/dev/libsrv-comm-c" -O0 -g3 -Wall -c -fmessage-length=0 -fPIC -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


