#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include <limits.h>
#include "skiplist.h"
#include "zmalloc.h"

sl *sl_create(void) {
    sl *head;

    head = zmalloc(sizeof(sl));
    head->first = zmalloc(sizeof(slnode*)*(SL_MAXLEVEL+1));
    head->first[0] = NULL;
    head->level = 0;
    return head;
}

void sl_destroy(sl *list) {
    slnode *node = list->first[0];
    slnode *last = node;
    while(node) {
        node = last->next[0];
        free(last->next);
        free(last);
        last = node;
    }
    free(list);
    free(list->first);
}

void *sl_find(const sl *list, int key) {
    slnode **nodep;

    nodep = list->first;
    /* search starting at the top level */
    for(int lvl = list->level; lvl >= 0; lvl--) {
        while(nodep[lvl] && nodep[lvl]->key < key) {
            nodep = nodep[lvl]->next;
        }
    }
    if(nodep[0]->key == key) return nodep[0]->value;
    return NULL;
}

int sl_remove(sl *list, int key) {
    slnode ***update;
    slnode *node;

    update = slnode_locate(list, key);
    node = update[0][0];

    if(node && node->key == key) {
        for(int lvl = node->level; lvl >= 0 ; lvl--) {
            *(update[lvl]) = node->next[lvl];
        }
        /* resize list */
        for(int lvl = list->level; lvl > 0; lvl--) {
            if(list->first[lvl]) {
                list->level = lvl;
                break;
            }
        }

        slnode_destroy(node);
        free(update);
        return 1;
    }
    free(update);
    return 0;
}

void sl_insert(sl *list, int key, void *const value) {
    slnode *new;
    slnode *node;
    slnode ***update;

    update = slnode_locate(list, key);
    node = update[0][0]; /* update[0] = address of array pointing at the node */
    
    /* exact match? update */
    if(node && node->key == key) {
        node->value = value;
    } else {
        /* otherwise insert */
        new = slnode_create(key, value);
        /* if the new level is larger than the max level, grow the list */
        if(new->level > list->level) {
            update = zrealloc(update, (new->level+1) * sizeof(slnode***));
            for(int lvl = new->level; lvl > list->level; lvl--) {
                list->first[lvl] = NULL;
                update[lvl] = &(list->first[lvl]);
            }
            list->level = new->level;
        }
        for(int lvl = new->level; lvl >= 0; lvl--) {
            new->next[lvl] = *(update[lvl]);
            *(update[lvl]) = new;
        }
    }

    free(update);
}

void sl_output(const sl *list) {
    for(int lvl = 0; lvl <= list->level; lvl++) {
        slnode *node = list->first[0];
        while(node) {
            if(node->level >= lvl) printf("> %d --", node->key);
            else {
                printf("-----");
                for(int i = 0; i < intstrlen(node->key); i++) printf("-");
            }
            node = node->next[0];
        }
        printf(">#\n");
    }
}

static int intstrlen(int x) {
    int len = 1;

    if(x < 0) len++;
    x = abs(x);
    while(x > 10) {
        x /= 10;
        len++;
    }
    return len;
}

int sl_len(const sl *list) {
    int len = 0;
    slnode *node = list->first[0];
    while(node) {
        node = node->next[0];
        len++;
    }
    return len;
}

static int random_level(void) {
    int level = 0;
    while((rand() % 3) && level < SL_MAXLEVEL) {
        level++;
    }
    return level;
}

static slnode *slnode_create(int key, void *const value) {
    slnode *new = zmalloc(sizeof(slnode));
    int level = random_level();
    new->key = key;
    new->value = value;
    new->level = level;
    new->next = zmalloc((level+1)*sizeof(slnode**));
    return new;
}

static void slnode_destroy(slnode *node) {
    if(node) free(node->next);
    free(node->next);
}

static slnode ***slnode_locate(sl *const list, int key) {
    slnode **nodep = list->first;
    slnode ***update = zmalloc((list->level+1) * sizeof(slnode***));
    /* update holds an array of pointers to nodes (one per level)
    this is so we can go through and update the POINTER to point at the new node
    and access the NODE to point our new node at it. */
    for(int lvl = list->level; lvl >= 0; lvl--) {
        while(nodep[lvl] && nodep[lvl]->key < key) {
            nodep = nodep[lvl]->next;
        }
        update[lvl] = &(nodep[lvl]);
    }

    return update;
}
