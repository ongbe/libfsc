################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../stmp/StmpCb.cpp \
../stmp/StmpFusr.cpp \
../stmp/StmpN2H.cpp \
../stmp/StmpNet.cpp \
../stmp/StmpSsc.cpp \
../stmp/StmpTrans.cpp \
../stmp/StmpTransInitiative.cpp \
../stmp/StmpTransPassive.cpp \
../stmp/StmpTransSwitchInitiative.cpp \
../stmp/StmpTransSwitchPassive.cpp 

OBJS += \
./stmp/StmpCb.o \
./stmp/StmpFusr.o \
./stmp/StmpN2H.o \
./stmp/StmpNet.o \
./stmp/StmpSsc.o \
./stmp/StmpTrans.o \
./stmp/StmpTransInitiative.o \
./stmp/StmpTransPassive.o \
./stmp/StmpTransSwitchInitiative.o \
./stmp/StmpTransSwitchPassive.o 

CPP_DEPS += \
./stmp/StmpCb.d \
./stmp/StmpFusr.d \
./stmp/StmpN2H.d \
./stmp/StmpNet.d \
./stmp/StmpSsc.d \
./stmp/StmpTrans.d \
./stmp/StmpTransInitiative.d \
./stmp/StmpTransPassive.d \
./stmp/StmpTransSwitchInitiative.d \
./stmp/StmpTransSwitchPassive.d 


# Each subdirectory must supply rules for building sources it contributes
stmp/%.o: ../stmp/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -std=c++1y -DLIBFSC -DLINUX -I"/root/c/project/dev/libmisc-c" -I"/root/c/project/dev/libsrv-comm-c" -O0 -g3 -Wall -c -fmessage-length=0 -fPIC -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


