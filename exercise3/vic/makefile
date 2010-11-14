
ISPBIN = /usr/local/bin
GCCBIN = /usr/local/bin/
LPC21ISP_PORT = COM3

LPC21ISP_PORT = /dev/ttyUSB0


LDSCRIPT = lpc2148-flash
#LDSCRIPT = lpc2148-ram

SOURCES			= ../test-busywait.c
SOURCES			= ../test-switches.c
SOURCES			= ../test-systick.c
SOURCES			= ../test-char-lcd.c
SOURCES			= ../test-clocks.c
SOURCES			= ../test-gpio.c
SOURCES			= test-gpio.c
SOURCES			= test-clocks.c
SOURCES			= test-leds.c
SOURCES			= test-systick.c
SOURCES			= test-busywait.c
SOURCES			= test-buzzer.c
SOURCES			= test-usb-serial.c
SOURCES			= test-uart.c
SOURCES			= test-char-lcd.c
SOURCES			= fiqinvic.c

CPU             = arm7tdmi

NAME            = image
CFLAGS          = -mcpu=${CPU} -O2 -Wall -g -gdwarf-2 -std=gnu99 -I. -I.. -I../../generic -I../../stdc -I../../arm7 -I../../lpc2000
CFLAGS          = -mcpu=${CPU} -O2 -std=gnu99 -I. -I.. -I../../stdc -I../../generic -I../../arm7 -I../../lpc2000
CC              = $(GCCBIN)arm-elf-gcc

all: ${NAME}.hex

startup.o: startup.S
	$(CC) -c -I. -x assembler-with-cpp -mcpu=arm7tdmi -Wa,-adhlns=$(<:.S=.lst),--gdwarf-2 -Wa,--gdwarf-2 $< -o $@

${NAME}.elf: startup.o *.c  
	${CC} ${CFLAGS} -DSHOFFER_CTC -o $@ ${SOURCES} startup.o -lc -lm -lgcc -nostartfiles -T$(LDSCRIPT).ld

#msp430-objcopy -O ihex $^ $@
${NAME}.hex: ${NAME}.elf
	$(GCCBIN)arm-elf-objcopy --strip-debug -O ihex $< $@

${NAME}.lst: ${NAME}.elf
	$(GCCBIN)arm-elf-objdump -dSt $^ >$@

LPC21ISP_BAUD = 19200
LPC21ISP_XTAL = 12000
program: ${NAME}.hex
	$(ISPBIN)/lpc21isp -term -control $(LPC21ISP_DEBUG) ${NAME}.hex $(LPC21ISP_PORT) $(LPC21ISP_BAUD) $(LPC21ISP_XTAL)

connect: ${NAME}.hex
	$(ISPBIN)/lpc21isp -termonly -control $(LPC21ISP_DEBUG) ${NAME}.hex $(LPC21ISP_PORT) $(LPC21ISP_BAUD) $(LPC21ISP_XTAL)

clean:
	rm -f *.o *.elf *.hex *.lst 


