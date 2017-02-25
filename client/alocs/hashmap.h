#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <inttypes.h>
#include "util.h"

#ifndef HASHMAP_H
#define HASHMAP_H

#define COUNT_ITMAP(s,l) ( (s * l) / s )
#define SIZE_ITMAP(s) (s)

typedef struct data_s{
    const char *key;
    int value;
}data_t;

int data_cmp_char(void const *lhs, void const *rhs);
int data_cmp_int(void const *lhs, void const *rhs);
int create_map(data_t **map,size_t size);
int add_item_int(data_t *map,const char *key, int value,int limit);
//int add_item_char(data_t **map,const char key, const char *value,int limit);
const data_t *get_item_int(data_t *search,data_t *map,int limit);
const data_t *get_item_char(data_t *search,data_t *map,int limit);

#endif
