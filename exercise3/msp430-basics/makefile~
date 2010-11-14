SOURCES = led-isr.c
SOURCES = do-nothing.c

CPU             = msp430x1611

NAME            = image
CFLAGS          = -mmcu=${CPU} -O2 -I. -I../../generic -I../../msp430
CC              = msp430-gcc

all: ${NAME}.elf

${NAME}.elf: *.c
	${CC} ${CFLAGS} -o $@ ${SOURCES} -lm

${NAME}.a43: ${NAME}.elf
	msp430-objcopy -O ihex $^ $@

${NAME}.lst: ${NAME}.elf
	msp430-objdump -dSt $^ >$@

download-jtag: all
	msp430-jtag -e ${NAME}.elf

#c:\programs\mspgcc\bin\msp430-bsl --telosb --f1x -r -e ${NAME}.elfprogram: ${NAME}.elf
#c:\Programs\msp430-bsl-windows.exe --comport=com5 --telosb -r -e ${NAME}.elf
# c:\Software\msp430-bsl\msp430-bsl.exe --comport=com5 --telosb -r -e ${NAME}.elf
  #	msp430-bsl -e ${NAME}.elf
program:
	msp430-bsl --comport=/dev/ttyUSB0 --telosb -r -e ${NAME}.elf

clean:
	rm -f project.h *.o *.elf *.a43 *.lst

