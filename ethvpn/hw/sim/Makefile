TARGET = ethvpn

UIP_ROOT = ../../external/uip-1.0
UIP_SRC = $(UIP_ROOT)/uip
UIP_INC = $(UIP_ROOT)

POLARSSL_ROOT = ../../external/polarssl-0.14.0
POLARSSL_INC = $(POLARSSL_ROOT)/include

POLARSSL_UTIL_ROOT = ../../util/polarssl
POLARSSL_UTIL_INC = $(POLARSSL_UTIL_ROOT)

COMMON_DIR = ../common

OBJS += $(COMMON_DIR)/app.o $(COMMON_DIR)/pkt_channel.o $(COMMON_DIR)/secure_channel.o
OBJS += main.o usbnet.o udp_channel.o util.o
OBJS += drivers/enc28j60.o
OBJS += $(UIP_SRC)/uip.o $(UIP_SRC)/uip_arp.o $(UIP_SRC)/timer.o
OBJS += $(UIP_ROOT)/apps/dhcpc/dhcpc.o $(UIP_ROOT)/apps/resolv/resolv.o

LOG_LEVEL ?= INFO

CC = gcc

CFLAGS += -I. -I$(COMMON_DIR) -I$(UIP_INC) -I$(UIP_INC)/uip -I$(POLARSSL_INC) -I$(POLARSSL_UTIL_INC)
CFLAGS += -g -Wall -Werror -DLOG_LEVEL=LOG_LEVEL_$(LOG_LEVEL)

LDFLAGS = -L../../util/polarssl -lpolarssl


$(TARGET): $(OBJS)
	$(CC) $(OBJS) $(LDFLAGS) -o $@

%.o: %.c
	$(CC) $(CFLAGS) $< -c -o $@

clean:
	rm -f $(TARGET) $(OBJS)

