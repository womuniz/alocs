#include "cache_types.h"

#ifndef CACHE_HEAP_H
#define CACHE_HEAP_H


#define HPAR(i) ((i - 1) / 2)
#define HLEFT(i,hs) ((2 * i + 1)>=hs?i:(2 * i + 1))
#define HRIG(i,hs) ((2 * i + 2)>=hs?i:(2 * i + 2))

//heap para o lru
HEAPLRU_T *HLRU;

void update_tstamp_lru(int indx,unsigned int tstamp);
void create_heap();
HEAPLRU_T *insert_heap_lru(char* key,unsigned int tstamp,LINE_T **line);
int get_least_used(HEAPLRU_T **it_heap);
//void print_heap();

#endif
