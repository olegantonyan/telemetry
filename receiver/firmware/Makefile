TOOLCHAIN_PATH = /opt/gcc-arm-none-eabi-6-2017-q1-update
LDSCRIPT = STM32F103R8Tx_FLASH.ld
CHIP = STM32F103xB
ARTEFACT = telemetry_receiver

SOURCES += $(shell find Drivers/STM32F1xx_HAL_Driver/Src/ -type f -iname '*.c')
SOURCES += $(shell find Drivers/CMSIS/Device/ST/STM32F1xx/Source/Templates/gcc/ -type f -iname '*.c')
SOURCES += $(shell find Middlewares/ -type f -iname '*.c')
SOURCES += $(shell find Src/ -type f -iname '*.c')
SOURCES += $(shell find startup/ -type f -iname '*.s')

INCLUDES += -IDrivers/CMSIS/Include
INCLUDES += -IDrivers/CMSIS/Device/ST/STM32F1xx/Include
INCLUDES += -IDrivers/STM32F1xx_HAL_Driver/Inc
INCLUDES += -IMiddlewares/ST/STM32_USB_Device_Library/Core/Inc/
INCLUDES += -IMiddlewares/ST/STM32_USB_Device_Library/Class/HID/Inc/
INCLUDES += -IMiddlewares/Third_Party/FreeRTOS/Source/CMSIS_RTOS/
INCLUDES += -IMiddlewares/Third_Party/FreeRTOS/Source/include/
INCLUDES += -IMiddlewares/Third_Party/FreeRTOS/Source/portable/GCC/ARM_CM3/
INCLUDES += -IInc
INCLUDES += -ISrc/app

DEFINES += -D$(CHIP)
DEFINES += -DUSE_HAL_DRIVER
BUILDDIR = build
OBJECTS = $(addprefix $(BUILDDIR)/, $(addsuffix .o, $(basename $(SOURCES))))

CC = $(TOOLCHAIN_PATH)/bin/arm-none-eabi-gcc
LD =  $(TOOLCHAIN_PATH)/bin/arm-none-eabi-gcc
AR =  $(TOOLCHAIN_PATH)/bin/arm-none-eabi-ar
OBJCOPY =  $(TOOLCHAIN_PATH)/bin/arm-none-eabi-objcopy
SIZE = $(TOOLCHAIN_PATH)/bin/arm-none-eabi-size

ELF = $(BUILDDIR)/$(ARTEFACT).elf
HEX = $(BUILDDIR)/$(ARTEFACT).hex
BIN = $(BUILDDIR)/$(ARTEFACT).bin

CFLAGS  = -O1 -g -Wall -mcpu=cortex-m3 -mthumb -mfloat-abi=soft $(INCLUDES) $(DEFINES)
LDFLAGS += -T$(LDSCRIPT) -mthumb -mcpu=cortex-m3 -mfloat-abi=soft -specs=nosys.specs -specs=nano.specs

all: $(BIN) $(HEX) print_size

flash: all
	st-flash --reset write $(BIN) 0x8000000

clean:
	rm -rf build/*

$(ELF): $(OBJECTS)
	$(LD) $(LDFLAGS) -o $@ $(OBJECTS) $(LDLIBS)

$(BUILDDIR)/%.o: %.c
	mkdir -p $(dir $@)
	$(CC) -c $(CFLAGS) $< -o $@

$(BUILDDIR)/%.o: %.s
	mkdir -p $(dir $@)
	$(CC) -c $(CFLAGS) $< -o $@

$(HEX): $(ELF)
	$(OBJCOPY) -O ihex $< $@

$(BIN): $(ELF)
	$(OBJCOPY) -O binary $< $@

print_size: $(ELF)
	$(SIZE) -t $(ELF)
