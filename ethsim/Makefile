TARGET = ethapp

OBJS = main.o usbnet.o sock_util.o app.o util.o
OBJS += drivers/enc28j60.o
OBJS += $(UIP_PATH)/uip.o $(UIP_PATH)/uip_arp.o

UIP_PATH = libs/uip-1.0/uip

LOG_LEVEL ?= INFO

CC = gcc
CFLAGS = -I. -Ilibs/uip-1.0 -g -Wall -Werror -std=c99 -D_POSIX_C_SOURCE=200809L -DLOG_LEVEL=LOG_LEVEL_$(LOG_LEVEL)

$(TARGET): $(OBJS)
	$(CC) $(OBJS) -o $@

%.o: %.c
	$(CC) $(CFLAGS) $< -c -o $@

clean:
	rm -f $(TARGET) $(OBJS)

