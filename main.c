/*
 * ZADATAK (br. 50)
 * Napisati biblioteku koja omogucava pravljenje asocijativne memorije kod koje je
 * kljuc struktura podataka sa pet celobrojnih polja, tj. preslikavanje tog kljuca
 * na neke druge elemente. Biblioteku realizovati kao dinamicku sa povezivanjem tokom
 * izvrsavanja i ukljuciti je u glavni program koji obavlja komunikaciju sa
 * okruzenjem (unos/ispis).
 *
 * AUTOR: Boško Mitrović, indeks RA48/2023
 *
 * KRATAK OPIS RJESENJA
 * Projekat ima dva modula:
 *  - lib/asmem.h, lib/asmem.c: biblioteka (libasmem.so). Kljuc je struktura Key sa
 *    pet polja tipa uint8_t, a vrijednost je int32_t. Parovi se cuvaju u jednostruko
 *    povezanoj listi. Biblioteka nudi: asmem_insert, asmem_find_data, asmem_delete,
 *    asmem_foreach i asmem_destroy. Rezultat operacija se vraca kao AsmemStatus
 *    (enum sa fiksnim tipom uint8_t, mogucnost C23 standarda).
 *    Biblioteka NE obavlja nikakav unos/ispis.
 *  - main.c: glavni program. Biblioteku ucitava tokom izvrsavanja (dlopen), adrese
 *    funkcija dobija pomocu dlsym i poziva ih preko pokazivaca na funkcije. Program
 *    ima tekstualni meni (unos, pretraga, ispis svih parova, brisanje, izlaz).
 *  - Ispis cijele liste: main prosljedjuje biblioteci funkciju print_pair (callback),
 *    a biblioteka je poziva za svaki par u listi (asmem_foreach). Treci parametar
 *    (void* user) biblioteka samo prosljedjuje callback-u; ovdje se koristi za
 *    brojanje ispisanih parova, kako bi se prepoznala prazna lista.
 *
 * PREVODJENJE I POKRETANJE
 *  Pomocu Makefile-a: make (prevodi biblioteku i program), make run, make clean.
 *  Rucno (C23; na starijim kompajlerima umjesto -std=c23 koristiti -std=c2x):
 *    gcc -std=c23 -Ilib -Wall -Wextra -Wpedantic -fPIC -c lib/asmem.c -o lib/asmem.o
 *    gcc -shared -o lib/libasmem.so lib/asmem.o
 *    gcc -std=c23 -Ilib -Wall -Wextra main.c -o program -ldl
 *  Program se pokrece iz korijenskog foldera projekta (./program), jer je putanja
 *  do biblioteke relativna: ./lib/libasmem.so. Potreban je kompajler sa podrskom za
 *  C23 (npr. gcc 13 ili noviji).
 *
 * NEPRENOSIVI ELEMENTI
 *  1. dlopen, dlsym, dlclose, dlerror (<dlfcn.h>) nisu dio ISO C standarda, vec
 *     POSIX-a. Na Windows-u bi trebalo koristiti LoadLibrary/GetProcAddress/
 *     FreeLibrary. Ta varijanta nije implementirana. Na starijim glibc verzijama
 *     potrebno je linkovanje sa -ldl.
 *  2. Pretvaranje void* (povratna vrijednost dlsym) u pokazivac na funkciju nije
 *     definisano ISO C standardom (POSIX zahtijeva da radi). Zbog toga -Wpedantic
 *     prijavljuje upozorenje, pa se main.c prevodi bez te opcije. Mjesto primjene
 *     je oznaceno komentarom uz dlsym pozive.
 *  3. Biblioteka je u .so formatu (ELF). Na Windows-u bi izvezene funkcije trebalo
 *     oznaciti sa __declspec(dllexport).
 *
 * PRETPOSTAVKE I OGRANICENJA
 *  - Kljuc cini pet vrijednosti 0-255; dva kljuca su jednaka ako su sva polja jednaka.
 *  - Vrijednost je int32_t. Biblioteka nije generalna po tipu vrijednosti.
 *  - Ako kljuc vec postoji, asmem_insert ga ne mijenja i vraca ASMEM_KEY_EXISTS.
 *    Da bi se vrijednost promijenila, par se prvo obrise pa ponovo unese.
 *  - Broj parova je ogranicen samo raspolozivom memorijom (svaki par se zasebno
 *    alocira). Biblioteka drzi jednu globalnu asocijativnu memoriju (static head),
 *    pa nije moguce imati vise instanci niti je sigurna za upotrebu iz vise niti.
 *  - Korisnicki unos se cita pomocu scanf.
 *  - Asserti u biblioteci se iskljucuju sa -DNDEBUG; provjere argumenata koje
 *    slijede iza njih (npr. result == NULL) ostaju aktivne.
 *
 * IZBOR KRITERIJUMA: MEMORIJA
 *  Odabran je kriterijum male memorije. Koristi se jednostruko povezana lista bez
 *  dodatnih struktura (hash tabela, stablo, indeksi), pa je utrosak memorije po paru
 *  minimalan (kljuc + vrijednost + jedan pokazivac). Cijena je brzina: pretraga,
 *  brisanje i unos (zbog provjere duplikata) su O(n).
 *
 * MISRA C ANALIZA
 *  Prekrsena pravila (uz razlog):
 *  - 21.3: malloc/free u biblioteci (dinamicka lista je nuzna za neogranicen broj
 *    parova).
 *  - 21.6: stdio funkcije (printf, scanf) u main.c; potrebne za komunikaciju sa
 *    korisnikom.
 *  - 21.8: exit() u funkciji check_error.
 *  - 17.7 i Dir 4.7: ne koriste se sve povratne vrijednosti (printf, dlclose,
 *    dlerror, scanf za vrijednost). [Ukloniti ako se ispravi.]
 *  - 11.1: pretvaranje void* iz dlsym u pokazivac na funkciju (vidi neprenosive
 *    elemente); ne moze se izbjeci kod ucitavanja biblioteke tokom izvrsavanja.
 *  - 11.5: pretvaranje void* u size_t* u callback-u print_pair.
 *  - 15.5: vise izlaznih tacaka iz funkcija (vise return naredbi).
 *  - Dir 4.6: koristi se int (povratna vrijednost scanf i main) umjesto tipova
 *    sa fiksnom sirinom, jer taj tip odredjuje potpis funkcije iz standardne biblioteke
 *    i ne moze se promijeniti
 *  Ispostovano: sve if/else/petlje imaju zagrade, svaki switch ima default i svaka
 *  grana zavrsava sa break, nema goto ni rekurzije, koriste se tipovi fiksne
 *  sirine (uint8_t, int32_t), bool iz C23.
 *
 * TESTIRANJE

 */

