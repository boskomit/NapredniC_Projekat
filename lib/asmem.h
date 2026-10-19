#ifndef ASMEM_H_
#define ASMEM_H_

#include <stdint.h>

typedef enum: uint8_t
{
    ASMEM_OK = 0,
    ASMEM_KEY_EXISTS,
    ASMEM_NOT_FOUND,
    ASMEM_NO_MEMORY,
    ASMEM_INVALID_ARG
}AsmemStatus;

typedef struct{
    uint8_t keyA;
    uint8_t keyB;
    uint8_t keyC;
    uint8_t keyD;
    uint8_t keyE;
}Key;

typedef void (*AsmemVisitFn)(Key key, int32_t data, void* user);

AsmemStatus asmem_insert(Key key, int32_t data);
AsmemStatus asmem_find_data(Key key, int32_t* result);
AsmemStatus asmem_delete(Key key);
void asmem_foreach(AsmemVisitFn visit, void* user);
void asmem_destroy(void);

#endif
