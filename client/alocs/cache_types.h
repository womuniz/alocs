#include <math.h>
#include "alocs_conf.h"

#ifndef CACHE_TYPES_H
#define CACHE_TYPES_H


typedef struct DIRTYLINES_S{
    LOCALITY_T local;
    unsigned char *data;
    struct DIRTYLINES_S *next;
}DIRTYLINES_T;

//tipo de dado dos frames que formam o cache
typedef struct LINE_S{
  char key[IDBUCKET_SIZE+1];
  LOCALITY_T local; //posteriormente mudar para o tipo registro LOCAL_T
  TSTAMP_T sourcev;
  TSTAMP_T cachev;
  int pin_count;
  int dirty;
  int locked;
  unsigned char *data;
}LINE_T;

//tipo de dados do heap para o lru
typedef struct HEAPLRU_S{
  int indx;
  char key[IDBUCKET_SIZE+1];
  unsigned int tstamp;
  LINE_T *line;
}HEAPLRU_T;

//tipo de dados da tabela hash
typedef struct NODELOC_S{
  char key[IDBUCKET_SIZE+1];
  HEAPLRU_T *it_heap;
  struct NODELOC_S *next;
}NODELOC_T;

typedef struct LOCATOR_S{
  NODELOC_T *node;
}LOCATOR_T;

/*prototipo int init_cache()
 * descricao: inicializa o cache no ALOCS, esta funcao e chamada na funcao init no common.c
 * parametros:
 * retorno: um inteiro indicativo de falha ou sucesso*/
int init_cache();

/*prototipo int fin_cache()
 * descricao: libera a memoria utilizada pelo cache ao finalizar o ALOCS
 * parametros:
 * retorno: um inteiro indicativo de falha ou sucesso*/
int fin_cache();

/*prototipo: int put_buffer(char* key,char *path,unsigned char** buffer)
 * descricao: adiciona um bucket no cache retornando um ponteiro para o buffer designado
 * parametros: key ->   idBucket do Bucket que esta sendo adicionado ao cache
 * 						 path ->  path do bucket no metadados por enquanto nao esta sendo utilizado
 * 						 buffer-> espaco no cache designado para o bucket
 * retorno: um inteiro indicativo de falha ou sucesso na obtencao do cache*/
int put_data_cache(char* key,LOCALITY_T *local,unsigned char** buffer);

/*prototipo: int get_buffer(char* key, unsigned char* buffer)
 * descricao: localiza o cache referente ao bucket identificado por parametro
 * parametros: key -> id do Bucket que que sera pesquisado
 * 						 buffer -> aponta para o buffer do bucket no cache
 * retorno: um inteiro indicativo de falha ou sucesso na busca pelo bucket*/
int get_data_cache(char* key,unsigned char** buffer);

/*prototipo: int set_lock(char*,int)
 * descricao: altera o valor do parametro locked no buffer_pool
 * parametros: key -> chave para encontrar o buffer
 * 						 lock_owner -> srvName que fica como o lock_owner
 * 						 lock -> valor que sera atribuido ao parametro
 * retorno: um inteiro indicativo de falha ou sucesso da operacao*/
int set_locked(char *key,char *lock_owner, int locked);

/*prototipo: int get_locked(char *key,char *lock_owner)
 * descricao: busca o valor do parametro locked no buffer_pool e compara o
 * 						lock_owner passado por parametro com o registrado no buffer manager
 * parametros: key -> chave para encontrar o buffer
 * 						 lock_owner -> srvName que fica como o lock_owner
 * retorno: um inteiro indicativo da obtencao do lock pelo lock_owner*/
int check_locked(char *key,char *requester, int *locked, char* lock_owner);

/*prototipo: int find_dirty_buffers()
 * descricao: percorre o buffer pool em busca de buffers alterados (sujos)
 * 						os buffer sujos sao apontados na lista DIRTY_BUFFERS que e acessada
 * 						pela interface do sa
 * parametros:
 * retorno: qtd de buffers alterados */
int find_dirty_buffers();

/*prototipo: int set_dirty(char*,int)
 * descricao: altera o valor do parametro dirty no buffer_pool
 * parametros: key -> chave para encontrar o buffer
 * 						 dirty -> valor que sera atribuido ao parametro
 * retorno: um inteiro indicativo de falha ou sucesso da operacao*/
int set_dirty(char *key, int dirty);

NODELOC_T *search_node_ht(char *key);

#endif
