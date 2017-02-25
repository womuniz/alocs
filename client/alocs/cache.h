#ifndef CACHE_H
#define CACHE_H

#include "alocs_conf.h"
#include "storage_types.h"
#include "cache_htable.h"
#include "cache_heap.h"

//tabela hash para localizacao dos buckets no BUFFER POOL
LOCATOR_T *LOCATOR;

//BUFFER POOL
LINE_T *CACHE;

//lista das linhas que foram alterados nas operacoes de escrita
DIRTYLINES_T *DIRTY_LINES;


/*********************************************************
DEFINICAO FUNCOES DO CACHE
**********************************************************/

/*prototipo: int rm_data_cache(LINE_T*)
 * descricao: remove do cache o bucket identificado por get_least_used()
 * parametros: HEAPLRU_T* -> ponteiro para o item do heap
 * retorno: um inteiro indicativo de falha ou sucesso na remocao*/
int rm_data_cache(LINE_T *line);

#endif
