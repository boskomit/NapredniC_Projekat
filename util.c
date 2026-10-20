#include "util.h"

#include <ctype.h>
#include <errno.h>
#include <inttypes.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

constexpr size_t LINE_SIZE = 128U;
constexpr size_t KEY_FIELDS = 5U;

static bool read_line(char* buffer, size_t size);
static const char* skip_spaces(const char* text);

static bool read_line(char* buffer, size_t size)
{
	bool valid = false;

	if (fgets(buffer, (int)size, stdin) != NULL)
	{
		const size_t length = strlen(buffer);

		if (length > 0U && buffer[length - 1U] == '\n')
		{
			buffer[length - 1U] = '\0';
			valid = true;
		}
		else if (length == size - 1U)
		{
			/* Red je predug: odbaci ostatak reda. */
			int c = getchar();

			while (c != '\n' && c != EOF)
			{
				c = getchar();
			}
		}
		else
		{
			/* Zadnji red bez novog reda (kraj ulaza). */
			valid = true;
		}
	}

	return valid;
}

static const char* skip_spaces(const char* text)
{
	const char* position = text;

	while (isspace((unsigned char)*position) != 0)
	{
		position++;
	}

	return position;
}

bool util_read_int32(int32_t* result)
{
	char line[LINE_SIZE] = {};
	bool valid = false;

	if (read_line(line, sizeof(line)))
	{
		char* end = NULL;

		errno = 0;
		const long number = strtol(line, &end, 10);

		if (end != line && errno == 0 && *skip_spaces(end) == '\0'
			&& number >= INT32_MIN && number <= INT32_MAX)
		{
			*result = (int32_t)number;
			valid = true;
		}
	}

	return valid;
}

bool util_read_key(Key* key)
{
	char line[LINE_SIZE] = {};
	long fields[KEY_FIELDS] = {};
	bool valid = false;

	printf("Unesite kljuc u formatu [x x x x x]\n");

	if (read_line(line, sizeof(line)))
	{
		const char* position = line;
		size_t count = 0U;
		bool ok = true;

		while (ok && count < KEY_FIELDS)
		{
			char* end = NULL;

			errno = 0;
			const long number = strtol(position, &end, 10);

			if (end == position || errno != 0 || number < 0 || number > UINT8_MAX)
			{
				ok = false;
			}
			else
			{
				fields[count] = number;
				count++;
				position = end;
			}
		}

		if (ok && *skip_spaces(position) == '\0')
		{
			key->keyA = (uint8_t)fields[0];
			key->keyB = (uint8_t)fields[1];
			key->keyC = (uint8_t)fields[2];
			key->keyD = (uint8_t)fields[3];
			key->keyE = (uint8_t)fields[4];
			valid = true;
		}
	}

	return valid;
}

void util_print_pair(Key key, int32_t data, void* user)
{
	size_t* count = user;

	printf("Kljuc: (%" PRIu8 " %" PRIu8 " %" PRIu8 " %" PRIu8 " %" PRIu8 ") -> Vrijednost: %" PRId32 "\n",
		key.keyA, key.keyB, key.keyC, key.keyD, key.keyE, data);

	(*count)++;
}

bool util_check_error(const char* error)
{
	bool status = false;

	if (error != NULL)
	{
		printf("Greska: %s\n", error);
		status = true;
	}

	return status;
}
