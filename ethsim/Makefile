TARGET = ethapp

OBJS = main.o usbnet.o sock_util.o app.o util.o
OBJS += drivers/enc28j60.o
OBJS += $(UIP_SRC)/uip.o $(UIP_SRC)/uip_arp.o
OBJS += $(POLARSSL_SRC)/bignum.o $(POLARSSL_SRC)/rsa.o

UIP_ROOT = libs/uip-1.0
UIP_SRC = $(UIP_ROOT)/uip
UIP_INC = $(UIP_ROOT)

POLARSSL_ROOT = libs/polarssl-0.14.0
POLARSSL_SRC = $(POLARSSL_ROOT)/library
POLARSSL_INC = $(POLARSSL_ROOT)/include

LOG_LEVEL ?= INFO

CC = gcc
#CFLAGS = -I. -I$(UIP_INC) -I$(POLARSSL_INC) -g -Wall -Werror -std=c99 -D_POSIX_C_SOURCE=200809L -DLOG_LEVEL=LOG_LEVEL_$(LOG_LEVEL)
CFLAGS = -I. -I$(UIP_INC) -I$(POLARSSL_INC) -g -Wall -Werror -DLOG_LEVEL=LOG_LEVEL_$(LOG_LEVEL)

$(TARGET): $(OBJS)
	$(CC) $(OBJS) -o $@

%.o: %.c
	$(CC) $(CFLAGS) $< -c -o $@

clean:
	rm -f $(TARGET) $(OBJS)

