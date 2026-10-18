#include "asmem.h"

int8_t read_key(Key* key);
void check_error(const char* error, void* library);

int8_t (*insert)(Key,int32_t);
int8_t (*find_data)(Key,int32_t*);
int8_t (*delete)(Key);
void (*print)(void);
void (*destroy)(void);

int main(void){

    void* library = dlopen("./lib/libasmem.so", RTLD_NOW);

    const char *error;

    if(library == NULL){
        printf("Greska %s\n", dlerror());
        return 1;
    }

    dlerror();
    insert = (int8_t (*)(Key, int32_t)) dlsym(library, "asmem_insert");
    error = dlerror();
    check_error(error, library);

    dlerror();
    find_data = (int8_t (*)(Key, int32_t *)) dlsym(library, "asmem_find_data");
    error = dlerror();
    check_error(error, library);

    dlerror();
    delete = (int8_t (*)(Key)) dlsym(library, "asmem_delete");
    error = dlerror();
    check_error(error, library);

    dlerror();
    print = (void (*)(void)) dlsym(library, "asmem_print");
    error = dlerror();
    check_error(error, library);

    dlerror();
    destroy = (void (*)(void)) dlsym(library, "asmem_destroy");
    error = dlerror();
    check_error(error, library);

    int choice;
    Key key;
    int32_t data;
    int8_t ret;

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
                    printf("Unesite vrijednost: ");
                    scanf("%" SCNd32, &data);
                    ret = insert(key,data);
                    if(ret == KEY_EXISTS){
                        printf("Dati kljuc vec postoji!\n");
                        break;
                    } else if (ret == MALLOC_ERROR){
                        printf("Malloc Error\n");
                        return 1;
                    } else {
                         break;
                    }

                case 2:
                    /* Unos kljuca, pa asmem_find_data(). */
                    read_key(&key);
                    ret = find_data(key, &data);
                    if(ret == LIST_EMPTY){
                        printf("Prazna lista\n");
                        break;
                    } else if (ret == NOT_FOUND){
                        printf("Vrijednost za dati kljuc ne postoji!\n");
                        break;
                    } else {
                        printf("Vrijednost: ");
                        printf("%" PRId32 "\n", data);
                        break;
                    }



                case 3:
                    /* Poziv asmem_print(). */
                    print();
                    break;

                case 4:
                    /* Unos kljuca, pa asmem_remove(). */
                    read_key(&key);
                    ret = delete(key);
                    if(ret == LIST_EMPTY){
                        printf("Prazna lista\n");
                        break;
                    } else if (ret == NOT_FOUND){
                        printf("Vrijednost za dati kljuc ne postoji!\n");
                        break;
                    } else {
                        break;
                    }

                case 0:
                    destroy();
                    dlclose(library);
                    return 0;

                default:
                    printf("Nepostojeca opcija. Pokusaj ponovo.\n");
                    break;
            }
        }
}


int8_t read_key(Key* key){
    printf("Unesite kljuc u formatu [x x x x x]\n");
    uint8_t a = scanf("%" SCNu8  " %" SCNu8 " %" SCNu8 " %" SCNu8 " %" SCNu8,
                        &key->keyA, &key->keyB, &key->keyC, &key->keyD, &key->keyE);

    if (a != 5){
        return 0;
    }

    return 1;
}

void check_error(const char* error, void* library){

    if (error != NULL) {
        printf("Greska: %s\n", error);
        dlclose(library);
        exit(1);
    }
}
