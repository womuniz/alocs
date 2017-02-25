#include "alocs_conf.h"
#include <jni.h>

#ifndef IMDS_H
#define IMDS_H

//variavel que guarda info da jvm para o mds
JavaVM *jvm;
JNIEnv *env;

void invoke_class_createServer(jstring path);
void invoke_class_createBucket(jstring path, jint min, jint max);
void invoke_class_createDir(jstring path);
int invoke_class_getKeyLocation(jint key,const char **location);

/*
void invoke_class_deleteBucket(JNIEnv* env, jstring path);
void invoke_class_getMetadata(JNIEnv* env, jstring path);
void invoke_class_listServers(JNIEnv* env);
void invoke_class_listDirectories(JNIEnv* env, jstring path);
void invoke_class_listBuckets(JNIEnv* env, jstring path);
void invoke_class_rebalancing(JNIEnv* env);*/
#endif
