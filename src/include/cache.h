#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "util.h"

#ifndef CACHE_H
#define CACHE_H

#define AVALUE ((sqrt(5) - 1) / 2)
#define HASH(k) ( HT_SIZE * fmod((k * AVALUE),1) )
#define BUFF_SLOT(i) ( (i * LIMIT_SIZE_BUCKET) + 0 )
#define KEY_SIZE 10
#define LCKOWNER_SIZE 10

//ajustar conforme o ambiente que esta sendo utilizado
#define BUFFER_LIMIT 600
//TODO: VER UM VALOR MAIS ADEQUADO PARA O TAMANHO DO MANAGER
#define HT_SIZE 490

typedef struct DIRTYBUFF_S{
		char path[PATH_SIZE+1];
		unsigned char *buffer;
		struct DIRTYBUFF_S *next;
}DIRTYBUFF_T;

typedef struct FRAME_S{
		char path[PATH_SIZE+1];
		int pin_count;
		int dirty;
		int locked;
		char lock_owner[LCKOWNER_SIZE];
		unsigned char *buffer;
}FRAME_T;

typedef struct NODELOC_S{
	char key[KEY_SIZE+1];
	FRAME_T *frame;
	struct NODELOC_S *next;
}NODELOC_T;

typedef struct LOCATOR_S{
	NODELOC_T *node;
}LOCATOR_T;

//tabela hash para localizacao dos buckets no BUFFER POOL
LOCATOR_T *LOCATOR;

//BUFFER POOL
FRAME_T *BUFFER_POOL;

//lista dos buffers que foram alterados nas operacoes de escrita
DIRTYBUFF_T *DIRTY_BUFFERS;

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
int put_buffer(char* key,char *path,unsigned char** buffer);

/*prototipo: int get_buffer(char* key, unsigned char* buffer)
 * descricao: localiza o cache referente ao bucket identificado por parametro 
 * parametros: key -> id do Bucket que que sera pesquisado
 * 						 buffer -> aponta para o buffer do bucket no cache	
 * retorno: um inteiro indicativo de falha ou sucesso na busca pelo bucket*/
int get_buffer(char* key,unsigned char** buffer);

/*prototipo: int remove_buffer(char* key)
 * descricao: remove do cache o bucket identificado pela chave passada por parametro 
 * parametros: key ->   idBucket do Bucket que esta no cache
 * retorno: um inteiro indicativo de falha ou sucesso na obtencao do cache*/
int remove_buffer(char* key);

/*prototipo: unsigned int get_hash(char* str)
 * descricao: atribui um hash para a localizacao do bucket no cache 
 * parametros: str ->   idBucket do Bucket que esta no cache
 * retorno: o valor de hash para o Bucket*/
unsigned int get_hash(char* str);

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

#endif 
