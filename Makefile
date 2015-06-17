CC = gcc
SRCS = $(shell ls *.c)
OBJS = $(SRCS:.c=.o)
MANS = $(shell ls *.1)
MANSGZ = $(MANS:.1=.1.gz)
TARGET = rtee
GZIP = gzip

DESTDIR= 
PREFIX = /usr
BINDIR = $(PREFIX)/bin
MANDIR = $(PREFIX)/share/man

.PHONY: all clean

all: $(TARGET)

$(TARGET): $(OBJS) $(MANSGZ)
	$(CC) -o $@ $<

%.o: %.c
	$(CC) -c -o $@ $<

%.1.gz: %.1
	$(GZIP) -c $< > $@

clean:
	rm -f $(TARGET) $(OBJS) $(MANSGZ)

install: $(TARGET) 
	install -m 755 -d $(DESTDIR)$(BINDIR)
	install -m 755 -t $(DESTDIR)$(BINDIR) $(TARGET)
	install -m 755 -d $(DESTDIR)$(MANDIR)/man1
	install -m 644 -t $(DESTDIR)$(MANDIR)/man1 $(MANSGZ)
