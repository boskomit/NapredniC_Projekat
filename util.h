#ifndef UTIL_H_
#define UTIL_H_

#include <stdint.h>

#include "asmem.h"

/* Cita jedan cijeli broj u opsegu int_least32_t. Vraca false za neispravan unos. */
bool util_read_int32(int_least32_t* result);

/* Cita kljuc: tacno pet brojeva 0-255 u jednom redu. *key se mijenja samo ako je unos ispravan. */
bool util_read_key(Key* key);

/* Callback za asmem_foreach: ispisuje par, a user mora biti pokazivac na size_t brojac. */
void util_print_pair(Key key, int_least32_t data, void* user);

/* Vraca true ako je error != NULL (i tada ispisuje poruku o gresci), inace false. */
bool util_check_error(const char* error);

#endif
