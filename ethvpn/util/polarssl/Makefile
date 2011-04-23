TARGET = libpolarssl.so

POLARSSL_ROOT = ../../external/polarssl-0.14.0
POLARSSL_SRC = $(POLARSSL_ROOT)/library
POLARSSL_INC = $(POLARSSL_ROOT)/include

OBJS = polarssl_util.o
OBJS += $(POLARSSL_SRC)/bignum.o $(POLARSSL_SRC)/rsa.o $(POLARSSL_SRC)/aes.o $(POLARSSL_SRC)/padlock.o

CC = gcc
CFLAGS = -I. -I$(POLARSSL_INC) -g -Wall -Werror

$(TARGET): $(OBJS)
	$(CC) $(OBJS) -shared -o $@

%.o: %.c
	$(CC) $(CFLAGS) $< -c -o $@

clean:
	rm -f $(TARGET) $(OBJS) polarssl.pyc

