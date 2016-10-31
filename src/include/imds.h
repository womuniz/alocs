#include <stdio.h>
#include <stdlib.h>
#include <jni.h>
#include "common.h"

#ifndef IMDS_H
#define IMDS_H

//variavel que guarda info da jvm para o mds
JavaVM *jvm;
JNIEnv *env;

/*prototipo: init_mds()
 * descricao: inicializa o servico de metadados
 * parametros: */
int init_mds();

/*prototipo: fin_mds()
 * descricao: finaliza o servico de metadados
 * parametros: */
int fin_mds();

void create_vm(JavaVM **jvm,JNIEnv **env);

int put_server(char *srvName);
int put_dir(char *srvName,char *dirName);
int put_bucket(char *srvName,char *dirName,char *idBucket,uint64_t iniKey,uint64_t finKey);
int get_key_location(jint key,const char **location);


/*
void invoke_class_deleteBucket(JNIEnv* env, jstring path);
void invoke_class_getMetadata(JNIEnv* env, jstring path);
void invoke_class_listServers(JNIEnv* env);
void invoke_class_listDirectories(JNIEnv* env, jstring path);
void invoke_class_listBuckets(JNIEnv* env, jstring path);
void invoke_class_rebalancing(JNIEnv* env);*/
#endif
