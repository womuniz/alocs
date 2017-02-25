#include "alocs_conf.h"
#include "storage_types.h"
#include "iceph.h"

#ifndef STORAGE_H
#define STORAGE_H

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
DEFINICAO FUNCOES DE GERENCIAMENTO
************************************************/

/*prototipo: init_ssystem()
 * descricao: inicializa o sistema de armazenamento
 * parametros: */
int init_ssystem(void);

int fin_rados();/*prototipo: fin_ssystem()
 * descricao: finaliza o sistema de armazenamento
 * parametros: */
int fin_ssystem(void);


/*********************************************************
DEFINICAO FUNCOES QUE SÃO INDEPENDENTES DE INTERFACE
**********************************************************/

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

int get_numkeys_bucket(BUCKET_T *bucket);

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


#endif
