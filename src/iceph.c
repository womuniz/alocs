/*
* Arquivo: iceph.c
* Data: 26/02/2014
* Versao: 1.0
* Descricao: Este arquivo e a camada que conversa com o ceph
* 
*
* to_do: a funcao create_bucket ainda nao esta aceitando a criacao de bucket com um crush_rule
* por enquanto usa a default, o importante eh que funciona. (MUDAR PARA DIRETORIO)
*
* TODO:
* 	implementar as operacoes que faltam (is_Empty(),copy_dir())
*/
#include "include/iceph.h"

/*
***********************************************
* Prototipos funcoes privadas 
***********************************************
*/

/*prototipo: int get_lock(char *srvName,char *dirName,char *idBucket,BUCKET_T *buffer)
 *objetivo: verificar se o servidor que ira executar operacao de escrita adquiriu o lock ou nao*/
int get_lock(char *srvName,char *dirName,char *idBucket,BUCKET_T *buffer);

int unlock_buffer(char *srvName,char *dirName,char *idBucket,BUCKET_T *buffer);

/*
***********************************************
* Funcoes de gerenciamento 
***********************************************
*/

/*
* Funcao que inicia o rados(), conecta no cluster
* arquivo de configuracao: /etc/ceph/ceph.conf
*/
int init_rados() {
	
	// cria o manipulador do cluster
	create_handle(&cluster);
	if(cluster == NULL){
		fprintf(stderr,"[init_rados/iceph.c] cannot create a cluster handle\n");
		
		return 1;
	}
	
	return 0;
}

/*
* Funcao que termina a conexao com o rados
* 
*/
int fin_rados(){
	
	cluster_shutdown(&cluster);  // desconecta do cluster
	
	return 0;
}

/**********************************
DEFINICAO OPERACOES INTERFACE S.A.
**********************************/

/*prototipo: int create_server(char *srvName)
 *objetivo: Cria o Servidor com o identificador especificado por parametro.*/
int ss_create_server(char *srvName){
	
	//definir identificador da regra 
	uint8_t crush_ruleset = 0;
	
	state = create_pool(cluster,srvName,crush_ruleset);
	
	return ((state >= 0) ? 0 : 1);
}

/*prototipo: int create_server(char *srvName)
 *objetivo: Cria o Servidor com o identificador especificado por parametro.*/
int ss_drop_server(char *srvName){
	
	state = remove_pool(cluster,srvName);
	
	return ((state >= 0) ? 0 : 1);
}

/*prototipo: int create_dir(char *dirName,char *srvName)
 *objetivo: Cria um Diretorio em Servidor especificado por parametro.*/
int ss_create_dir(char *dirName,char *srvName){
	
	char* dir_content;
	size_t len_cont;
	
	dir_content = NULL;
	
	//aloca memoria para escrever o conteudo do arquivo
	len_cont = (sizeof(char) * strlen(dirName))+1;
	dir_content = (char *) xmalloc(len_cont);
	
	//o conteudo do arquivo por enquanto e o nome do diretorio 
	strcpy(dir_content,dirName);
	
	// conecta no pool/servidor
	state = set_server(cluster,srvName,&ioctx);
	if(state >= 0)//grava o arquivo sinalizador do diretorio no servidor
		state = write_object_full(ioctx, dirName, dir_content, sizeof(dir_content));
	else
		fprintf(stderr,"[create_dir/iceph.c] Servidor não localizado!\n");
	
	//encerra o contexto de io
	destroy_ioctx(&ioctx);
	
	free(dir_content);
	
	return ((state >= 0) ? 0 : 1);
}

/*prototipo: int copy_dir(char *dirName,char *srvName1,char *srvName2)
 *objetivo: Copia um Diretorio de um Servidor para outro Servidor, ambos especificados por parametros.*/
int ss_copy_dir(char *dirName,char *srvName1,char *srvName2){
	
	/*devemos pensar como esta operacao sera realizada pois o
	 *CEPH possui o conceito de copia de Pool mas o novo diretorio
	 *nao permanece com o mesmo nome*/
	return 0;
}

/*prototipo: int drop_dir(char *dirName,char *srvName)
 *objetivo: Remove um Diretorio de um Servidor, ambos especificados por parametros.*/
int ss_drop_dir(char *dirName,char *srvName){
	
	/*TODO: drop dir sera mais complexo terei que criar um contexto de io no diretorio 
	 *e solicitar uma lista de arquivos
	 *1) consultar no metados os buckets do diretorio (esta operacao nao existe)
	 *2) remover os buckets baseado na lista recebida
	 *3) remover arquivo correspondente ao diretorio no ceph
	 *4) remover do metadados
	 **/
	
	return ((state >= 0) ? 0 : 1);
}
 
