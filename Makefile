TARGET = FindRoute
LIBS =
CC = gcc
CFLAGS = -g -std=c99

OBJECTS = main.o List.o status.o Map.o
HEADERS = List.h Map.h status.h

.PHONY: default all clean

default: $(TARGET)
all: default

%.o: %.c $(HEADERS)
	$(CC) $(CFLAGS) -c $< -o $@

$(TARGET): $(OBJECTS)
	$(CC) $(OBJECTS) $(LIBS) -o $@

clean:
	-rm -f *.o
	-rm -f $(TARGET)