#include "asmem.h"

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>

#include <dlfcn.h>

static int8_t read_key(Key* key);
static void check_error(const char* error, void* library);
static void print_pair(Key key, int32_t data, void* user);

static AsmemStatus (*insert)(Key,int32_t);
static AsmemStatus (*find_data)(Key,int32_t*);
static AsmemStatus (*delete)(Key);
static void (*foreach) (AsmemVisitFn, void*);
static void (*destroy)(void);

int main(void){

    void* library = dlopen("./lib/libasmem.so", RTLD_NOW);

    const char *error;

    if(library == NULL){
        printf("Greska %s\n", dlerror());
        return 1;
    }

    /* Neprenosivo: void* -> pokazivac na funkciju (POSIX), vidi komentar na pocetku. */
    /* Vazi za sve dlsym pozive */
    dlerror();
    insert = (AsmemStatus (*)(Key, int32_t)) dlsym(library, "asmem_insert");
    error = dlerror();
    check_error(error, library);

    dlerror();
    find_data = (AsmemStatus (*)(Key, int32_t *)) dlsym(library, "asmem_find_data");
    error = dlerror();
    check_error(error, library);

    dlerror();
    delete = (AsmemStatus (*)(Key)) dlsym(library, "asmem_delete");
    error = dlerror();
    check_error(error, library);

    dlerror();
    foreach = (void (*)(AsmemVisitFn,void *)) dlsym(library,"asmem_foreach");
    error = dlerror();
    check_error(error, library);

    dlerror();
    destroy = (void (*)(void)) dlsym(library, "asmem_destroy");
    error = dlerror();
    check_error(error, library);

    int8_t choice;
    Key key;
    int32_t data;
    AsmemStatus ret;
    size_t count;

    while (1) {
            printf("\n===== ASOCIJATIVNA MEMORIJA =====\n");
            printf("1. Unesi par kljuc-vrednost\n");
            printf("2. Pronadji vrednost po kljucu\n");
            printf("3. Ispisi sve parove\n");
            printf("4. Obrisi par po kljucu\n");
            printf("0. Izlaz\n");
            printf("Izbor: ");

            if (scanf("%" SCNd8, &choice) != 1) {
                printf("Neispravan unos.\n");
                return 1;
            }

            switch (choice) {
                case 1:
                    /* Unos kljuca i vrednosti, pa asmem_insert(). */
                    if(!read_key(&key)){
                        printf("Neispravan unos kljuca\n");
                        break;
                    }
                    printf("Unesite vrijednost: ");
                    scanf("%" SCNd32, &data);
                    ret = insert(key,data);
                    if(ret == ASMEM_KEY_EXISTS){
                        printf("Dati kljuc vec postoji!\n");
                        break;
                    } else if (ret == ASMEM_NO_MEMORY){
                        printf("Nema memorije\n");
                        return 1;
                    } else {
                         break;
                    }

                case 2:
                    /* Unos kljuca, pa asmem_find_data(). */
                    if(!read_key(&key)){
                        printf("Neispravan unos kljuca\n");
                        break;
                    }
                    ret = find_data(key, &data);
                    if(ret == ASMEM_NOT_FOUND){
                        printf("Vrijednost za dati kljuc ne postoji\n");
                        break;
                    } else if (ret == ASMEM_OK){
                        printf("Vrijednost: %" PRId32 "\n", data);
                        break;
                    } else{
                        break;
                    }

                case 3:
                    /* Poziv asmem_print(). */
                    count = 0;
                    foreach(print_pair,&count);
                    if (count == 0){
                        printf("Lista je prazna\n");
                    }
                    break;

                case 4:
                    /* Unos kljuca, pa asmem_remove(). */
                    if(!read_key(&key)){
                        printf("Neispravan unos kljuca\n");
                        break;
                    }
                    ret = delete(key);
                    if(ret == ASMEM_OK){
                        printf("Par je obrisan\n");
                        break;
                    } else if (ret == ASMEM_NOT_FOUND){
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


static int8_t read_key(Key* key){
    printf("Unesite kljuc u formatu [x x x x x]\n");
    int a = scanf("%" SCNu8  " %" SCNu8 " %" SCNu8 " %" SCNu8 " %" SCNu8,
                        &key->keyA, &key->keyB, &key->keyC, &key->keyD, &key->keyE);

    if (a != 5){
        return 0;
    }

    return 1;
}

static void check_error(const char* error, void* library){

    if (error != NULL) {
        printf("Greska: %s\n", error);
        dlclose(library);
        exit(1);
    }
}

static void print_pair(Key key, int32_t data, void* user){

    size_t* count = user;

    printf("Kljuc: (%" PRIu8 " %" PRIu8 " %" PRIu8 " %" PRIu8 " %" PRIu8 ") -> Vrijednost: %" PRId32 "\n",
            key.keyA, key.keyB, key.keyC, key.keyD, key.keyE, data);

    (*count)++;
}