/*prototipo: int create_bucket(char *srvName,char *dirName,char *idBucket,unsigned int maxKeys)
 *objetivo: Cria um Bucket em Diretorio especificado nos parametros de entrada, em conjunto com o
 *					 identificador do Bucket, e Servidor.*/
/*
	*header definition
	*|----------|----------|----------|----------|----------|
	 | LOCK     |qtdChaves | bitmap   | OFFSET#1 | LENGTH#1 |  
	 |----------|----------|----------|----------|----------|
	 | KEY#1 		| OFFSET#2 | LENGTH#2 |   KEY#2  | OFFSET#N |
	 |----------|----------|----------|----------|----------|
	 | LENGTH#N | KEY#N    | VALUE#1  ...  VALUE#N          |
	 |----------|----------|--------------------------------|
	*
	*max_keys = inteiro
	*bitmap = variavel
	*key (char(10)) conteudo da chave
	*offset (int) = localizacao dentro do bucket do value (deslocamento em relacao ao inicio do arquivo)
	*length = int tamanho de value
	*value (variavel) = valor do par chave-valor
	*
	*slot (key+offset+length)
*/
int ss_create_bucket(char *srvName,char *dirName,char *idBucket,unsigned int maxKeys) {
	
	char *header;
	unsigned char bitMap[maxKeys+1];
	unsigned int hsize,i;
	char caux[BYTES_LIMIT+1];
	char lock_flag;
	size_t len_head;
	
	lock_flag = '0';
	
	//gravacao do header
	
	//inicializacao do bitMap,preenche o vetor com zeros e o ultimo caracter nulo
	memset(bitMap,'\0',maxKeys+1);
	memset(bitMap,'0',maxKeys);
	
	//converte maxKeys para char[4];
	ntochr(caux, maxKeys);
	
	//define o tamanho do header, para a criacao do bucket
	hsize = HEADER_SIZE(maxKeys);
	header = (unsigned char*) xmalloc(sizeof(unsigned char) * hsize);
	
	//inicializacao do header
	
	//escreve a qtd e chaves e o mapa de bits no header
	sprintf(header,"%c%s%s",lock_flag,caux,bitMap);
	
	fprintf(stderr,"[create_bucket/iceph.c] header:%s\n",header);
	
	//preenche o restante do header com espacos em branco 
	len_head = strlen(header);
	memset(header+len_head,' ',hsize - len_head);
	
	//conecta no pool/servidor
	state = set_server(cluster,srvName,&ioctx);
	if(state >= 0){
		//seta o diretorio
		state = set_directory(dirName,&ioctx);
		
		//grava o bucket no diretorio 
		state = write_object_full(ioctx, idBucket, header, hsize);
	}else
		fprintf(stderr,"[create_bucket/iceph.c] Servidor não localizado!\n");
	
	free(header);
	
	//encerra o contexto de io
	destroy_ioctx(&ioctx);
	
	return ((state >= 0)? 0 : 1);
}

/*prototipo: int drop_bucket(char *idBucket,char *dirName,char *srvrName)
 *objetivo: Remove um Bucket, do Servidor e Diretório especificados por
 *parametros em conjunto com o identificador do Bucket*/
int ss_drop_bucket(char *idBucket,char *dirName,char *srvName) {

	// conecta no pool/servidor
	state = set_server(cluster, srvName,&ioctx);
	if(state >= 0){
		//seta o diretorio
		state = set_directory(dirName,&ioctx);
		
		/* remove o bucket do Diretorio
		   se a operacao falhar error > 0*/
		state = remove_object(ioctx,idBucket);
	}
	else
		fprintf(stderr,"[drop_bucket/iceph.c] Servidor não localizado!\n");
	
	destroy_ioctx(&ioctx);
	
	return ((state >= 0)? 0 : 1);
}

/*prototipo: int getBucket(char *srvName,char *dirName,char *idBucket)
 *objetivo: Retorna um Bucket, de um Diretorio e Servidor especificados nos parametros de entrada*/
