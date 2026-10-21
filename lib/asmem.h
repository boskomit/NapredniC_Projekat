#ifndef ASMEM_H_
#define ASMEM_H_

#include <stdint.h>

typedef enum : uint_least8_t
{
	ASMEM_OK = 0,
	ASMEM_KEY_EXISTS,
	ASMEM_NOT_FOUND,
	ASMEM_NO_MEMORY,
	ASMEM_INVALID_ARG
} AsmemStatus;

typedef struct
{
	uint_least8_t keyA;
	uint_least8_t keyB;
	uint_least8_t keyC;
	uint_least8_t keyD;
	uint_least8_t keyE;
} Key;

typedef void (*AsmemVisitFn)(Key key, int_least32_t data, void* user);

/* Ubacuje par. Ako kljuc vec postoji, lista se ne mijenja i vraca se ASMEM_KEY_EXISTS. */
AsmemStatus asmem_insert(Key key, int_least32_t data);

/* Upisuje vrijednost za dati kljuc u *result. result ne smije biti NULL (ASMEM_INVALID_ARG). */
AsmemStatus asmem_find_data(Key key, int_least32_t* result);

/* Brise par sa datim kljucem. Ako ga nema, vraca ASMEM_NOT_FOUND. */
AsmemStatus asmem_delete(Key key);

/* Poziva visit za svaki par (od najnovijeg ka najstarijem); user se samo prosljedjuje. */
void asmem_foreach(AsmemVisitFn visit, void* user);

/* Oslobadja cijelu listu. */
void asmem_destroy(void);

#endif
