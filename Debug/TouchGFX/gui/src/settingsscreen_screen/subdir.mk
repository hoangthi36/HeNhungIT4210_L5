################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (13.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../TouchGFX/gui/src/settingsscreen_screen/SettingsScreenPresenter.cpp \
../TouchGFX/gui/src/settingsscreen_screen/SettingsScreenView.cpp 

OBJS += \
./TouchGFX/gui/src/settingsscreen_screen/SettingsScreenPresenter.o \
./TouchGFX/gui/src/settingsscreen_screen/SettingsScreenView.o 

CPP_DEPS += \
./TouchGFX/gui/src/settingsscreen_screen/SettingsScreenPresenter.d \
./TouchGFX/gui/src/settingsscreen_screen/SettingsScreenView.d 


# Each subdirectory must supply rules for building sources it contributes
TouchGFX/gui/src/settingsscreen_screen/%.o TouchGFX/gui/src/settingsscreen_screen/%.su TouchGFX/gui/src/settingsscreen_screen/%.cyclo: ../TouchGFX/gui/src/settingsscreen_screen/%.cpp TouchGFX/gui/src/settingsscreen_screen/subdir.mk
	arm-none-eabi-g++ "$<" -mcpu=cortex-m4 -std=gnu++14 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F429xx -c -I../TouchGFX/App -I../TouchGFX/target/generated -I../TouchGFX/target -I../Core/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../Drivers/CMSIS/Include -I../Middlewares/ST/touchgfx/framework/include -I../TouchGFX/generated/fonts/include -I../TouchGFX/generated/gui_generated/include -I../TouchGFX/generated/images/include -I../TouchGFX/generated/texts/include -I../TouchGFX/generated/videos/include -I../TouchGFX/gui/include -O0 -ffunction-sections -fdata-sections -fno-exceptions -fno-rtti -fno-use-cxa-atexit -Wall -femit-class-debug-always -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-TouchGFX-2f-gui-2f-src-2f-settingsscreen_screen

clean-TouchGFX-2f-gui-2f-src-2f-settingsscreen_screen:
	-$(RM) ./TouchGFX/gui/src/settingsscreen_screen/SettingsScreenPresenter.cyclo ./TouchGFX/gui/src/settingsscreen_screen/SettingsScreenPresenter.d ./TouchGFX/gui/src/settingsscreen_screen/SettingsScreenPresenter.o ./TouchGFX/gui/src/settingsscreen_screen/SettingsScreenPresenter.su ./TouchGFX/gui/src/settingsscreen_screen/SettingsScreenView.cyclo ./TouchGFX/gui/src/settingsscreen_screen/SettingsScreenView.d ./TouchGFX/gui/src/settingsscreen_screen/SettingsScreenView.o ./TouchGFX/gui/src/settingsscreen_screen/SettingsScreenView.su

.PHONY: clean-TouchGFX-2f-gui-2f-src-2f-settingsscreen_screen