int ss_get_bucket(char *srvName,char *dirName,char *idBucket,BUCKET_T *buff_bucket,int* hit){
	
	uint64_t len_bucket;
	time_t mtime;
	char path[PATH_SIZE+1];
	
	*buff_bucket = NULL;	
	*hit = 0; //0 sem cache 1 com cache
	
	snprintf(path,PATH_SIZE,"%s %s %s",srvName,dirName,idBucket);
	
	//cria um contexto de io no ceph, associando ao Pool que faz o papel de servidor
	state = set_server(cluster,srvName,&ioctx);
	if(state >= 0){
		/*se o contexto for criado seta o diretorio
		 *(associa uma chave ao contexto para influenciar o hash)*/ 
		state = set_directory(dirName,&ioctx);
		
		/*obtem o tamanho do bucket e a data de modficacao
		 *sera utilizado na leitura do bucket e para alocar memoria quando nao obtiver cache*/
		state = get_object_size(ioctx, idBucket, &len_bucket, &mtime);
		if(state < 0){
			fprintf(stderr,"[get_bucket/iceph.c] Bucket não localizado!\n");
			destroy_ioctx(&ioctx);
			
			return 1;
		}	
	}else{
		fprintf(stderr,"[get_bucket/iceph.c] Servidor não localizado!\n");
		destroy_ioctx(&ioctx);
		
		return 1;
	}
	
	/*busca no cache o Bucket desejado, se nao encontrar
	 *requisita no sistema de armazenamento e disponibiliza no cache*/
	*hit = get_buffer(idBucket,buff_bucket);
	if(*hit == 0){ //o bucket ainda nao foi para o buffer
		/*se tiver cache disponivel registra o bucket no cache
		 *apos a execucao buff_bucket aponta para a area disponivel*/
		*hit = put_buffer(idBucket,path,buff_bucket);
		if(*hit == 0) //se nao tiver cache disponivel aloca individual
			*buff_bucket = (BUCKET_T) xmalloc(len_bucket);
		
		//busca o bucket no sistema de armazenamento
		state = read_object(ioctx,idBucket,*buff_bucket,len_bucket,0,0);
		//IF TEMPORARIO APENAS DEBUG
		if(*hit == 1)
		  fprintf(stdout,"[get_bucket/iceph.c] cache:\n");
		else
			fprintf(stdout,"[get_bucket/iceph.c] no cache:\n");
	}else //APENAS DEBUG
		fprintf(stdout,"[get_bucket/iceph.c] cache:\n");
	
  destroy_ioctx(&ioctx);
	
	return ( (state >= 0) ? 0 : 1);

}

/*prototipo: int is_Empty(char *idBucket,char *dirName,char *srvName)
 *objetivo: Verifica se um Bucket, especificado por parametro, esta vazio, são indicados tambem o Diretorio e Servidor
 *TODO: pensar como fazer*/
int ss_is_empty_bucket(char *idBucket,char *dirName,char *srvName){
	
	int hit,pos,is_empty;
	unsigned char numKeysChr[BYTES_LIMIT+1];
	unsigned int numKeys;
	unsigned char *p_bitmap;
	BUCKET_T buff_bucket;
	
	/*hit indica se o alocs pegou o bucket do buffer ou do SA*/
	hit = -1;
	is_empty = 1;
	
	state = ss_get_bucket(srvName,dirName,idBucket,&buff_bucket,&hit);
	if(state >= 0){
		
		/*busca a qtd. max de chaves no bucket para 
		delimitar as leituras no mapa de bits*/
		memcpy(numKeysChr,buff_bucket,BYTES_LIMIT);
	
		// converte o char[5] numKeysChr para inteiro -- 4bytes+1nulo
		//obtem o numero max de chaves no bucket -> baseado no intervalo de chaves 
		numKeys = chrton(numKeysChr);
	
		p_bitmap = buff_bucket+BYTES_LIMIT;
		
		pos = -1;
		while((is_empty) && (pos < numKeys)){
			if(p_bitmap[++pos] == '1')
				is_empty = 0; 
		}
		
		p_bitmap = NULL;
		
		if(hit == 0) //hit == 0 indica que o bucket nao veio do buffer manager
			free(buff_bucket);	
	}
	
	return is_empty;
}

/*prototipo: int is_Empty(char *dirName,char *srvName)
 *objetivo: Verifica se um Diretório, especificado por parametro, está vazio, deve ser indicado tambem o Servidor
 *TODO: pensar como fazer*/
int ss_is_empty_dir(char *dirName,char *srvName){
	return 0;
}

/*prototipo: int put_pair(char *srvName,char *dirName,char *idBucket,KEY_T key,char *value)
 *objetivo: adicionar um par chave-valor no bucket passado por parametro*/
