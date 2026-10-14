CC = gcc
CFLAGS = -Wall -Wextra -pthread -std=c99 -D_GNU_SOURCE -Iinclude
TARGET = sentinel-dispatcher
SRC = Main.c $(wildcard src/*.c)

all: $(TARGET)

$(TARGET): $(SRC) $(wildcard include/*.h)
	$(CC) $(CFLAGS) $(SRC) -o $@

clean:
	rm -f $(TARGET)