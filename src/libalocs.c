/*
* Arquivo: alocs.c
* Data: 26/02/2014
* Numero de Serie: 20140226002
*
************************************************************************
* Nota: Switch resolvido. Ate o presente momento, o put e get estao funcionado redondinho.
* O tratamento em relacao a quantidade de argumentos passados por parametro
* tambem esta ok. todos estao aceitando a quantia adequada e alertando sobre o mau uso.
* get() esta ok.
* Todas as opcoes estao funcionado corretamente.
* O elementar solicitado pela profa Carmem esta [OK]
*
* Versao: 0.4
* Descricao: Este arquivo faz a camada entre a aplicacao e o
* sistema de arquivos distribuido. Alem disso, conversa com o metadados.
************************************************************************
* Versao 2.0
*
* Notas: Nesta versao, ainda nao existe a integracao com o Metadados, o que depende dele, ou e
* passado como parametro ou entao esta fixado.
** O alocs chama as funcoes do ceph por meio da biblioteca iceph, ela e de fato quem
* conversa com o ceph. Seus retornos sao de 0.
**
*TODO: funcoes a implementar
*      clean() / replicate_dir() / dropALL_dir
*
*/
#include "include/libalocs.h"

int retOper;

//void aponta_dir(char *bucket,char *srvName,char *dirName);

/*
 *declarados no imds.h
 *JavaVM *jvm;
JNIEnv *env;*/

/********************************************
 *inicio funcoes de auxiliares
 ********************************************/
int parse_location(const char *location,char **srvName,char **dirName, char **idBucket){

  char buff[SRVNAME_SIZE+DIRNAME_SIZE+IDBUCKET_SIZE+3];
	
	strcpy(buff, location);
	srvName = buff;
	strtok_r(buff,"/",dirName);
	strtok_r(dirName,"/",idBucket);
	
	return 0;

}


/********************************************
 *inicio funcoes de gerenciamento
 ********************************************/
int init_api(){

	//inicializa sistema de armazenamento
	retOper = init_ssystem();
	if(retOper == 0) //inicializa o cache se o sistema de armazenamento iniciar
    retOper = init_cache();	
	
	//inicializa o sistema de metadados
	retOper = init_mds();
	
	return retOper;
}

int fin_api(){

	DIRTYBUFF_T *lst_dirty;
	char srv_name[SRVNAME_SIZE+1],dir_name[DIRNAME_SIZE+1],id_bucket[IDBUCKET_SIZE+1];
	BUCKET_T buffer;
	int state;
	
	state = 0;
	
	/*identifica os buffers sujos para atualizar
	  no sistema de armazenamento*/
	retOper = find_dirty_buffers();
	if(retOper > 0){
		lst_dirty = DIRTY_BUFFERS;
		
		while( (lst_dirty != NULL) && (state == 0) ){
			//resgata o buffer
			buffer = lst_dirty->buffer;
			//obtem a localizacao do buffer
			sscanf(lst_dirty->path,"%s %s %s",srv_name,dir_name,id_bucket);
			//escreve o buffer em disco
			state = write_buffer_disk(srv_name,dir_name,id_bucket,&buffer,strlen(buffer));
			
			lst_dirty = lst_dirty->next;
		}
	}
	
	if(state == 0){
		//finaliza o cache
		retOper = fin_cache();
		//finaliza o sistema de armazemento
		retOper = fin_ssystem();
		//finaliz o sistema de metadados
		retOper = fin_mds();
	}
	
	return retOper;
}

/********************************************
 *inicio operacoes interface da aplicacao
 ********************************************/

/*assinatura: int clean()
 *descricao: requisita a remoção de todos os Buckets que estiverem vazios*/

int clean(void){
	return 0;
}

/*assinatura: int create_server(char *srvName)
 *descricao: Requisita a criação do Servidor especificado nos parametros de entrada*/
int create_server(char *srvName){
	
	//chamda do ceph
	retOper = ss_create_server(srvName);
	
	//clock_t start = clock();
	
	//chamada do metadados
	retOper = put_server(srvName);
	
	/*clock_t stop = clock();
	double elapsed = (long double)(stop - start)* 1000.0 / CLOCKS_PER_SEC;
	printf("Time elapsed in ms: %f", elapsed);*/
	
	return retOper;
}

/*assinatura: int drop_server(char *srvName)
 *descricao: Requisita a remocao do Servidor especificado nos parametros de entrada*/
int drop_server(char *srvName){

	retOper = ss_drop_server(srvName);
	//Chamada do sistema de metadados para crear o server
	//invoke_class_deleteServer(env, srvName);

	return retOper;
}