int ss_put_pair(char *srvName,char *dirName,char *idBucket,KEY_T key,char *value){

	unsigned char numKeysChr[BYTES_LIMIT+1];
	unsigned int numKeys;
	int acquired_lock,hit;
	BUCKET_T buff_bucket;
	
	hit = acquired_lock = 0;
	buff_bucket = NULL;
	
	//resgata o buffer do cache
	state = ss_get_bucket(srvName,dirName,idBucket,&buff_bucket,&hit);
	if(state >= 0){
		if(buff_bucket != NULL){
			/*verifica se este servidor tem permissao para escreve no bucket
			 *ele adquire permissao atraves da obtencao do lock, o nome do servidor
			 *fica registrado no buffer manager*/
			acquired_lock = get_lock(srvName,dirName,idBucket,&buff_bucket);
			fprintf(stderr,"[ss_put_pair/iceph.c] acquired_lock:%d\n",acquired_lock); 
			if(acquired_lock){
				//obter nr. maximo de chaves
				/*busca a qtd. max de chaves no bucket para 
				delimitar as leituras no mapa de bits*/
				//memcpy(numKeysChr,buff_bucket+LOCK_FLAG_SIZE,BYTES_LIMIT);
				get_numkey_bucket(&buff_bucket,numKeysChr);
				numKeys = chrton(numKeysChr);
				
				//escreve o par chave-valor no buffer
				state = write_pair_buffer(&buff_bucket,numKeys,key,value);
				
				//altera o parametro que indica buffer sujo para 1
				state = set_dirty(idBucket,1);
				
				/*se o get_bucket retornar um buffer que nao esta no cache a alteracao
				 *no buffer e propagada para o disco de imediato esta situacao deve ser excessao
				 *somente se o cache estiver indisponivel*/
				if(hit == 0){
					state = write_buffer_disk(srvName,dirName,idBucket,&buff_bucket,strlen(buff_bucket));
					free(buff_bucket);
				}
				//libera o lock do buffer
				state = unlock_buffer(srvName,dirName,idBucket,&buff_bucket);
				if(state == 1){
					fprintf(stderr,"[ss_put_pair/iceph.c] Erro ao liberar bloqueio do Buffer!\n"); 
					return 0;
				}	
			}else{
				fprintf(stderr,"[ss_put_pair/iceph.c] Sem permissão de escrita no Buffer!\n"); 
				return 0;
			}
		}	
	}
	
	//state retorna valor < 0 se ocorrer erro
	return ((state >= 0) ? 0 : 1);
} 

/*prototipo: int ss_remove_pair(char *srvName,char *dirName,char *idBucket,KEY_T key)
 *objetivo: Remove um par chave-valor do Bucket correspondente ao identificador passado como parametro
 *de entrada, e necessario fornecer tambem o Servidor e Diretorio*/
int ss_remove_pair(char *srvName,char *dirName,char *idBucket,KEY_T key){
	
	unsigned char numKeysChr[BYTES_LIMIT+1];
	unsigned char *header,*value;
	unsigned int hsize,numKeys,slot_pos,length,offset;
	int bit_pos;
	
	BUCKET_T buff_bucket;
	int hit;
	
	hit = 0;
	buff_bucket = NULL;
	
	//resgata o buffer do cache
	state = ss_get_bucket(srvName,dirName,idBucket,&buff_bucket,&hit);
	if(state >= 0){
		if(buff_bucket != NULL){
			/*busca a qtd. max de chaves no bucket para 
			delimitar as leituras no mapa de bits*/
			memcpy(numKeysChr,buff_bucket,BYTES_LIMIT);
			numKeys = chrton(numKeysChr);
			
			//remove o par do buffer
			state = remove_pair_buffer(&buff_bucket,numKeys,key);
			
			/*se o get_bucket retornar um buffer que nao esta no 
				cache a alteracao no buffer e propagada para o disco de imediato*/
			/*TODO:testar fora do if para ver se esta tudo certo - confirmado a execucao
			  o write_buffer_disk vai ser chamado somente no if e na finalizacao do cache*/ 
			state = write_buffer_disk(srvName,dirName,idBucket,&buff_bucket,strlen(buff_bucket));
			
			/*esta situacao deve ser excessao somente se o cache estiver
			 *indisponivel*/
			if(hit == 0){
				//state = write_buffer_disk(idBucket,&buff_bucket,numKeys);
				free(buff_bucket);
			}
		}
	}
	
}

/*prototipo int write_buffer_disk(char* srvName,char* dirName,char *idBucket,BUCKET_T *buffer,size_t len_buffer){
 * descricao: escreve o buffer do bucket em disco, esta funcao deve ficar neste arquivo
 * 						pois a escrita do buffer em disco depende da interface do sa
 * parametros: srvName ->servidor em que o bucket esta armazenado
 *             dirName -> diretorio em que o bucket esta armazenado
 * 						 idBucket -> identificador do bucket
 * 						 buffer -> buffer que sera gravado
 * 						 len_buffer -> tamanho do buffer que sera gravado
 * retorno: inteiro indicativo de sucesso ou falha da operacao*/
