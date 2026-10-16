#include "asmem.h"
#include <stdint.h>
#include <stdio.h>
#include <sys/types.h>

int8_t read_key(Key* key){
    printf("Unesite kljuc u formatu [x x x x x]\n");
    uint8_t a = scanf("%" SCNu8  " %" SCNu8 " %" SCNu8 " %" SCNu8 " %" SCNu8,
                        &key->keyA, &key->keyB, &key->keyC, &key->keyD, &key->keyE);

    if (a != 5){
        return 0;
    }

    return 1;
}


int main(void){

    int choice;
    Key key;
    int32_t data;

    while (1) {
            printf("\n===== ASOCIJATIVNA MEMORIJA =====\n");
            printf("1. Unesi par kljuc-vrednost\n");
            printf("2. Pronadji vrednost po kljucu\n");
            printf("3. Ispisi sve parove\n");
            printf("4. Obrisi par po kljucu\n");
            printf("0. Izlaz\n");
            printf("Izbor: ");

            if (scanf("%d", &choice) != 1) {
                printf("Neispravan unos.\n");
                return 1;
            }

            switch (choice) {
                case 1:
                    /* Unos kljuca i vrednosti, pa asmem_insert(). */

                    read_key(&key);
                    scanf("%" SCNd32, &data);
                    asmem_insert(key, data);
                    break;

                case 2:
                    /* Unos kljuca, pa asmem_find_data(). */
                    read_key(&key);
                    asmem_find_data(key, &data);
                    printf("%" PRId32, data);
                    break;

                case 3:
                    /* Poziv asmem_print(). */
                    asmem_print();
                    break;

                case 4:
                    /* Unos kljuca, pa asmem_remove(). */
                    read_key(&key);
                    asmem_remove(key);
                    break;

                case 0:
                    asmem_destroy();
                    return 0;

                default:
                    printf("Nepostojeca opcija. Pokusaj ponovo.\n");
                    break;
            }
        }
}
