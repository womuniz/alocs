#include "include/imds.h"

void invoke_class_createServer(jstring path);
void invoke_class_createBucket(jstring path, jint min, jint max);
void invoke_class_createDir(jstring path);
int invoke_class_getKeyLocation(jint key,const char **location);

jclass class_intJava;

/*prototipo: init_mds()
 * descricao: inicializa o servico de metadados
 * parametros: */
int init_mds(){
	
	jclass intJava;
	
	create_vm(&jvm,&env);
	
	//Localiza a classe InterfaceJava e cria uma referencia global
	intJava = (*env)->FindClass(env, "InterfaceJava");
	if(intJava != NULL){
	  class_intJava = (*env)->NewGlobalRef(env, intJava);
		//remove referencia local
		(*env)->DeleteLocalRef(env, localRefCls);
	}
	
	return 0;
}

/*prototipo: fin_mds()
 * descricao: finaliza o servico de metadados
 * parametros: */
int fin_mds(){
	
	//int rv = JNI_DestroyJavaVM(jvm);
	(*env)->DeleteGlobalRef(env, class_intJava);
	
	return 0;
}

void create_vm(JavaVM **jvm,JNIEnv **env){

    JavaVMInitArgs args;
    JavaVMOption options[1];
    args.version = JNI_VERSION_1_6;
    args.nOptions = 1;

    options[0].optionString = "-Djava.class.path=./:/home/ceph/testes/alocs/mds:/home/ceph/testes/alocs/mds/jar/zookeeper-3.4.6.jar:/home/ceph/testes/alocs/mds/jar/junit-4.11.jar:/home/ceph/testes/alocs/mds/jar/log4j-1.2.16.jar:/home/ceph/testes/alocs/mds/jar/slf4j-api-1.6.3.jar:/home/ceph/testes/alocs/mds/jar/slf4j-log4j12-1.7.0.jar";
    //options[2].optionString = "-verbose:jni";
   
    args.options = options;
    args.ignoreUnrecognized = 0;

    int rv = JNI_CreateJavaVM(jvm,(void **)env, &args);

}

void invoke_class_createServer(jstring path){
	
	//jclass java_class = (*env)->FindClass(env, "InterfaceJava");
	jmethodID java_method = (*env)->GetStaticMethodID(env,class_intJava,"createServer","(Ljava/lang/String;)V");	
	
	(*env)->CallStaticVoidMethod(env, java_class, java_method, path);
		
}

void invoke_class_createDir(jstring path){
    
	//jclass java_class = (*env)->FindClass(env, "InterfaceJava");
	jmethodID dir_method = (*env)->GetStaticMethodID(env,class_intJava, "createDirectory", "(Ljava/lang/String;)V");
	
	(*env)->CallStaticVoidMethod(env, java_class, dir_method, path);
}

void invoke_class_createBucket(jstring path, jint min, jint max){
    
	//jclass java_class = (*env)->FindClass(env, "InterfaceJava");
	jmethodID createBucket_method = (*env)->GetStaticMethodID(env, class_intJava, "createBucket", "(Ljava/lang/String;II)V");
	
	(*env)->CallStaticVoidMethod(env, java_class, createBucket_method, path, min, max);

}

int invoke_class_getKeyLocation(jint key,const char **location){
    
	//jclass java_class = (*env)->FindClass(env, "InterfaceJava");
	jmethodID fetchKey_method = (*env)->GetStaticMethodID(env, class_intJava, "fetchKey", "(I)Ljava/lang/String;");
	
	jstring result = (*env)->CallObjectMethod(env, class_intJava, fetchKey_method, key);
	if(result != NULL){
		*location = (*env)->GetStringUTFChars(env, result, NULL);
		
		return 0;
	}else //nao encontrou a localidade 
		return 1;

}

int put_server(char *srvName){
    
		jstring path;
		
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

int put_dir(char *srvName,char *dirName){
    
    char buffer[SRVNAME_SIZE+DIRNAME_SIZE+2];
    jstring path;
		
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

int get_key_location(jint key,const char **location){
    
    int ret;
		
		if(env == NULL){
			fprintf(stdout,"JNI nao inicializado\n");
			
			return 1;
		}
		
		ret = invoke_class_getKeyLocation(key,location);
		
		return ret;    
}



/*
void invoke_class_deleteBucket(JNIEnv* env, jstring path){
    jclass call_interfacejava_class = (*env)->FindClass(env, "javasrc/InterfaceJava");
    jm	ethodID deleteBucket_method = (*env)->GetStaticMethodID(env, call_interfacejava_class, "deleteBucket", "(Ljava/lang/String;)V");
    (*env)->CallStaticVoidMethod(env, call_interfacejava_class, deleteBucket_method, path);
}

void invoke_class_getMetadata(JNIEnv* env, jstring path){
    jclass call_interfacejava_class = (*env)->FindClass(env, "javasrc/InterfaceJava");
    jmethodID getMetadata_method = (*env)->GetStaticMethodID(env, call_interfacejava_class, "getMetadata", "(Ljava/lang/String;)Ljava/lang/String;");
    jstring result = (*env)->CallObjectMethod(env, call_interfacejava_class, getMetadata_method, path);
    const char* cresult= (*env)->GetStringUTFChars(env, result, 0);
    printf("Data -> %s\n" , cresult);
}

void invoke_class_listServers(JNIEnv* env){
    jclass call_interfacejava_class = (*env)->FindClass(env, "javasrc/InterfaceJava");
    jmethodID listServers_method = (*env)->GetStaticMethodID(env, call_interfacejava_class, "listServers", "()Ljava/lang/String;");
    jstring result = (*env)->CallObjectMethod(env, call_interfacejava_class, listServers_method);
    const char* cresult= (*env)->GetStringUTFChars(env, result, 0);
    printf("Servers -> %s\n" , cresult);
}

void invoke_class_listDirectories(JNIEnv* env, jstring path){
    jclass call_interfacejava_class = (*env)->FindClass(env, "javasrc/InterfaceJava");
    jmethodID listDirectories_method = (*env)->GetStaticMethodID(env, call_interfacejava_class, "listDirectories", "(Ljava/lang/String;)Ljava/lang/String;");
    jstring result = (*env)->CallObjectMethod(env, call_interfacejava_class, listDirectories_method, path);
    const char* cresult= (*env)->GetStringUTFChars(env, result, 0);
    printf("Directories -> %s\n" , cresult);
}

void invoke_class_listBuckets(JNIEnv* env, jstring path){
    jclass call_interfacejava_class = (*env)->FindClass(env, "javasrc/InterfaceJava");
    jmethodID listBuckets_method = (*env)->GetStaticMethodID(env, call_interfacejava_class, "listBuckets", "(Ljava/lang/String;)Ljava/lang/String;");
    jstring result = (*env)->CallObjectMethod(env, call_interfacejava_class, listBuckets_method, path);
    const char* cresult= (*env)->GetStringUTFChars(env, result, 0);
    printf("Buckets -> %s\n" , cresult);
}

void invoke_class_rebalancing(JNIEnv* env){
    jclass call_interfacejava_class = (*env)->FindClass(env, "javasrc/InterfaceJava");
    jmethodID rebalancing_method = (*env)->GetStaticMethodID(env, call_interfacejava_class, "rebalancing", " ()V");
    (*env)->CallObjectMethod(env, call_interfacejava_class, rebalancing_method);
}*/
