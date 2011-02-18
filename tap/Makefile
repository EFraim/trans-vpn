CFLAGS = -g -Wall

.PHONY: all
all: createtap

TARGET = createtap
OBJS = createtap.o

$(TARGET): $(OBJS) libopentap.so
	gcc $(OBJS) -L. -lopentap -Wl,-rpath,. -o $@

%.o: %.c
	gcc $(CFLAGS) $< -c -o $@

clean:
	rm -f $(TARGET) $(OBJS) libopentap.so

libopentap.so: opentap.c
	gcc -g -Wall opentap.c -shared -o $@

