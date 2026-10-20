CC = gcc

STD ?= -std=c23
EXTRA ?=

CFLAGS = $(STD) -Wall -Wextra -Wpedantic $(EXTRA)
MAIN_CFLAGS = $(STD) -Wall -Wextra $(EXTRA)
CPPFLAGS = -Ilib

LIB_OBJ = lib/asmem.o
LIB = lib/libasmem.so
OBJS = main.o util.o
TARGET = program

.PHONY: all run  clean

all: $(LIB) $(TARGET)

$(LIB_OBJ): lib/asmem.c lib/asmem.h
	$(CC) $(CPPFLAGS) $(CFLAGS) -fPIC -c $< -o $@

$(LIB): $(LIB_OBJ)
	$(CC) $(EXTRA) -shared -o $@ $^

main.o: main.c util.h lib/asmem.h
	$(CC) $(CPPFLAGS) $(MAIN_CFLAGS) -c $< -o $@

util.o: util.c util.h lib/asmem.h
	$(CC) $(CPPFLAGS) $(CFLAGS) -c $< -o $@

$(TARGET): $(OBJS)
	$(CC) $(EXTRA) $^ -o $@ -ldl

run: all
	./$(TARGET)

clean:
	rm -f $(TARGET) $(OBJS) $(LIB_OBJ) $(LIB)
