#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <inttypes.h>
#include "hashmap.h"

#ifndef ALOCS_CONF_H
#define ALOCS_CONF_H

//gerenciamento do cache
#define SRVNAME_SIZE 10
#define DIRNAME_SIZE 10
#define IDBUCKET_SIZE 20

//arquivo de configuracoes
#define CLIMIT_SLOT_SIZE 5
#define BLIMIT_SLOT_SIZE 5
#define CSERVER_SLOT_SIZE 10

//nomes do diretorio e arq. de conf.
#define DIR_CONF "SYSALOCS"
#define FILE_CONF "ALOCSCONF"  

//definição do tamanho do timestamp no alocs
#define TSTAMP_SIZE 10

//define o tipo TSTAMP_T referente aos timestamps utilizados no alocs
typedef uint64_t TSTAMP_T;

//define o tipo LOCALITY_T referente a localização do Bucket
typedef struct LOCALITY_S{
  char srvName[SRVNAME_SIZE+1];
  char dirName[DIRNAME_SIZE+1];
  char idBucket[IDBUCKET_SIZE+1];
}LOCALITY_T;

//CURRENT SERVER
char CURRENT_SERVER[CSERVER_SLOT_SIZE];
//TAMANHO BUCKET
unsigned int LIMIT_SIZE_BUCKET;
//CAPACIDADE CACHE
unsigned int CACHE_LIMIT;


#endif
