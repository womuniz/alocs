#include "storage_types.h"
#include "ceph_lib.h"

#ifndef ICEPH_H
#define ICEPH_H

//variavel que recebe o retorno das operacoes
int state;

/*
* Variaveis usadas por todas as funcoes
* Usadas para comunicacao com o ceph
*/
rados_t cluster;  // variavel tipo rados_t que manipula o cluster
rados_ioctx_t ioctx;  //variavel de contexto de io
rados_read_op_t read_op; //variavel que armazena o handle para operacoes de escrita


/*
***********************************
FUNCOES DE GERENCIAMENTO
***********************************
*/

/*prototipo: init_ssystem()
 * descricao: inicializa o sistema de armazenamento
 * parametros: */
int init_ssystem(void);

int fin_rados();/*prototipo: fin_ssystem()
 * descricao: finaliza o sistema de armazenamento
 * parametros: */
int fin_ssystem(void);

/*
***********************************
FUNCOES AUXILIARES
***********************************
*/

/*prototipo int read_config_file(unsigned char*)
 * descricao: faz a leitura do arquivo de configuração, e retorna o conteúdo do arquivos
 *            em buffer
 * parametros: buffer ->buffer que receberá o conteúdo do arquivo
 * retorno: inteiro indicativo de sucesso ou falha da operacao*/
int read_config_file(unsigned char** buffer);

/*prototipo int write_config_file()
 * descricao: cria o arquivo de configuração
 * parametros: int climit -> valor para CACHE_LIMIT
 * 						 int blimit -> valor para LIMIT_SIZE_BUCKET
 * 						 char cserver -> valor para CURRENT_SERVER
 * retorno: inteiro indicativo de sucesso ou falha da operacao*/
int write_config_file();

/**********************************
DEFINICAO OPERACOES INTERFACE S.A.
**********************************/

/*prototipo: int create_server(char *srvName)
 *objetivo: Cria o Servidor com o identificador especificado por parametro.*/
//extern int ss_create_server(char *srvName);

/*prototipo: int create_server(char *srvName)
 *objetivo: Cria o Servidor com o identificador especificado por parametro.*/
//extern int ss_drop_server(char *srvName);

/*prototipo: int create_bucket(char *dirName,char *srvName)
 *objetivo: Cria um Diretorio em Servidor especificado por parametro.*/
int ss_create_dir(char *dirName,char *srvName);

/*prototipo: int copy_dir(char *dirName,char *srvName1,char *srvName2)
 *objetivo: Copia um Diretorio de um Servidor para outro Servidor, ambos especificados por parametros.*/
int ss_copy_dir(char *dirName,char *srvName1,char *srvName2);

/*prototipo: int drop_dir(char *dirName,char *srvName)
 *objetivo: Remove um Diretorio de um Servidor, ambos especificados por parametros.*/
int ss_drop_dir(char *dirName,char *srvName);

/*prototipo: int create_bucket(char *srvName,char *dirName,char *idBucket)
 *objetivo: Cria um Bucket em Diretorio especificado nos parametros de entrada, em conjunto com o
 *					 identificador do Bucket, e Servidor.*/
int ss_create_bucket(char *srvName,char *dirName,char *idBucket,unsigned int maxKeys);

/*prototipo: int drop_bucket(char *idBucket,char *dirName,char *srvrName)
 *objetivo: Remove um Bucket, do Servidor e Diretório especificados por
 *parametros em conjunto com o identificador do Bucket*/
int ss_drop_bucket(LOCALITY_T *local);

/*prototipo: int getBucket(LOCALITY_T,TSTAMP_T,BUCKET_T*,int*)
 *objetivo: Retorna um Bucket, de um Diretorio e Servidor especificados nos parametros de entrada*/
int ss_get_bucket(LOCALITY_T *local,TSTAMP_T tstamp,BUCKET_T *bucket);

/*prototipo: int is_Empty(char *idBucket,char *dirName,char *srvName)
 *objetivo: Verifica se um Bucket, especificado por parametro, esta vazio, são indicados tambem o Diretorio e Servidor*/
int ss_is_empty_bucket(char *idBucket,char *dirName,char *srvName);

/*prototipo: int is_Empty(char *dirName,char *srvName)
 *objetivo: Verifica se um Diretório, especificado por parametro, está vazio, deve ser indicado tambem o Servidor*/
int ss_is_empty_dir(char *dirName,char *srvName);

/*prototipo: int (LOCALITY_T,TSTAMP_T,KEY_T,PAIR_T)
 *objetivo: Retorna o valor associado a chave key*/
int ss_get_key(LOCALITY_T *local,TSTAMP_T tstamp,KEY_T key,PAIR_T *pair);

/*prototipo: int put_pair(LOCALITY_T,TSTAMP_T,KEY_T,char*)
 *objetivo: adicionar um par chave-valor no bucket passado por parametro*/
int ss_put_data(LOCALITY_T *local,TSTAMP_T tstamp,KEY_T key,char *value);

/*prototipo: int ss_remove_pair(char*,char*,char*,TSTAMP_T,KEY_T)
 *objetivo: Remove um par chave-valor do Bucket correspondente ao identificador passado como parametro
 *de entrada, e necessario fornecer tambem o Servidor e Diretorio*/
int ss_remove_data(LOCALITY_T *local,TSTAMP_T tstamp,KEY_T key);

/*prototipo int write_data_disk(LOCALITY_T,TSTAMP_T,BUCKET_T,size_t)
 * descricao: escreve o buffer do bucket em disco, esta funcao deve ficar neste arquivo
 * 						pois a escrita do buffer em disco depende da interface do sa
 * parametros: srvName ->servidor em que o bucket esta armazenado
 *             dirName -> diretorio em que o bucket esta armazenado
 * 						 idBucket -> identificador do bucket
 * 						 buffer -> buffer que sera gravado
 * 						 len_buffer -> tamanho do buffer que sera gravado
 * retorno: inteiro indicativo de sucesso ou falha da operacao*/
int persist_bucket(LOCALITY_T *local,TSTAMP_T tstamp,BUCKET_T *buffer,size_t len_buffer);


#endif