/*assinatura: int create_dir(char *dirName,char *srvName)
 *descricao: Requisita a criação de um Bucket em Diretorio especificado nos parametros de entrada,
 *em conjunto com o identificador do Bucket, e o intervalo de chaves que sera armazenado*/
int create_dir(char *dirName,char *srvName){

	//clock_t start,stop;
	//double elapsed;
	
	// fazer a chamada na interface SA para criacao do diretorio
	retOper = ss_create_dir(dirName,srvName);
	
	//start = clock();

	// chamada de metadados para crear o diretorio
	retOper = put_dir(srvName,dirName);

	/*stop = clock();
	elapsed = (long double)(stop - start)* 1000.0 / CLOCKS_PER_SEC;
	fprintf(stdout,"Time elapsed in ms: %f", elapsed);*/
	
	return retOper;
}

/*assinatura: int drop_dir(char *dirName,char *srvName)
 *descricao: Requisita a remocao de um Diretorio, especificado nos parametros de entrada,
 *em conjunto com o Servidor em que ele esta alocado*/
int drop_dir(char *dirName,char *srvName){

	/*sequencia de operacoes
	 *1)remover diretorio no SA
	 *2)atualizar metadados*/

	//d) drop dir
	retOper = ss_drop_dir(dirName,srvName);

	/*2) atualizar metadados
	if(retOper == 0)
		retOper = drop_dir(dirName,srvName);

	*/

	return retOper;
}

/*assinatura: int replicate_dir(char *dirName,char *srvName)
 *descricao: Requisita a replicacao de um Diretorio, especificado como parametro de entrada,
 *em conjunto com o Servidor de destino.*/
int replicate_dir(char *dirName,char *srvName){

	/*sequencia de operacoes
	 *1)solicitar ao metadados a localidade do servidor
	 *2)fazer a chamada na interface SA para copia do diretorio
	 *3)atualizar os metadados*/

	/*1) solicitar ao metadados a localidade do servidor
	locality = get(dirName);*/

	/*2) fazer a chamada na interface SA para criacao do diretorio
	retOper = copy_dir(dirName,locality->srvName,srvName);*/

	/*3) atualizar metadados
	retOper = put_dir(dirName,srvName);
	*/
	return 0;
}

/*assinatura: int drop_ALLdir(char *dirName)
 *descricao: Requisita a remocao de um Diretorio, e suas replicas, especificado como
 *parametro de entrada.*/
int dropALL_dir(char *dirName){
	return 0;
}

/*assinatura: int create_bucket(char *dirName,char *idBucket,double inikey,double finkey)
 *descricao: Requisita a criação de um Bucket em Diretorio especificado nos parametros de entrada,
 *em conjunto com o identificador do Bucket, e o intervalo de chaves que sera armazenado*/
int create_bucket(char *srvName,char *dirName,char *idBucket,uint64_t iniKey,uint64_t finKey){

	/* No futuro, verificar se o create_bucket avisa se
	* um bucket com mesmo nome ja existe, hoje,
	* simplesmente sobrescreve
	*/
	int retOper = 1;
	unsigned int maxKeys;
	
	/*chamada da operacao create_bucket da interface S.A.
	 *srvName deve ser buscado no metadados*/
	maxKeys = (finKey - iniKey)+1;

	//start = clock();
	retOper = ss_create_bucket(srvName,dirName,idBucket,maxKeys);

	/*atualizar metadados*/
	//invoke_class_createBucket(env, pathj, iniKey, finKey);
	put_bucket(srvName,dirName,idBucket,iniKey,finKey);
	
	/*stop = clock();
	double elapsed = (long double)(stop - start)* 1000.0 / CLOCKS_PER_SEC;
	fprintf(stdout,"Time elapsed in ms: %f", elapsed);*/
	
	return retOper;
}

/*assinatura: int drop_bucket(char *idBucket)
 *descricao: Requisita a remocao de um Bucket especificado nos parametros de entrada.*/
int drop_bucket(char *idBucket){

	char dirName[10],srvName[10];

	//temporario enquanto nao liga com mds
	//aponta_dir(idBucket,srvName,dirName);

	/*sequencia de operacoes
	 *1)obter localizacao do bucket
	 *2)remover bucket no SA
	 *3)atualizar metadados*/

	/*1) obter localizacao do bucket
	locality deve ser um struct para colocar na sequencia servidor/diretorio
	get(idBucket) deve retornar uma lista caso o diretorio seja replicado
	locality = get(idBucket);*/

	//2) remover bucket no SA
	retOper = ss_drop_bucket(idBucket, dirName,srvName);

	/*3) atualizar metadados
	if(retOper == 0)
		retOper = drop_bucket(idBucket,dirName);*/

	return retOper;
}

