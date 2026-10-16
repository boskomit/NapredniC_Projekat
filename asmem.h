#ifndef ASMEM_H_
#define ASMEM_H_

#include <inttypes.h>

typedef struct{
    uint8_t keyA;
    uint8_t keyB;
    uint8_t keyC;
    uint8_t keyD;
    uint8_t keyE;
}Key;

int8_t asmem_insert(Key key, int32_t data);
int8_t asmem_find_data(Key key, int32_t* result);
int8_t asmem_remove(Key key);
void asmem_print(void);
void asmem_destroy(void);

#endif
