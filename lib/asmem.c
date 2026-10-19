#include "asmem.h"

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <stdbool.h>
#include <inttypes.h>

struct node{
    Key key;
    int32_t data;
    struct node* next;
};

static struct node* head = NULL;

static bool key_equal(Key key1, Key key2){
    return (key1.keyA == key2.keyA
        && key1.keyB == key2.keyB
        && key1.keyC == key2.keyC
        && key1.keyD == key2.keyD
        && key1.keyE == key2.keyE);
}

AsmemStatus asmem_insert(Key key, int32_t data){

    for(const struct node* current = head; current != NULL; current = current->next){

        if(key_equal(current->key, key)){
            return ASMEM_KEY_EXISTS;
        }
    }

    struct node* new_node = malloc(sizeof(struct node));

    if(new_node == NULL){
        return ASMEM_NO_MEMORY;
    }

    new_node->key = key;
    new_node->data = data;
    new_node->next = head;

    head = new_node;

    return ASMEM_OK;
}

AsmemStatus asmem_find_data(Key key, int32_t* result){

    assert(result != NULL);
    if(result == NULL){
        return ASMEM_INVALID_ARG;
    }

    for(const struct node* current = head; current != NULL; current = current->next){

        if(key_equal(current->key, key)){
            *result = current->data;
            return ASMEM_OK;
        }
    }

    return ASMEM_NOT_FOUND;
}

AsmemStatus asmem_delete(Key key){

    struct node* previous = NULL;

    for(struct node* current = head; current != NULL; current = current->next){
        if(key_equal(key, current->key)){
            if(previous == NULL){
                head = current->next;
            } else {
                previous->next = current->next;
            }
            free(current);
            return ASMEM_OK;
        }
        previous = current;
    }

    return ASMEM_NOT_FOUND;
}

void asmem_foreach(AsmemVisitFn visit, void* user){

    assert(visit != NULL);
    if(visit == NULL){
        return;
    }

    for(const struct node* current = head; current != NULL; current = current->next){
        visit(current->key, current->data, user);
    }
}

void asmem_destroy(void){
    struct node* current = head;
    struct node* next = NULL;

    while (current != NULL){
        next = current->next;
        free(current);
        current = next;
    }

    head = NULL;
}
