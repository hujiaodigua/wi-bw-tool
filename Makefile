# Makefile for wi_bw_tool

CC = gcc
CFLAGS = -Wall -Wextra
TARGET = wi_bw_tool
SRC = wi_bw_tool.c

.PHONY: all clean run debug

all: $(TARGET)

$(TARGET): $(SRC)
	$(CC) -o $(TARGET) $(SRC) $(CFLAGS)

debug: $(SRC)
	$(CC) -g -O0 -o $(TARGET) $(SRC) $(CFLAGS)

run: $(TARGET)
	./$(TARGET)

clean:
	rm -f $(TARGET)
