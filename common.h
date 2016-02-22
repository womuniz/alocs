#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>
#include <string.h>

#ifndef COMMON_H
#define COMMON_H

/*
* constantes para manipulacao do bucket
*/
#define BASE 95
#define BYTES_LIMIT 4
#define SLOT_SIZE 18
#define OFF_SLOT_SIZE 4
#define LEN_SLOT_SIZE 4
#define KEY_SLOT_SIZE 10

//definicao do limite de tamanho do bucket
#define LIMIT_SIZE_BUCKET 64000

/*tamanho do header é definido pela qtd de chaves vezes o tamanho do slot + o tamanho do mapa
 *de bits que e equivalente a qtd de chaves + 1(null)*/
#define HEADER_SIZE(k) ((k * SLOT_SIZE) + (k + 1))

//obtem a posicao do slot livre obtido a partir do bitmap
#define GET_SLOT_POS(p,k) ((p * SLOT_SIZE)+ k)

//posicao do offset no slot
#define OFF_POS_SLOT(s) (s)

//posicao do length no slot
#define LEN_POS_SLOT(s) (s+4)

//posicao de key no slot
#define KEY_POS_SLOT(s) (s+8)

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

/*prototipo void set_slot(unsigned char**,unsigned int,unsigned int,KEY_T)
 * descricao: atualiza o slot no header
 * parametros: src ->ponteiro para a posicao inicial do slot
 * 						 offset -> valor do offset que sera gravado
 * 						 length -> valor do length que sera gravado
 * 						 key -> valor da chave que sera gravada	
 * retorno: ponteiro para a posicao inicial do slot atualizado*/
void set_slot(unsigned char **p_header,unsigned int slot_pos, unsigned int offset,unsigned int length,KEY_T key);

/*prototipo int find_slot_key(unsigned char**,int,KEY_T)
 * descricao: busca a chave passada por parametro no slots
 * parametros: p_buffer ->ponteiro para a posicao inicial 
 * 												do mapa de bits no header
 *             numKeys -> qtd max de chaves no bucket, para limitar a busca no mapa de bits
 * 						 key -> chave requisitada	
 * retorno: a posicao do slot no mapa de bits*/
int find_slot_key(unsigned char **p_buffer,int numKeys,KEY_T key);

/*prototipo int extractPair(BUCKET_T *buff_bucket,KEY_T key,PAIR_T *p_pair)
 * descricao: busca a chave passada por parametro no slots
 * parametros: p_buffer ->ponteiro para a posicao inicial 
 * 												do mapa de bits no header
 *             numKeys -> qtd max de chaves no bucket, para limitar a busca no mapa de bits
 * 						 key -> chave requisitada	
 * retorno: a posicao do slot no mapa de bits*/
int extractPair(BUCKET_T *buff_bucket,KEY_T key,PAIR_T *p_pair);

/*prototipo int find_slot_free(unsigned char**,int,int,int*)
 * descricao: busca um slot livre no bucket 
 * parametros: p_buffer ->ponteiro para a posicao inicial 
 * 												do mapa de bits no header
 *             numKeys -> qtd max de chaves no bucket, para limitar a busca no mapa de bits
 * 						 len_value -> length no value necessário para verificar 
 * 													se o tamanho do valor e compativel com o slot
 * 						 offset -> retorna o offset para insercao do valor	
 * retorno: a posicao do slot no mapa de bits*/
int find_slot_free(unsigned char **p_buffer,int numKeys,int len_value,int *offset);

/**********************************
DEFINICAO FUNCOES UTEIS.
**********************************/

/*prototipo void itoc(unsigned char *str,size_t length, int number)
 * descricao: converte um int para char
 * parametros: str ->ponteiro para a string com o retorno
 * 						 length -> sizeof de str	
 *             number -> inteiro a ser convertido
 * retorno: string contendo o inteiro*/
void itoc(unsigned char *str,size_t length, int number);

/*prototipo void itoc64(unsigned char *str,size_t length, uint64_t number)
 * descricao: converte um uint64_t para char
 * parametros: str ->ponteiro para a string com o retorno
 * 						 length -> sizeof de str
 *             number -> inteiro a ser convertido
 * retorno: string contendo o uint64_t*/
void itoc64(unsigned char *str,size_t length, uint64_t number);

/*prototipo void ctoi(unsigned char*,int)
 * descricao: converte um char para int ou u_int64
 * parametros: str ->ponteiro para a char com o numero
 *             int32 -> indica se o inteiro e 32 ou 64, se for 64 o valor e 0
 * retorno: int  contendo o inteiro*/
uint64_t ctoi(char *str,int int32);

/*prototipo void ntochr(char* str, int number)
 * descricao: converte um inteiro para um caracter de 4 bytes,funcao utilizada 
 * 						para guardar o length e o offset no slot limitando o tamanho a 4 bytes
 * parametros: str ->ponteiro para o caracter
 *             number ->inteiro  que ser convertido
 * retorno: str com o char de 4 bytes*/
void ntochr(char* str, int number);

/*prototipo int s4toi(unsigned char* str)
 * descricao: converte um um caracter de 4 bytes para um inteiro, funcao utilizada para 
 * 						guardar o length e o offset no slot limitando o tamanho a 4 bytes
 * parametros: str -> ponteiro para o caracter
 * retorno: um inteiro com o numero convertido*/
int chrton(char* str);

/*prototipo exp(int,int)
 * descricao: calculo da exponenciacao, para a conversao de char para inteiro,
 * 						utilizada na funcao s4toi
 * parametros: x -> base
 * 						 n -> expoente	
 * retorno: inteiro com o resultado do calculo*/
int get_exp(int x, int n);

/*prototipo void *xmalloc(size_t size)
 * descricao: aloca memoria de um determinado tamanho e retorna o ponteiro 
 * parametros: size -> tamanho da memoria a ser alocado
 * retorno: ponteiro para a memoria alocada*/
void *xmalloc(size_t size);

#endif
