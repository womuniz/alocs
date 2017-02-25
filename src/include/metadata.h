#include "alocs_conf.h"
#include "imds.h"

#ifndef METADATA_H
#define METADATA_H


/*prototipo: init_mds()
 * descricao: inicializa o servico de metadados
 * parametros: */
int init_mds();

/*prototipo: fin_mds()
 * descricao: finaliza o servico de metadados
 * parametros: */
int fin_mds();


int put_server(char *srvName);
int put_dir(char *dirName,char *srvName);
int put_bucket(char *srvName,char *dirName,char *idBucket,uint64_t iniKey,uint64_t finKey);
int get_key_location(jint key,LOCALITY_T *local);

/*
void invoke_class_deleteBucket(JNIEnv* env, jstring path);
void invoke_class_getMetadata(JNIEnv* env, jstring path);
void invoke_class_listServers(JNIEnv* env);
void invoke_class_listDirectories(JNIEnv* env, jstring path);
void invoke_class_listBuckets(JNIEnv* env, jstring path);
void invoke_class_rebalancing(JNIEnv* env);*/

#endif
