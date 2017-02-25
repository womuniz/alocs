#include "include/imds.h"

jclass class_intJava;

void create_vm(JavaVM **jvm,JNIEnv **env);

void create_vm(JavaVM **jvm,JNIEnv **env){

    JavaVMInitArgs args;
    JavaVMOption options[1];
    args.version = JNI_VERSION_1_6;
    args.nOptions = 1;

    options[0].optionString = "-Djava.class.path=./:/home/ceph/app-alocs/mds:/home/ceph/app-alocs/mds/jar/zookeeper-3.4.9.jar:/home/ceph/app-alocs/mds/jar/junit-4.11.jar:/home/ceph/app-alocs/mds/jar/log4j-1.2.16.jar:/home/ceph/app-alocs/mds/jar/slf4j-api-1.6.3.jar:/home/ceph/app-alocs/mds/jar/slf4j-log4j12-1.7.0.jar";
    //options[2].optionString = "-verbose:jni";
   
    args.options = options;
    args.ignoreUnrecognized = 0;

    int rv = JNI_CreateJavaVM(jvm,(void **)env, &args);

}

/*prototipo: init_mds()
 * descricao: inicializa o servico de metadados
 * parametros: */
int init_mds(){
	
	jclass intJava;
	
	create_vm(&jvm,&env);
	
	//Localiza a classe InterfaceJava e cria uma referencia global
	//referencia local
	intJava = (*env)->FindClass(env, "InterfaceJava");
	if(intJava != NULL){
	  //referencia global
		class_intJava = (*env)->NewGlobalRef(env, intJava);
		//remove referencia local
		(*env)->DeleteLocalRef(env, intJava);
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

void invoke_class_createServer(jstring path){
	
	//jclass java_class = (*env)->FindClass(env, "InterfaceJava");
	jmethodID java_method = (*env)->GetStaticMethodID(env,class_intJava,"createServer","(Ljava/lang/String;)V");	
	
	(*env)->CallStaticVoidMethod(env,class_intJava,java_method,path);
		
}

void invoke_class_createDir(jstring path){
    
	//jclass java_class = (*env)->FindClass(env, "InterfaceJava");
	jmethodID dir_method = (*env)->GetStaticMethodID(env,class_intJava, "createDirectory", "(Ljava/lang/String;)V");
	
	(*env)->CallStaticVoidMethod(env,class_intJava,dir_method,path);
}

void invoke_class_createBucket(jstring path, jint min, jint max){
    
	//jclass java_class = (*env)->FindClass(env, "InterfaceJava");
	jmethodID createBucket_method = (*env)->GetStaticMethodID(env, class_intJava, "createBucket", "(Ljava/lang/String;II)V");
	
	(*env)->CallStaticVoidMethod(env,class_intJava,createBucket_method, path, min, max);

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
