CC = gcc
CFLAGS = -g -Wall -pedantic -std=c11 -O3

OSX = -I /Library/Frameworks/ -framework SDL2 -framework SDL2_net -framework SDL2_image
LINUX = -lSDL2 -lSDL2_image -lSDL2_net -lm
WINDOWS = -lSDL2main -lSDL2 -lSDL2_image -lSDL2_net -lm

OBJECTS = $(patsubst %.c,%.o,$(wildcard *.c))
HEADERS = $(wildcard *.h)

TARGET = TFOY

%.o: %.c $(HEADERS)
	$(CC) $(CFLAGS) -c $< -o $@

all: osx

run: all
	./$(TARGET)

osx: $(OBJECTS)
	$(CC) $(OBJECTS) -Wall $(OSX) -o $(TARGET)

linux: $(OBJECTS)
	$(CC) $(OBJECTS) -Wall $(LINUX) -o $(TARGET)

windows: $(OBJECTS)
	$(CC) $(OBJECTS) -Wall $(WINDOWS) -o $(TARGET)

clean:
	-rm -f *.o
	rm -f $(TARGET)
