CC = gcc
SRCS = $(shell ls *.c)
OBJS = $(SRCS:.c=.o)
TARGET = rtee

PREFIX = /usr
BINDIR = bin

.PHONY: all clean

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) -o $@ $<

%.o: %.c
	$(CC) -c -o $@ $<

clean:
	rm -f $(TARGET) $(OBJS)

install: $(TARGET)
	install -m 755 -d $(PREFIX)/$(BINDIR)
	install -m 755 -t $(PREFIX)/$(BINDIR) $(TARGET)
