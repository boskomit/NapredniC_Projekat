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
 * Projekat ima tri modula:
 *  - lib/asmem.h, lib/asmem.c: biblioteka (libasmem.so). Kljuc je struktura Key sa
 *    pet polja tipa uint8_t, a vrijednost je int32_t. Parovi se cuvaju u jednostruko
 *    povezanoj listi. Biblioteka nudi: asmem_insert, asmem_find_data, asmem_delete,
 *    asmem_foreach i asmem_destroy. Rezultat operacija se vraca kao AsmemStatus
 *    (enum sa fiksnim tipom uint8_t, mogucnost C23 standarda).
 *    Biblioteka NE obavlja nikakav unos/ispis.
 *  - util.h, util.c: pomocne funkcije glavnog programa: unos sa provjerom
 *    ispravnosti (util_read_int32, util_read_key), ispis jednog para
 *    (util_print_pair) i ispis greske pri ucitavanju biblioteke (util_check_error).
 *  - main.c: sadrzi samo funkciju main. Biblioteku ucitava tokom izvrsavanja
 *    (dlopen), adrese funkcija dobija pomocu dlsym i poziva ih preko pokazivaca na
 *    funkcije. Program ima tekstualni meni (unos, pretraga, ispis svih parova,
 *    brisanje, izlaz).
 *  - Ispis cijele liste: main prosljedjuje biblioteci funkciju util_print_pair
 *    (callback), a biblioteka je poziva za svaki par u listi (asmem_foreach). Treci
 *    parametar (void* user) biblioteka samo prosljedjuje callback-u; ovdje se
 *    koristi za brojanje ispisanih parova, kako bi se prepoznala prazna lista.
 *
 * PREVODJENJE I POKRETANJE
 *  Pomocu Makefile-a: make (prevodi biblioteku i program), make run, make clean,
 *  make sanitize (prevodi sa AddressSanitizer i UndefinedBehaviorSanitizer),
 *  make test (sanitize + test.sh). Na kompajleru bez -std=c23 (npr. gcc 13)
 *  koristiti: make STD=-std=c2x
 *  Rucno (C23; na starijim kompajlerima umjesto -std=c23 koristiti -std=c2x):
 *    gcc -std=c23 -Ilib -Wall -Wextra -Wpedantic -fPIC -c lib/asmem.c -o lib/asmem.o
 *    gcc -shared -o lib/libasmem.so lib/asmem.o
 *    gcc -std=c23 -Ilib -Wall -Wextra -Wpedantic -c util.c -o util.o
 *    gcc -std=c23 -Ilib -Wall -Wextra -c main.c -o main.o
 *    gcc main.o util.o -o program -ldl
 *  Program se pokrece iz korijenskog foldera projekta (./program), jer je putanja
 *  do biblioteke relativna: ./lib/libasmem.so. Potreban je kompajler sa podrskom za
 *  C23 (npr. gcc 13 ili noviji).
 *
 * NEPRENOSIVI ELEMENTI
 *  1. dlopen, dlsym, dlclose, dlerror (<dlfcn.h>) nisu dio ISO C standarda, vec
 *     POSIX-a (Linux, BSD, macOS). Program zato radi na svakoj arhitekturi koju
 *     takav sistem podrzava. Na starijim glibc verzijama potrebno je linkovanje
 *     sa -ldl.
 *  2. Pretvaranje void* (povratna vrijednost dlsym) u pokazivac na funkciju nije
 *     definisano ISO C standardom (POSIX zahtijeva da radi). Zbog toga -Wpedantic
 *     prijavljuje upozorenje, pa se main.c prevodi bez te opcije. Mjesto primjene
 *     je oznaceno komentarom uz dlsym pozive.
 *
 * PRETPOSTAVKE O PLATFORMI (arhitekturi i kompajleru)
 *  - Tipovi uint8_t i int32_t moraju postojati (CHAR_BIT == 8). Na platformama bez
 *    njih kod se ne prevodi (greska pri prevodjenju, ne tiha greska).
 *  - Program ne zavisi od poretka bajtova (endianness) niti od poravnanja i
 *    popunjavanja struktura: kljucevi se porede polje po polje, a ne sa memcmp.
 *  - Key i AsmemStatus prelaze granicu biblioteke po vrijednosti, pa biblioteka i
 *    glavni program moraju biti prevedeni istim kompajlerom (isti ABI).
 *  - Koristi se C23: constexpr, enum sa fiksnim tipom, bool kao kljucna rijec i
 *    prazan inicijalizator {}.
 *
 * PRETPOSTAVKE I OGRANICENJA
 *  - Kljuc cini pet vrijednosti 0-255; dva kljuca su jednaka ako su sva polja jednaka.
 *  - Vrijednost je int32_t. Biblioteka nije generalna po tipu vrijednosti.
 *  - Ako kljuc vec postoji, asmem_insert ga ne mijenja i vraca ASMEM_KEY_EXISTS.
 *    Da bi se vrijednost promijenila, par se prvo obrise pa ponovo unese.
 *  - Broj parova je ogranicen samo raspolozivom memorijom (svaki par se zasebno
 *    alocira). Biblioteka drzi jednu globalnu asocijativnu memoriju (static head),
 *    pa nije moguce imati vise instanci niti je sigurna za upotrebu iz vise niti.
 *  - Korisnicki unos se cita po redovima (fgets, bafer od 128 znakova) i parsira
 *    sa strtol. Redovi duzi od 127 znakova se odbacuju kao neispravni. Kljuc je
 *    tacno pet cijelih brojeva 0-255 odvojenih razmacima, a vrijednost i izbor iz
 *    menija su cijeli brojevi u opsegu int32_t. Neispravan unos ne mijenja stanje
 *    liste. Kraj ulaza (EOF) se tretira kao izlaz iz programa.
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
 *  Analiza je uradjena rucno (bez alata za staticku analizu), pa lista ne mora biti
 *  potpuna.
 *  Prekrsena pravila (uz razlog):
 *  - 21.3: malloc/free u biblioteci (dinamicka lista je nuzna za neogranicen broj
 *    parova).
 *  - 21.6: stdio funkcije (printf, fgets, getchar) u main.c i util.c; potrebne za
 *    komunikaciju sa korisnikom.
 *  - 17.7 i Dir 4.7: ne koriste se povratne vrijednosti printf i poziva dlerror()
 *    koji samo brise staro stanje greske. Povratne vrijednosti fgets, strtol,
 *    dlopen, dlsym i dlclose se provjeravaju.
 *  - 11.1: pretvaranje void* iz dlsym u pokazivac na funkciju (vidi neprenosive
 *    elemente); ne moze se izbjeci kod ucitavanja biblioteke tokom izvrsavanja.
 *  - 11.5: pretvaranje void* u size_t* u callback-u util_print_pair.
 *  - Dir 4.6: koriste se int (povratna vrijednost main i getchar) i long (povratna
 *    vrijednost strtol) umjesto tipova sa fiksnom sirinom, jer te tipove odredjuju
 *    potpisi funkcija iz standardne biblioteke i ne mogu se promijeniti.
 *  Ispostovano: svaka funkcija ima jednu izlaznu tacku (15.5), exit se ne koristi
 *  (21.8), sve if/else/petlje imaju zagrade, svaki if ... else if lanac zavrsava sa
 *  else (15.7), svaki switch ima default i svaka grana zavrsava sa break, nema goto
 *  ni rekurzije, koriste se tipovi fiksne sirine (uint8_t, int32_t), bool iz C23.
 *
 * TESTIRANJE
 *  Testiranje je automatizovano skriptom test.sh (pokrece se sa: make test).
 *  Program i biblioteka se prevode sa -fsanitize=address,undefined, pa se uz
 *  funkcionalne greske hvataju i curenje memorije, upotreba oslobodjene memorije,
 *  prekoracenja i nedefinisano ponasanje. Skripta salje programu pripremljene
 *  ulaze preko stdin i provjerava izlaz (i da sanitizeri nisu prijavili gresku).
 *  Pokriveni slucajevi:
 *  - prazna lista: ispis, pretraga i brisanje;
 *  - unos pa pretraga, i unos duplikata (vrijednost se ne mijenja, u listi ostaje
 *    jedan par);
 *  - brisanje glave, sredine, repa i jedinog elementa, brisanje nepostojeceg kljuca;
 *  - granicne vrijednosti: kljuc 0 0 0 0 0 i 255 255 255 255 255, vrijednosti
 *    INT32_MIN i INT32_MAX;
 *  - neispravan kljuc (256, -1, premalo ili previse brojeva, slova, zarez, preveliki
 *    broj, prazan red) i neispravna vrijednost (slova, 2147483648, -2147483649,
 *    12abc, prazan red): provjerava se poruka o gresci i da lista ostaje nepromijenjena;
 *  - neispravan izbor u meniju, nepostojeca opcija, predugacak red (500 znakova);
 *  - kraj ulaza (EOF) odmah i usred unosa: program mora cisto zavrsiti;
 *  - veliki broj parova (3000): ispis, brisanje i oslobadjanje bez curenja;
 *  - nepostojeca biblioteka i biblioteka bez trazenih simbola: poruka o gresci i
 *    izlazni kod 1.
 *  Razlog za ovakav pristup: pojedinacne granice (prazna lista, glava/sredina/rep,
 *  0 i 255, INT32_MIN/MAX) su mjesta gdje se najcesce prave greske u radu sa
 *  povezanom listom i ulazom, a sanitizeri hvataju greske koje se ne vide u izlazu.
 */

