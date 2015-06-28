CC=clang
CFLAGS=-c -Wall -O3
LDFLAGS=
SOURCES=main.c msglist.c
OBJECTS=$(SOURCES:.cpp=.o)
EXECUTABLE=scheduler
PREFIX=/usr/bin

all: $(SOURCES) $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS)
	$(CC) $(LDFLAGS) $(OBJECTS) -o $@

.cpp.o:
	$(CC) $(CFLAGS) $< -o $@

clean:
	rm -f *.o
	rm $(EXECUTABLE)
install:
	cp $(EXECUTABLE) $(PREFIX)

remove:
	rm -vi $(PREFIX)/$(EXECUTABLE)
