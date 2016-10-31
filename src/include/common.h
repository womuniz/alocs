#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>
#include <string.h>
#include <jni.h>
#include "cache.h"
#include "util.h"

#ifndef COMMON_H
#define COMMON_H

/*
* constantes para manipulacao do bucket
*/
#define MBITS_OFFSET 5
#define LOCK_FLAG_SIZE 1
#define LCKFLAG_OFFSET 0
#define SLOT_SIZE 18
#define OFF_SLOT_SIZE 4
#define LEN_SLOT_SIZE 4
#define KEY_SLOT_SIZE 10
#define SRVNAME_SIZE 10
#define DIRNAME_SIZE 10
#define IDBUCKET_SIZE 10


/*tamanho do header é definido por 4 bytes iniciais que indicam a qtd de chaves do intervalo
 *+ bytes reservados para os slots + bytes para o mapa de bits
 *de bits que e equivalente a qtd de chaves + 1(null)*/
#define HEADER_SIZE(k) (LOCK_FLAG_SIZE + BYTES_LIMIT + k + (k * SLOT_SIZE) + 1)

//obtem a posicao do slot livre obtido a partir do bitmap
#define GET_SLOT_POS(p,k) ((p * SLOT_SIZE)+ k)

//posicao do offset no slot
#define OFF_POS_SLOT(s) (s)

//posicao do length no slot
#define LEN_POS_SLOT(s) (s+4)

//posicao de key no slot
#define KEY_POS_SLOT(s) (s+8)

//#define MBITS_OFFSET() (BYTES_LIMIT + LOCK_FLAG_SIZE)

/***********************************************
DEFINICAO FUNCOES NECESSARIAS PARA TODOS A API.
************************************************/

//define o tipo KEY_T referente a chave
typedef uint64_t KEY_T;

//define o tipo BUCKET_T referente ao bucket
typedef unsigned char *BUCKET_T;

//define o tipo PAIR_T referente ao par chave valor
typedef unsigned char *PAIR_T;

/*prototipo void set_offset_slot(unsigned char**,int)
 * descricao: grava o valor do offset no slot
 * parametros: src ->ponteiro para a posicao do offset no slot
 * 						 offset ->valor que sera gravado*/
void set_offset_slot(unsigned char **src,unsigned int offset);

/*prototipo int get_offset_slot(unsigned char*)
 * descricao: recupera o valor do offset no slot
 * parametros: src ->ponteiro para a posicao do offset no slot
 * retorno: um inteiro correspondente ao offset*/
unsigned int get_offset_slot(unsigned char *src);

/*prototipo void set_len_slot(unsigned char**,int)
 * descricao: grava o valor do length no slot
 * parametros: src ->ponteiro para a posicao do length no slot
 * 						 length ->valor que sera gravado*/
void set_len_slot(unsigned char **src,unsigned int length);

/*prototipo int get_len_slot(unsigned char*)
 * descricao: recupera o valor do length no slot
 * parametros: src ->ponteiro para a posicao do length no slot
 * retorno: um inteiro correspondente ao length*/
unsigned int get_len_slot(unsigned char *src);

/*prototipo void set_key_slot(unsigned char**,KEY_T)
 * descricao: grava o valor da chave no slot
 * parametros: src ->ponteiro para a posicao da chave no slot
 * 						 key ->chave que sera gravado*/
void set_key_slot(unsigned char **src,KEY_T key);

/*prototipo KEY_T get_key_slot(unsigned char*)
 * descricao: recupera o valor da chave no slot
 * parametros: src ->ponteiro para a posicao da chave no slot
 * retorno: um inteiro correspondente a chave*/
KEY_T get_key_slot(unsigned char *src);

void get_numkey_bucket(BUCKET_T *buff_bucket, unsigned char *numKeysChr);

/*prototipo void write_slot(unsigned char**,unsigned int,unsigned int,KEY_T)
 * descricao: atualiza o slot no header
 * parametros: buffer ->ponteiro para a posicao inicial do slot no buffer
 * 						 offset -> offset que sera gravado
 * 						 length -> length que sera gravado
 * 						 key -> valor da chave que sera gravada	
 * retorno: void*/
void write_slot(unsigned char **buffer,unsigned int slot_pos, unsigned int offset,unsigned int length,KEY_T key);

/*prototipo int find_slot_key(unsigned char**,int,KEY_T)
 * descricao: busca a chave passada por parametro no slots
 * parametros: p_buffer ->ponteiro para a posicao inicial 
 * 												do mapa de bits no header
 *             numKeys -> qtd max de chaves no bucket, para limitar a busca no mapa de bits
 * 						 key -> chave requisitada	
 * retorno: a posicao do slot no mapa de bits*/
int find_slot_key(unsigned char **buffer,int numKeys,KEY_T key);

/*prototipo int extractPair(BUCKET_T *buff_bucket,KEY_T key,PAIR_T *p_pair)
 * descricao: busca a chave passada por parametro no slots
 * parametros: p_buffer ->ponteiro para a posicao inicial 
 * 												do mapa de bits no header
 *             numKeys -> qtd max de chaves no bucket, para limitar a busca no mapa de bits
 * 						 key -> chave requisitada	
 * retorno: a posicao do slot no mapa de bits*/
int extract_pair(BUCKET_T *buff_bucket,KEY_T key,PAIR_T *p_pair);

/*prototipo int find_slot_free(unsigned char**,int,int,int*)
 * descricao: busca um slot livre no bucket 
 * parametros: p_buffer ->ponteiro para a posicao inicial 
 * 												do mapa de bits no header
 *             numKeys -> qtd max de chaves no bucket, para limitar a busca no mapa de bits
 * 						 len_value -> length no value necessário para verificar 
 * 													se o tamanho do valor e compativel com o slot
 * 						 offset -> retorna o offset para insercao do valor	
 * retorno: a posicao do slot no mapa de bits*/
int find_slot_free(unsigned char **buffer,int numKeys,int len_value,int *offset);

/*prototipo int write_pair_buffer(BUCKET_T *buffer,unsigned int numKeys,KEY_T key,char *value)
 * descricao: busca um slot livre no bucket 
 * parametros: buffer ->buffer que sera modificado
 *             numKeys -> qtd max de chaves no bucket, para limitar a busca no mapa de bits
 * 						 key -> chave que sera escrita
 * 						 value -> valor que sera escrito
 * retorno: inteiro indicativo de sucesso ou falha da operacao*/
int write_pair_buffer(BUCKET_T *buffer,unsigned int numKeys,KEY_T key,char *value);

/*prototipo int remove_pair_buffer(BUCKET_T *buffer,unsigned int numKeys,KEY_T key)
 * descricao: remove um par chave-valor do bucket no buffer 
 * parametros: buffer ->buffer que sera modificado
 *             numKeys -> qtd max de chaves no bucket, para limitar a busca no mapa de bits
 * 						 key -> chave que sera escrita
 * retorno: inteiro indicativo de sucesso ou falha da operacao*/
int remove_pair_buffer(BUCKET_T *buffer,unsigned int numKeys,KEY_T key);

#endif
