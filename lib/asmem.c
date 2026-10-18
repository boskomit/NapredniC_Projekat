#include "asmem.h"


struct node{
    Key key;
    int32_t data;
    struct node* next;
};

static struct node* head = NULL;

static int8_t key_equal(Key key1, Key key2){
    return (key1.keyA == key2.keyA
        && key1.keyB == key2.keyB
        && key1.keyC == key2.keyC
        && key1.keyD == key2.keyD
        && key1.keyE == key2.keyE);
}

static void print_node(const struct node* node){
    printf("Kljuc: (%d %d %d %d %d) -> Vrijednost: %d\n",
            node->key.keyA, node->key.keyB, node->key.keyC, node->key.keyD,node->key.keyE, node->data);
}

int8_t asmem_insert(Key key, int32_t data){

    struct node *current = head;

    while (current != NULL) {
        if (key_equal(current->key, key)) {
            /* Kljuc vec postoji. */
            return KEY_EXISTS;
        }

        current = current->next;
    }

    struct node* newNode = malloc(sizeof(struct node));

    if(newNode != NULL){
        newNode->key = key;
        newNode->data = data;
        newNode->next = NULL;
    } else {
        return MALLOC_ERROR;
    }

    if(head == NULL){
        head = newNode;
    } else{
        newNode->next = head;
        head = newNode;
    }

    return SUCCESS;
}

int8_t asmem_find_data(Key key, int32_t* result){

    if(head == NULL || result == NULL){
        return LIST_EMPTY;
    } else {
        struct node* current = head;

        while(current != NULL){

            if(key_equal(current->key, key)){
                *result = current->data;
                return SUCCESS;
            }

            current = current->next;
        }

        return NOT_FOUND;
    }
}

int8_t asmem_delete(Key key){

    if(head == NULL){
        return LIST_EMPTY;
    } else {
        struct node* current = head;
        struct node* previous = NULL;

        while(current != NULL){

            if(key_equal(current->key, key)){
                if(previous == NULL){
                    head = current->next;
                } else {
                    previous->next = current->next;
                }
                free(current);
                return SUCCESS;
            }

            previous = current;
            current = current->next;
        }

        return NOT_FOUND;
    }
}

void asmem_print(void){

    struct node* current = head;

    if(current == NULL){
        printf("Lista je prazna\n");
    } else{
        while(current != NULL){

            print_node(current);
            current = current->next;
        }
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
