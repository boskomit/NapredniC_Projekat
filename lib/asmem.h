#ifndef ASMEM_H_
#define ASMEM_H_

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include <inttypes.h>

#include <sys/types.h>
#include <dlfcn.h>

#define LIST_EMPTY -1
#define MALLOC_ERROR 0
#define SUCCESS 1
#define KEY_EXISTS 2
#define NOT_FOUND 3

typedef struct{
    uint8_t keyA;
    uint8_t keyB;
    uint8_t keyC;
    uint8_t keyD;
    uint8_t keyE;
}Key;

int8_t asmem_insert(Key key, int32_t data);
int8_t asmem_find_data(Key key, int32_t* result);
int8_t asmem_delete(Key key);
void asmem_print(void);
void asmem_destroy(void);

#endif
