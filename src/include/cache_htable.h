#include "cache_types.h"

#ifndef CACHE_HTABLE_H
#define CACHE_HTABLE_H

#define AVALUE ((sqrt(5) - 1) / 2)
#define HASH(k) ( HTSIZE * fmod((k * AVALUE),1) )

unsigned int HTSIZE;

/*prototipo: unsigned int get_hash(char* str)
 * descricao: atribui um hash para a localizacao do bucket no cache
 * parametros: str ->   idBucket do Bucket que esta no cache*/
unsigned int get_hash(char* str);

void create_htable();
int insert_node_ht(NODELOC_T *new_node);
int remove_node_ht(char *key);

//T_BUCKET *getFrame(int pos);

//tabela hash para localizacao dos buckets no cache
LOCATOR_T *LOCATOR;

//CACHE
LINE_T *CACHE;

#endif
