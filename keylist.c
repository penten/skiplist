#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include <limits.h>
#include "keylist.h"
#include "zmalloc.h"

kl *kl_create(void) {
    kl *head = zmalloc(sizeof(kl));
    head->first = NULL;
    return head;
}

void kl_destroy(kl *list) {
    klnode *next;
    klnode *this = list->first;
    while(this) {
        next = this->next;
        free(this);
        this = next;
    }
    free(list);
}

void *kl_find(const kl *list, int key) {
    klnode **nodep = klnode_locate(list, key);
    klnode *node = *nodep;

    if(node && node->key == key) return node->value;
    return NULL;
}

int kl_remove(kl *list, int key) {
    klnode **nodep = klnode_locate(list, key);
    klnode *node = *nodep;

    /* check if we found it or not */
    if(node && node->key == key) {
        *nodep = node->next;
        free(node);
        return 1;
    }
    return 0;
}

void kl_insert(kl *list, int key, void *value) {
    klnode *new;
    klnode **nodep = klnode_locate(list, key);
    klnode *node = *nodep;
    /* exact match, update */
    if(node && node->key == key) {
        node->value = value;
        return;
    }
    /* otherwise insert */
    new = klnode_create(key, value, node);
    *nodep = new;
}

void kl_output(const kl *list) {
    klnode *node = list->first;
    while(node) {
        printf("%d-->", node->key);
        node = node->next;
    }
    printf("#\n");
}

int kl_len(const kl *list) {
    int len = 0;
    klnode *node = list->first;
    while(node) {
        len++;
        node = node->next;
    }
    return len;
}

static klnode *klnode_create(int key, void *value, klnode *next) {
    klnode *new = zmalloc(sizeof(klnode));
    new->key = key;
    new->value = value;
    new->next = next;
    return new;
}

static klnode **klnode_locate(const kl *list, int key) {
    klnode **nodep = (klnode**)&list->first;

    while(*nodep && (*nodep)->key < key) {
        nodep = &(*nodep)->next;
    }
    return nodep;
}
