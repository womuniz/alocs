#include "cache_types.h"

#ifndef STORAGE_TYPES_H
#define STORAGE_TYPES_H

/*
* constantes para manipulacao do bucket
*/
#define LCKFLAG_OFFSET 0
#define MBITS_OFFSET 15
#define LOCK_FLAG_SIZE 1
#define SLOT_SIZE 18
#define OFF_SLOT_SIZE 4
#define LEN_SLOT_SIZE 4
#define KEY_SLOT_SIZE 10
#define TSTAMP_OFFSET 5

/*tamanho do header é definido por
  LOCK_FLAG(1 BYTE) + QTDCHAVES(4 BYTES) + TSTAMP(10 BYTES) + BITMAP(K) + SLOTS(k * SLOT_SIZE */
#define HEADER_SIZE(k) (LOCK_FLAG_SIZE + BYTES_LIMIT + TSTAMP_SIZE + k + (k * SLOT_SIZE) + 1)

//define o tipo KEY_T referente a chave
typedef uint64_t KEY_T;

//define o tipo BUCKET_T referente ao bucket
typedef unsigned char *BUCKET_T;

//define o tipo PAIR_T referente ao par chave valor
typedef struct PAIR_T{
  KEY_T key;
  char *value;
}PAIR_T;

typedef struct QUEUE_S{
  char *idBucket; //identificador do bucket que foi modificado
	LOCALITY_T local;  //local onde o bucket esta alocado servidor/diretorio/bucket
  TSTAMP_T sourcev; //versão do Bucket no Servidor de Origem
  TSTAMP_T cachev;  //versão do Bucket no Servidor Executor
  TSTAMP_T idTx;    //identificador da transação que escreveu os dados
  char *execServer; //identificador do Servidor Executor
	BUCKET_T *data; //bucket modificado
	struct QUEUE_S *next;
}QUEUE_T;

QUEUE_T *UPQUEUE;

int enq_data_cache(char *idBucket,LINE_T *line,char *execServer);

/*prototipo int extract_value(BUCKET_T *bucket,KEY_T key,PAIR_T *p_pair)
 * descricao: busca a chave passada por parametro no slots
 * parametros: p_buffer ->ponteiro para a posicao inicial
 * 												do mapa de bits no header
 *             numKeys -> qtd max de chaves no bucket, para limitar a busca no mapa de bits
 * 						 key -> chave requisitada
 * retorno: a posicao do slot no mapa de bits*/
int extract_value(BUCKET_T *bucket,KEY_T key,PAIR_T *pair);

/*prototipo void format_idBucket(char*,size_t,char*,char*)
 * descricao: formata o idbucket com o tstamp para gerar a versao do bucket
 * parametros: version string que recebe o resultado final
 *             size_t len_version tamanho da string que recebe o resultado final
 *             char *idBucket idbucket
 *             char *tstamp_c  tstamp*/
void format_idBucket(char *version,size_t len_version,char *idBucket,char *tstamp_c);

/*prototipo int write_pair(BUCKET_T,KEY_T,char*)
 * descricao: busca um slot livre no bucket
 * parametros: buffer ->buffer que sera modificado
 *             numKeys -> qtd max de chaves no bucket, para limitar a busca no mapa de bits
 * 						 key -> chave que sera escrita
 * 						 value -> valor que sera escrito
 * retorno: inteiro indicativo de sucesso ou falha da operacao*/
int write_pair(BUCKET_T *buffer,KEY_T key,char *value);

/*prototipo int rm_pair(BUCKET_T,KEY_T)
 * descricao: remove um par chave-valor do bucket no buffer
 * parametros: buffer ->buffer que sera modificado
 *             numKeys -> qtd max de chaves no bucket, para limitar a busca no mapa de bits
 * 						 key -> chave que sera escrita
 * retorno: inteiro indicativo de sucesso ou falha da operacao*/
int rm_pair(BUCKET_T *buffer,KEY_T key);

#endif
