/*objetivo: modulo de metadados
  funcoes relacionadas ao modulo de metadados
  comunicação com o sistema de metadados*/
#include "include/metadata.h"

jstring path;

int put_server(char *srvName){

  if(env == NULL){
    fprintf(stdout,"JNI nao inicializado\n");
    return 1;
  }

  path = (*env)->NewStringUTF(env, srvName);
  if(path != NULL){
    fprintf(stdout,"server path -> %s!\n",srvName);
    invoke_class_createServer(path);
  }

  return 0;
}

int put_dir(char *dirName,char *srvName){

  char buffer[SRVNAME_SIZE+DIRNAME_SIZE+2];

  if(env == NULL){
    fprintf(stdout,"JNI nao inicializado\n");

    return 1;
  }

  snprintf(buffer, sizeof(buffer), "%s/%s",srvName,dirName);
  fprintf(stdout,"Directory path -> %s!\n",buffer);

  path = (*env)->NewStringUTF(env, buffer);
  invoke_class_createDir(path);

  return 0;
}

int put_bucket(char *srvName,char *dirName,char *idBucket,uint64_t iniKey,uint64_t finKey){

  char buffer[SRVNAME_SIZE+DIRNAME_SIZE+IDBUCKET_SIZE+3];
  jstring path;

  if(env == NULL){
    fprintf(stdout,"JNI nao inicializado\n");

    return 1;
  }

  snprintf(buffer,sizeof(buffer),"%s/%s/%s",srvName,dirName,idBucket);
  path = (*env)->NewStringUTF(env, buffer);
  invoke_class_createBucket(path, iniKey, finKey);

  return 0;
}

//int get_key_location(jint key,const char **location){
int get_key_location(jint key,LOCALITY_T *local){

  char buff[SRVNAME_SIZE+DIRNAME_SIZE+IDBUCKET_SIZE+3];
  const char *location;
  char *str1,*str2,*str3;
  int ret;

  if(env == NULL){
    fprintf(stdout,"JNI nao inicializado\n");
    return 1;
  }

  ret = invoke_class_getKeyLocation(key,&location);

  //faz o parse do conteudo de location
  strcpy(buff,location);
  str1 = buff;
  strtok_r(buff,"/",&str2);
  strtok_r(str2,"/",&str3);

  //faz a copia para as variaveis de retorno
  strcpy(local->srvName,str1);
  strcpy(local->dirName,str2);
  strcpy(local->idBucket,str3);

  return ret;
}