#include "asmem.h"
#include "util.h"

#include <inttypes.h>
#include <stdint.h>
#include <stdio.h>

#include <dlfcn.h>

static AsmemStatus (*insert)(Key, int32_t) = NULL;
static AsmemStatus (*find_data)(Key, int32_t*) = NULL;
static AsmemStatus (*delete)(Key) = NULL;
static void (*foreach)(AsmemVisitFn, void*) = NULL;
static void (*destroy)(void) = NULL;

int main(void)
{
	int status = 0;
	bool loaded = false;
	bool running = false;

	void* library = dlopen("./lib/libasmem.so", RTLD_NOW);

	if (library == NULL)
	{
		printf("Greska %s\n", dlerror());
		status = 1;
	}
	else
	{
		const char* error = NULL;

		loaded = true;

		/* Neprenosivo: void* -> pokazivac na funkciju (POSIX), vidi komentar na pocetku. */
		/* Vazi za sve dlsym pozive */
		dlerror();
		insert = (AsmemStatus (*)(Key, int32_t)) dlsym(library, "asmem_insert");
		error = dlerror();
		loaded = !util_check_error(error) && loaded;

		dlerror();
		find_data = (AsmemStatus (*)(Key, int32_t*)) dlsym(library, "asmem_find_data");
		error = dlerror();
		loaded = !util_check_error(error) && loaded;

		dlerror();
		delete = (AsmemStatus (*)(Key)) dlsym(library, "asmem_delete");
		error = dlerror();
		loaded = !util_check_error(error) && loaded;

		dlerror();
		foreach = (void (*)(AsmemVisitFn, void*)) dlsym(library, "asmem_foreach");
		error = dlerror();
		loaded = !util_check_error(error) && loaded;

		dlerror();
		destroy = (void (*)(void)) dlsym(library, "asmem_destroy");
		error = dlerror();
		loaded = !util_check_error(error) && loaded;

		running = loaded;

		if (!loaded)
		{
			status = 1;
		}
	}

	int32_t choice = 0;
	Key key = {};
	int32_t data = 0;
	AsmemStatus ret = ASMEM_OK;
	size_t count = 0U;

	while (running)
	{
		printf("\n===== ASOCIJATIVNA MEMORIJA =====\n");
		printf("1. Unesi par kljuc-vrednost\n");
		printf("2. Pronadji vrednost po kljucu\n");
		printf("3. Ispisi sve parove\n");
		printf("4. Obrisi par po kljucu\n");
		printf("0. Izlaz\n");
		printf("Izbor: ");

		bool valid = util_read_int32(&choice);

		if (!valid && feof(stdin) != 0)
		{
			choice = 0;
			valid = true;
		}

		if (!valid)
		{
			printf("Neispravan unos.\n");
		}
		else
		{
			switch (choice)
			{
				case 1:
					/* Unos kljuca i vrednosti, pa asmem_insert(). */
					if (!util_read_key(&key))
					{
						printf("Neispravan unos kljuca\n");
						break;
					}
					printf("Unesite vrijednost: ");
					if (!util_read_int32(&data))
					{
						printf("Neispravna vrijednost\n");
						break;
					}

					ret = insert(key, data);

					if (ret == ASMEM_KEY_EXISTS)
					{
						printf("Dati kljuc vec postoji!\n");
					}
					else if (ret == ASMEM_NO_MEMORY)
					{
						printf("Nema memorije\n");
						status = 1;
						running = false;
					}
					else
					{
						/* ASMEM_OK: nema ispisa. */
					}
					break;

				case 2:
					/* Unos kljuca, pa asmem_find_data(). */
					if (!util_read_key(&key))
					{
						printf("Neispravan unos kljuca\n");
						break;
					}

					ret = find_data(key, &data);

					if (ret == ASMEM_NOT_FOUND)
					{
						printf("Vrijednost za dati kljuc ne postoji\n");
					}
					else if (ret == ASMEM_OK)
					{
						printf("Vrijednost: %" PRId32 "\n", data);
					}
					else
					{
						/* ASMEM_INVALID_ARG se ne moze desiti: result nije NULL. */
					}
					break;

				case 3:
					/* Poziv asmem_foreach(). */
					count = 0U;
					foreach(util_print_pair, &count);

					if (count == 0U)
					{
						printf("Lista je prazna\n");
					}
					break;

				case 4:
					/* Unos kljuca, pa asmem_delete(). */
					if (!util_read_key(&key))
					{
						printf("Neispravan unos kljuca\n");
						break;
					}

					ret = delete(key);

					if (ret == ASMEM_OK)
					{
						printf("Par je obrisan\n");
					}
					else if (ret == ASMEM_NOT_FOUND)
					{
						printf("Vrijednost za dati kljuc ne postoji!\n");
					}
					else
					{
						/* Ostali kodovi se ne mogu desiti pri brisanju. */
					}
					break;

				case 0:
					running = false;
					break;

				default:
					printf("Nepostojeca opcija. Pokusaj ponovo.\n");
					break;
			}
		}
	}

	if (loaded)
	{
		destroy();
	}

	if (library != NULL)
	{
		if (dlclose(library) != 0)
		{
			printf("Greska pri zatvaranju biblioteke: %s\n", dlerror());
			status = 1;
		}
	}

	return status;
}
