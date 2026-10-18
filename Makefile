CC = gcc

CFLAGS = -Wall -Wextra -Wpedantic
MAIN_CFLAGS = -Wall -Wextra
CPPFLAGS = -Ilib

LIB_OBJ = lib/asmem.o
LIB = lib/libasmem.so
TARGET = program

.PHONY: all run clean

all: $(TARGET)

$(LIB_OBJ): lib/asmem.c lib/asmem.h
	$(CC) $(CPPFLAGS) $(CFLAGS) -fPIC -c $< -o $@

$(LIB): $(LIB_OBJ)
	$(CC) -shared -o $@ $^

$(TARGET): main.c lib/asmem.h $(LIB)
	$(CC) $(CPPFLAGS) $(MAIN_CFLAGS) main.c -o $@ -ldl

run: all
	./$(TARGET)

clean:
	rm -f $(TARGET) $(LIB_OBJ) $(LIB)

