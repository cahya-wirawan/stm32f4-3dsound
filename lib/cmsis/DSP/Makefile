
CC=arm-none-eabi-gcc
AR=arm-none-eabi-ar

###################################################

# Check for valid float argument
ifneq ($(FLOAT_TYPE), hard)
ifneq ($(FLOAT_TYPE), soft)
#override FLOAT_TYPE = hard
override FLOAT_TYPE = soft
endif
endif

###################################################

vpath %.c src

CFLAGS  = -g -O2 -Wall
CFLAGS += -mlittle-endian -mthumb -mthumb-interwork -mcpu=cortex-m4
CFLAGS += -fdata-sections -ffunction-sections
CFLAGS += -DSTM32F40_41xxx -DUSE_STDPERIPH_DRIVER -DHSE_VALUE='((uint32_t)8000000)' -DARM_MATH_CM4 -D__FPU_PRESENT

ifeq ($(FLOAT_TYPE), hard)
CFLAGS += -fsingle-precision-constant -Wdouble-promotion
CFLAGS += -mfpu=fpv4-sp-d16 -mfloat-abi=hard
#CFLAGS += -mfpu=fpv4-sp-d16 -mfloat-abi=softfp
else
CFLAGS += -msoft-float
endif

CFLAGS += -ffreestanding -nostdlib
CFLAGS += -I. -I../../../src -I../../cmsis/device/inc -I../../cmsis/inc -I../../driver/inc -I../../usb_msc_host/inc -I../../stm32f4-tm/inc

SRCS =  arm_bitreversal.c arm_cfft_radix4_f32.c  arm_cfft_radix4_q15.c  arm_cmplx_mult_cmplx_q31.c  arm_common_tables.c  arm_fill_q15.c \
	arm_cfft_radix4_init_f32.c  arm_cfft_radix4_q31.c  arm_cmplx_mult_real_f32.c  arm_copy_f32.c  arm_scale_f32.c \
	arm_cfft_radix4_init_q15.c  arm_cmplx_mult_cmplx_f32.c  arm_cmplx_mult_real_q15.c  arm_copy_q15.c  arm_scale_q15.c \
	arm_cfft_radix4_init_q31.c  arm_cmplx_mult_cmplx_q15.c  arm_cmplx_mult_real_q31.c  arm_fill_f32.c


OBJS = $(SRCS:.c=.o)

.PHONY: libstm32-dsp.a

all: libstm32-dsp.a

%.o : %.c
	$(CC) $(CFLAGS) -c -o $@ $^

libstm32-dsp.a: $(OBJS)
	$(AR) -r ../../$@ $(OBJS)

clean:
	rm -f $(OBJS) ../../libstm32-dsp.a
