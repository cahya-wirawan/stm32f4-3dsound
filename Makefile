# Sources

SRCS = main.c stm32f4xx_it.c system_stm32f4xx.c syscalls.c \
	Audio.c cwSoundFile.c cwMemory.c cwWave.c cwSound3D.c \
	cwFS.c Hrtf.c HrtfArray.c HrtfDb.c Audio.c DSP.c Sound.c cwMems.c

# Project name

PROJ_NAME=stm32f4-musicplayer
OUTPATH=build

###################################################

# Check for valid float argument
# NOTE that you have to run make clan after
# changing these as hardfloat and softfloat are not
# binary compatible
ifneq ($(FLOAT_TYPE), hard)
ifneq ($(FLOAT_TYPE), soft)
override FLOAT_TYPE = hard
#override FLOAT_TYPE = soft
endif
endif

###################################################

CC=arm-none-eabi-gcc
OBJCOPY=arm-none-eabi-objcopy
SIZE=arm-none-eabi-size

CFLAGS  = -std=gnu90 -Wall -Tstm32_flash.ld
CFLAGS += -mlittle-endian -mthumb -mthumb-interwork -mcpu=cortex-m4
#CFLAGS += -mlittle-endian -mthumb -mthumb-interwork -nostartfiles -mcpu=cortex-m4
CFLAGS += -DSTM32F40_41xxx -DSTM32F407VG -DUSE_STDPERIPH_DRIVER -DHSE_VALUE='((uint32_t)8000000)'
CFLAGS += -DARM_MATH_CM4 -D__FPU_PRESENT
#CFLAGS += -fstack-usage -fstack-check
#CFLAGS += -DSTM32F407VG -DUSE_STDPERIPH_DRIVER

ifeq ($(FLOAT_TYPE), hard)
CFLAGS += -fsingle-precision-constant -Wdouble-promotion
CFLAGS += -mfpu=fpv4-sp-d16 -mfloat-abi=hard
#CFLAGS += -mfpu=fpv4-sp-d16 -mfloat-abi=softfp
else
CFLAGS += -msoft-float
endif

###################################################

vpath %.c src
vpath %.a lib

ROOT=$(shell pwd)

CFLAGS += -Iinc -Ilib -Isrc
CFLAGS += -Ilib/driver/inc -Ilib/cmsis/device/inc -Ilib/cmsis/inc -Ilib/fatfs -Ilib/usb_msc_host/inc -Ilib/stm32f4-tm/inc

#SRCS += lib/startup_stm32f40_41xxx.s # add startup file to build
SRCS += lib/startup_stm32f4xx.s

OBJS = $(SRCS:.c=.o)

###################################################

.PHONY: lib proj

all: lib proj
	$(SIZE) $(OUTPATH)/$(PROJ_NAME).elf

lib:
	$(MAKE) -C lib FLOAT_TYPE=$(FLOAT_TYPE)

proj: 	$(OUTPATH)/$(PROJ_NAME).elf

$(OUTPATH)/$(PROJ_NAME).elf: $(SRCS)
	mkdir -p $(OUTPATH)
	$(CC) $(CFLAGS) $^ -o $@ -Llib -lfatfs -lstm32f4-tm -lusbmschost -lstm32-dsp -lstm32f4 -lm -Wl,--gc-sections
	#$(CC) $(CFLAGS) $^ -o $@ -Llib -lfatfs -lstm32f4-tm -lusbmschost -lstm32-dsp -lstm32f4 -lm
	$(OBJCOPY) -O ihex $(OUTPATH)/$(PROJ_NAME).elf $(OUTPATH)/$(PROJ_NAME).hex
	$(OBJCOPY) -O binary $(OUTPATH)/$(PROJ_NAME).elf $(OUTPATH)/$(PROJ_NAME).bin

clean:
	rm -f *.o
	rm -f $(OUTPATH)/$(PROJ_NAME).elf
	rm -f $(OUTPATH)/$(PROJ_NAME).hex
	rm -f $(OUTPATH)/$(PROJ_NAME).bin
	$(MAKE) clean -C lib # Remove this line if you don't want to clean the libs as well
	