/*assinatura: int get_pair(KEY_T key)
 *descricao: Requisita um par chave-valor identificado por uma chave especificada como parametro de entrada.
 *O Bucket e identificado pelo sistema de Metadados baseado no intervalo de chaves*/
int get_pair(KEY_T key,PAIR_T *pair){

	char *srvName,*dirName,*idBucket;
	//clock_t start,stop;
	const char *location;
	double elapsed;
	
	/*sequencia de operacoes
	 *1)obter localizacao da chave
	 *2)solicitar ao SA o bucket
	 *3)extrair o par chave_valor*/

	//start = clock();

	/*1) obter localizacao do bucket*/ 
	retOper = get_key_location(key,&location);
	
	/*strcpy(buff, location);
	srvName=buff;
	strtok_r(buff,"/",&dirName);
	strtok_r(dirName,"/",&idBucket);*/
	if((retOper == 0) && (location != NULL)){ //executou sem erros
		retOper = parse_location(location,&srvName,&dirName,&idBucket);
		fprintf(stdout,"Server: '%s'\n Directory: '%s'\n Bucket: '%s'\n", srvName, dirName, idBucket);
		
		/*invocar a funcao get_key do mod. arm.(iceph) que faz a busca do bucket
		  e extracao do par*/
		retOper = ss_get_key(srvName,dirName,idBucket,key,pair);
		
	}else{
		fprintf(stderr,"Localidade da chave não encontrada!\n");
		
		return 1;
	}
	
	/*stop = clock();
	elapsed = (long double)(stop - start)* 1000.0 / CLOCKS_PER_SEC;
	fprintf(stdout,"Time elapsed in ms: %f", elapsed);*/

	return retOper;

}

/*assinatura: int put_pair(KEY_T key,char *value)
 *descricao: Requisita a adicao de um par chave-valor especificado nos parametro de entrada.
 *O Bucket e identificado pelo sistema de Metadados baseado no intervalo de chaves*/
int put_pair(KEY_T key,char *value){

	char *srvName,*dirName,*idBucket;
	const char *location;
	
	//clock_t start,stop;
	
	/*sequencia de operacoes
	 *1)obter localizacao da chave
	 *2)solicitar ao SA a adicao da chave
	 *3)retornar para a aplicacao*/

	//clock_t start = clock();
	
	/*1) obter localizacao da chave*/
	retOper = get_key_location(key,&location);
	
	if((retOper == 0) && (location != NULL)){ //executou sem erros
		retOper = parse_location(location,&srvName,&dirName,&idBucket);
		fprintf(stdout,"Server: '%s'\n Directory: '%s'\n Bucket: '%s'\n", srvName, dirName, idBucket);
		
		//2) solicitar ao SA a adicao da chave
		retOper = ss_put_pair(srvName, dirName, idBucket, key, value);
	
		/*clock_t stop = clock();
		double elapsed = (long double)(stop - start)* 1000.0 / CLOCKS_PER_SEC;
		printf("Time elapsed in ms: %f", elapsed);	*/
	}else{
		fprintf(stderr,"Localidade da chave não encontrada!\n");
		
		return 1;
	}
	
	return retOper;
}

/*assinatura: int rem_pair(KEY_T key)
 *descricao: Requisita a remoção de um par chave-valor identificado por uma chave especificada como parametro
 *de entrada. O Bucket é identificado pelo sistema de Metadados baseado no intervalo de chaves*/
int rem_pair(KEY_T key){

	char *srvName,*dirName,*idBucket;
	const char *location;
	
	/*sequencia de operacoes
	 *1)obter localizacao da chave
	 *2)solicitar ao SA a remocao da chave
	 *3)retornar para a aplicacao*/

	/*1) obter localizacao da chave*/
	retOper = get_key_location(key,&location);
	
	if((retOper == 0) && (location != NULL)){ //executou sem erros
		retOper = parse_location(location,&srvName,&dirName,&idBucket);
		fprintf(stdout,"Server: '%s'\n Directory: '%s'\n Bucket: '%s'\n", srvName, dirName, idBucket);
	
		//2) solicitar ao SA a remocao do par
		retOper = ss_remove_pair(srvName,dirName,idBucket,key);
	}else{
		fprintf(stderr,"Localidade da chave não encontrada!\n");
		
		return 1;
	}  
	
	return retOper;
}