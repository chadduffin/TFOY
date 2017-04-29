CC = gcc
FK = -framework
OSX_LIBS = -I /Library/Frameworks/ $(FK) SDL2 $(FK) SDL2_net $(FK) SDL2_image
LINUX_LIBS = -lSDL2 -lSDL2_image -lSDL2_net -lm
WINDOWS_LIBS = -lSDL2main -lSDL2 -lSDL2_image -lSDL2_net -lm
CFLAGS = -g -Wall -pedantic -std=c11 -O3

TARGET = TFOY

OSX = __osx__
LINUX = __linux__
WINDOWS = __windows__

.PHONY: default all clean

all: default
default: osx

osx: $(OSX)
linux: $(LINUX)
windows: $(WINDOWS)

%.o: %.c $(HEADERS)
	$(CC) $(CFLAGS) -c $< -o $@

.PRECIOUS: $(TARGET) $(OBJECTS)

OBJECTS = $(patsubst %.c, %.o, $(wildcard *.c))
HEADERS = $(wildcard *.h)

$(OSX): $(OBJECTS)
	$(CC) $(OBJECTS) -Wall $(OSX_LIBS) -o $(TARGET)

$(LINUX): $(OBJECTS)
	$(CC) $(OBJECTS) -Wall $(LINUX_LIBS) -o $(TARGET)

$(WINDOWS): $(OBJECTS)
	$(CC) $(OBJECTS) -Wall $(WINDOWS_LIBS) -o $(TARGET)

clean:
	-rm -f *.o
	rm -f $(TARGET)