int write_buffer_disk(char* srvName,char* dirName,char *idBucket,BUCKET_T *buffer,size_t len_buffer){
	
	// conecta-se ao pool/servidor
	state	=	set_server(cluster,srvName,&ioctx);
	if(state < 0){
		fprintf(stderr,"[write_buffer_disk/iceph.c] Servidor não localizado!\n");
		return 1;
	}

	//seta o diretorio
	state	=	set_directory(dirName,&ioctx);
	if(state >= 0)
		state = write_object_full(ioctx, idBucket,*buffer, len_buffer);
		
	destroy_ioctx(&ioctx);
	
	return state;
}

/*prototipo: int get_lock(char *srvName,char *dirName,char *idBucket,BUCKET_T *buffer)
 *objetivo: verificar se o servidor que ira executar operacao de escrita adquiriu o lock ou nao*/
int get_lock(char *srvName,char *dirName,char *idBucket,BUCKET_T *buffer){
	
	char lock_flag;
	BUCKET_T header;
	char lock_owner[SRVNAME_SIZE];
	int locked;
	
	lock_flag = ' ';
	locked = 0;
	header = *buffer;
	
	memset(lock_owner,'\0',SRVNAME_SIZE);
	
	/*utilizar check_locked() para verifcar se o servidor ja adquiriu o lock
	  se ele ja tiver adquirido lock, locked vai retornar 1 e lock_owner vai
	  retornar o nome do servidor*/
	state = check_locked(idBucket,srvName,&locked,lock_owner);
	/*somente sera requerido lock se nao houver lock no bufffer
	  se o buffer ja tiver bloqueado pelo proprio servidor que esta pedindo
	  vai retornar 1 permitindo a operacao de escrita*/
	if((state == 1) && (locked == 0) && (strlen(lock_owner) == 0) ){
		// conecta-se ao pool/servidor
		state	=	set_server(cluster,srvName,&ioctx);
		if(state < 0){
			fprintf(stderr,"[get_lock/iceph.c] Servidor não localizado!\n");
			return 1;
		}
	
		//seta o diretorio
		state	=	set_directory(dirName,&ioctx);
		if(state >= 0){
			state = read_object(ioctx,idBucket,&lock_flag,LOCK_FLAG_SIZE,0,0);		
			if(state >= 0){
				if(lock_flag == '0'){
					lock_flag = '1';
					//altera o flag em disco
					state = write_object(ioctx,idBucket,&lock_flag,LOCK_FLAG_SIZE,0,0);
					if(state >= 0){
						//altera no buffer
						*(header+LCKFLAG_OFFSET) = lock_flag;
						//altera o parametro locked no buffer_pool
						state = set_locked(idBucket,srvName,lock_flag);
					}
				}else
				  return 0;
			}else
				return 0; //retorna 0 pois nao conseguir escrever no bucket
		}else{
			fprintf(stderr,"[get_lock/iceph.c] Diretório não localizado!\n");
			return 0;
		}
		header = NULL;
	}else if((state == 1) && (locked == 1))
		return 1; //srvName ja adquiriu o lock neste buffer

	return 1;	 
}

int unlock_buffer(char *srvName,char *dirName,char *idBucket,BUCKET_T *buffer){
	char lock_flag;
	BUCKET_T header;
	
	header = *buffer;
	
	state	=	set_server(cluster,srvName,&ioctx);
	if(state < 0){
		fprintf(stderr,"[unlock_buffer/iceph.c] Servidor não localizado!\n");
		return 1;
	}
	
	//seta o diretorio
	state	=	set_directory(dirName,&ioctx);
	if(state >= 0){
		lock_flag = '0';
		//altera o valor do flag no sa
		state = write_object(ioctx,idBucket,&lock_flag,LOCK_FLAG_SIZE,0,0);
		if(state >= 0){
			//altera no buffer
			*(header+LCKFLAG_OFFSET) = lock_flag;
			//altera o parametro locked no buffer_pool
			state = set_locked(idBucket,srvName,lock_flag);
		}else
			return 1; //retorna 1 pois nao conseguiu escrever no buffer
	}else{
		fprintf(stderr,"[unlock_buffer/iceph.c] Diretório não localizado!\n");
		return 1;
	}		

	return 0; 
}