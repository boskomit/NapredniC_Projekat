#include "asmem.h"

#include <assert.h>
#include <stdlib.h>

struct node
{
	Key key;
	int32_t data;
	struct node* next;
};

static struct node* head = NULL;

static bool key_equal(Key key1, Key key2)
{
	return (key1.keyA == key2.keyA
		&& key1.keyB == key2.keyB
		&& key1.keyC == key2.keyC
		&& key1.keyD == key2.keyD
		&& key1.keyE == key2.keyE);
}

AsmemStatus asmem_insert(Key key, int32_t data)
{
	AsmemStatus status = ASMEM_OK;
	const struct node* current = head;

	while (current != NULL && status == ASMEM_OK)
	{
		if (key_equal(current->key, key))
		{
			status = ASMEM_KEY_EXISTS;
		}
		current = current->next;
	}

	if (status == ASMEM_OK)
	{
		struct node* new_node = malloc(sizeof(struct node));

		if (new_node == NULL)
		{
			status = ASMEM_NO_MEMORY;
		}
		else
		{
			new_node->key = key;
			new_node->data = data;
			new_node->next = head;
			head = new_node;
		}
	}

	return status;
}

AsmemStatus asmem_find_data(Key key, int32_t* result)
{
	AsmemStatus status = ASMEM_NOT_FOUND;

	assert(result != NULL);
	if (result == NULL)
	{
		status = ASMEM_INVALID_ARG;
	}
	else
	{
		const struct node* current = head;

		while (current != NULL && status == ASMEM_NOT_FOUND)
		{
			if (key_equal(current->key, key))
			{
				*result = current->data;
				status = ASMEM_OK;
			}
			current = current->next;
		}
	}

	return status;
}

AsmemStatus asmem_delete(Key key)
{
	AsmemStatus status = ASMEM_NOT_FOUND;
	struct node* previous = NULL;
	struct node* current = head;

	while (current != NULL && status == ASMEM_NOT_FOUND)
	{
		if (key_equal(key, current->key))
		{
			if (previous == NULL)
			{
				head = current->next;
			}
			else
			{
				previous->next = current->next;
			}
			free(current);
			status = ASMEM_OK;
		}
		else
		{
			previous = current;
			current = current->next;
		}
	}

	return status;
}

void asmem_foreach(AsmemVisitFn visit, void* user)
{
	assert(visit != NULL);
	if (visit != NULL)
	{
		const struct node* current = head;

		while (current != NULL)
		{
			visit(current->key, current->data, user);
			current = current->next;
		}
	}
}

void asmem_destroy(void)
{
	struct node* current = head;
	struct node* next = NULL;

	while (current != NULL)
	{
		next = current->next;
		free(current);
		current = next;
	}

	head = NULL;
}
