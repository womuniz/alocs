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

int read_data(LOCALITY_T *local,TSTAMP_T tstamp,BUCKET_T *bucket);

/*
***********************************************
* Funcoes de gerenciamento
***********************************************
*/


/*prototipo: init_ssystem()
 * descricao: inicializa o sistema de armazenamento
 * parametros: */
int init_ssystem(void){

	// cria o manipulador do cluster
	create_handle(&cluster);
	if(cluster == NULL){
		fprintf(stderr,"[init_rados/iceph.c] cannot create a cluster handle\n");

		return 1;
	}

	return 0;
}

/*prototipo: fin_ssystem()
 * descricao: finaliza o sistema de armazenamento
 * parametros: */
int fin_ssystem(void){

	cluster_shutdown(&cluster);  // desconecta do cluster

	return 0;
}

/**********************************
DEFINICAO FUNCOES AUXILIARES
**********************************/

/*prototipo: int read_data(LOCALITY_T,TSTAMP_T,BUCKET_T)
 * descricao: esta funcao faz a leitura de um bucket no servidor corrente,
 *            nao faz leitura remota, desta forma não há conexao com outros servidores.
 *            Esta função é chamada na operação get_bucket quando o Servidor de Origem é o
 *            Servidor Corrente (CURRENT_SERVER)
 * parametros: */
int read_data(LOCALITY_T *local,TSTAMP_T tstamp,BUCKET_T *bucket){

	char version[IDBUCKET_SIZE+1],tstamp_c[TSTAMP_SIZE+1];;
	uint64_t len_bucket;
	time_t mtime;

	//cria um contexto de io no ceph, associando ao Pool que faz o papel de diretório
	state = set_directory(cluster,local->dirName,&ioctx);
	if(state < 0){
		fprintf(stderr,"[read_data/iceph.c] Diretório não localizado!\n");
		return 1;
	}

	//formata o nome do bucket com versao inicial que sera IDBUCKET-TSTAMP
	//é necessário converter pois tstamp e numerico (TODO: verificar se sera utilizado)
	itoc64(tstamp_c,sizeof(tstamp_c),tstamp);
	format_idBucket(version,sizeof(version),local->idBucket,tstamp_c);
	fprintf(stdout,"[get_bucket/iceph.c] version:%s\n",version);

	//busca o bucket no sistema de armazenamento
	state = read_object(ioctx,version,*bucket,LIMIT_SIZE_BUCKET,0,0);

	//encerra o contexto de io
	destroy_ioctx(&ioctx);

	return state;
}

/*prototipo int read_config_file(unsigned char*)
 * descricao: faz a leitura do arquivo de configuração, e retorna o conteúdo do arquivos
 *            em buffer
 * parametros: buffer ->buffer que receberá o conteúdo do arquivo
 * retorno: inteiro indicativo de sucesso ou falha da operacao*/
int read_config_file(unsigned char** buffer){

	uint64_t file_len;
	time_t mtime;

	file_len = 0;

	//cria um contexto de io no ceph, associando ao Pool que faz o papel de diretorio
	state = set_directory(cluster,DIR_CONF,&ioctx);
	if(state < 0){
		fprintf(stderr,"[read_config_file/iceph.c] Diretório não localizado!\n");

		return 1;
	}

	/*obtem o tamanho do bucket e a data de modficacao
	*sera utilizado na leitura do bucket e para alocar memoria quando nao obtiver cache*/
	state = get_object_size(ioctx,FILE_CONF,&file_len,&mtime);
	if(state < 0){
		fprintf(stderr,"[read_config_file/iceph.c] Arquivo de configurações não encontrado!\n");
		destroy_ioctx(&ioctx);

		return 1;
	}

	if(file_len > 0){
		*buffer = (unsigned char *)xmalloc(file_len);
   	init_str(*buffer,'\0',file_len);
		//busca o bucket no sistema de armazenamento
		state = read_object(ioctx,FILE_CONF,*buffer,file_len,0,0);
	}

	return ((state >= 0) ? 0 : 1);
}

/*prototipo int write_config_file()
 * descricao: cria o arquivo de configuração
 * parametros:
 * retorno: inteiro indicativo de sucesso ou falha da operacao*/
int write_config_file(){

	unsigned char* file_content;
	size_t file_len;
	char climit[CLIMIT_SLOT_SIZE],blimit[BLIMIT_SLOT_SIZE];
	size_t len_climit,len_blimit;

	file_content = NULL;

	//aloca memoria para escrever o conteudo do arquivo
	file_len = CLIMIT_SLOT_SIZE+BLIMIT_SLOT_SIZE+CSERVER_SLOT_SIZE;
	file_content = (unsigned char *) xmalloc((sizeof(unsigned char) * file_len)+1);

	memset(climit,'\0',CLIMIT_SLOT_SIZE);
	memset(blimit,'\0',BLIMIT_SLOT_SIZE);

  //formata climit
	itoc64(climit,CLIMIT_SLOT_SIZE,CACHE_LIMIT);
	len_climit = strlen(climit);
	memset(climit+len_climit,' ',CLIMIT_SLOT_SIZE - len_climit);

  //format blimit
	itoc64(blimit,BLIMIT_SLOT_SIZE,LIMIT_SIZE_BUCKET);
	len_blimit = strlen(blimit);
	memset(blimit+len_blimit,' ',BLIMIT_SLOT_SIZE - len_blimit);

	snprintf(file_content,file_len,"%s%s%s",climit,blimit,CURRENT_SERVER);

	//indica o diretorio
	state = set_directory(cluster,DIR_CONF,&ioctx);
	if(state >= 0)//grava o arquivo sinalizador do diretorio no servidor
		state = write_object_full(ioctx,FILE_CONF,file_content, file_len);

	//encerra o contexto de io
	destroy_ioctx(&ioctx);

	free(file_content);

	return ((state >= 0) ? 0 : 1);
}

/**********************************
DEFINICAO OPERACOES INTERFACE S.A.
**********************************/

/*prototipo: int create_server(char *srvName)
 *objetivo: Cria o Servidor com o identificador especificado por parametro.*/
/*int ss_create_server(char *srvName){

	//definir identificador da regra
	uint8_t crush_ruleset = 0;

	state = create_pool(cluster,srvName,crush_ruleset);

	return ((state >= 0) ? 0 : 1);
}*

/*prototipo: int create_server(char *srvName)
 *objetivo: Cria o Servidor com o identificador especificado por parametro.*/
/*int ss_drop_server(char *srvName){

	state = remove_pool(cluster,srvName);

	return ((state >= 0) ? 0 : 1);
}*/

/*prototipo: int create_dir(char *dirName,char *srvName)
 *objetivo: Cria um Diretorio em Servidor especificado por parametro.*/
int ss_create_dir(char *dirName,char *srvName){

	//definir identificador da regra
	uint8_t crush_ruleset = 0;

	state = create_pool(cluster,dirName,crush_ruleset);

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
	state = remove_pool(cluster,dirName);

	return ((state >= 0) ? 0 : 1);
}

/*prototipo: int create_bucket(char *srvName,char *dirName,char *idBucket,unsigned int maxKeys)
 *objetivo: Cria um Bucket em Diretorio especificado nos parametros de entrada, em conjunto com o
 *					 identificador do Bucket, e Servidor.*/
/*
	*header definition
	*|----------|----------|----------|----------|----------|
	 |LOCK      |qtdChaves | tstamp		|  bitmap	 | OFFSET#1 |
	 |----------|----------|----------|----------|----------|
	 | LENGTH#1 | KEY#1		 | OFFSET#2 |  LENGTH#2| KEY#2    |
	 |----------|----------|----------|----------|----------|
	 |OFFSET#N  | LENGTH#N | KEY#N    | VALUE#1 ... VALUE#N |
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

	unsigned char *header;
	unsigned char bitMap[maxKeys+1];
	char tstamp_c[TSTAMP_SIZE+1],maxKeys_c[BYTES_LIMIT+1],version[IDBUCKET_SIZE+1];
	char lock_flag;
	size_t len_head,hsize;

	lock_flag = '0';

	//inicializacao do bitMap,preenche o vetor com zeros e o ultimo caracter nulo
	init_str(bitMap,'0',maxKeys);
	//inicializacao de tstamp_c,preenche o vetor com zeros e o ultimo caracter nulo
	init_str(tstamp_c,'0',TSTAMP_SIZE);

	//converte maxKeys para char[4];
	ntochr(maxKeys_c,maxKeys);

	//conecta no pool/diretorio
	state = set_directory(cluster,dirName,&ioctx);
	if(state < 0){
		fprintf(stderr,"[ss_create_bucket/iceph.c] Diretório não localizado!\n");
		return 1;
	}

	//gravacao do header
	//define o tamanho do header, para a criacao do bucket
	hsize = HEADER_SIZE(maxKeys);
	header = (unsigned char*) xmalloc(sizeof(unsigned char) * hsize);

	//inicializacao do header
	//escreve a qtd e chaves e o mapa de bits no header
	sprintf(header,"%c%s%s%s",lock_flag,maxKeys_c,tstamp_c,bitMap);

	//preenche o restante do header com espacos em branco
	len_head = strlen(header);
	memset(header+len_head,' ',hsize - len_head);

	//grava o bucket no diretorio
	//formata o nome do bucket com versao inicial que sera IDBUCKET-0
	format_idBucket(version,sizeof(version),idBucket,"0");
	fprintf(stderr,"[ss_create_bucket/iceph.c] version:%s\n",version);
	state = write_object_full(ioctx,version,header,hsize);

	//encerra o contexto de io
	destroy_ioctx(&ioctx);

	free(header);

	return ((state >= 0)? 0 : 1);
}

/*prototipo: int drop_bucket(char *idBucket,char *dirName,char *srvrName)
 *objetivo: Remove um Bucket, do Servidor e Diretório especificados por
 *parametros em conjunto com o identificador do Bucket*/
int ss_drop_bucket(LOCALITY_T *local) {

	// conecta no pool/diretorio
	state = set_directory(cluster,local->dirName,&ioctx);
	if(state < 0){
		fprintf(stderr,"[drop_bucket/iceph.c] Diretório não localizado!\n");
		return 1;
	}

	/* remove o bucket do Diretorio
		 se a operacao falhar error > 0*/
	state = remove_object(ioctx,local->idBucket);

	destroy_ioctx(&ioctx);

	return ((state >= 0)? 0 : 1);
}

/*prototipo: int getBucket(char*,char*,char*,TSTAMP_T,BUCKET_T*,int*)
 *objetivo: Retorna um Bucket, de um Diretorio e Servidor especificados nos parametros de entrada*/
int ss_get_bucket(LOCALITY_T *local,TSTAMP_T tstamp,BUCKET_T *bucket){

	int hit;

	*bucket = NULL;
	hit = 1; //0 sem cache 1 com cache

	/*busca no cache o Bucket desejado, se nao encontrar
	 *requisita no sistema de armazenamento e disponibiliza no cache*/
	hit = get_data_cache(local->idBucket,bucket);
	if(!hit){
		/*o bucket ainda nao foi para o buffer
	    as linhas de cache no alocs são pré-alocadas, put_data_cache busca uma linha
		 *disponível e retorna um ponteiro para a linha obtida (bucket)*/
	  hit = put_data_cache(local->idBucket,local,bucket);
		if(hit){
			fprintf(stderr,"[ss_get_bucket/iceph.c] local->srvName:%s.\n",local->srvName);
			fprintf(stderr,"[ss_get_bucket/iceph.c] CURRENT_SERVER:%s.\n",CURRENT_SERVER);
			fprintf(stderr,"[ss_get_bucket/iceph.c] strcmp(local->srvName,CURRENT_SERVER):%d\n",strcmp(local->srvName,CURRENT_SERVER));
			if(strcmp(local->srvName,CURRENT_SERVER) == 0) //Servidor Origem == Servidor Corrente
				state = read_data(local,tstamp,bucket);
			/*else  Servidor Origem != Servidor Corrente
			 *  request_bucket()*/
		}else{
			fprintf(stderr,"[ss_get_bucket/iceph.c] Erro ao colocar Bucket no cache. A operação falhou.\n");
			return 1;
		}
	}

  //state >=0 indica exec. sem falha
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
	BUCKET_T bucket;

	/*hit indica se o alocs pegou o bucket do buffer ou do SA*/
	hit = -1;
	is_empty = 1;

	/*state = ss_get_bucket(srvName,dirName,idBucket,&bucket,&hit);
	if(state >= 0){

		memcpy(numKeysChr,bucket,BYTES_LIMIT);

		// converte o char[5] numKeysChr para inteiro -- 4bytes+1nulo
		//obtem o numero max de chaves no bucket -> baseado no intervalo de chaves
		numKeys = chrton(numKeysChr);

		p_bitmap = bucket+BYTES_LIMIT;

		pos = -1;
		while((is_empty) && (pos < numKeys)){
			if(p_bitmap[++pos] == '1')
				is_empty = 0;
		}

		p_bitmap = NULL;

		if(hit == 0) //hit == 0 indica que o bucket nao veio do buffer manager
			free(bucket);
	}
	*/
	return is_empty;
}

/*prototipo: int is_Empty(char*,char*)
 *objetivo: Verifica se um Diretório, especificado por parametro, está vazio, deve ser indicado tambem o Servidor
 *TODO: pensar como fazer*/
int ss_is_empty_dir(char *dirName,char *srvName){
	return 0;
}

/*prototipo: int (char*,char*,char*,TSTAMP_T,KEY_T,PAIR_T*)
 *objetivo: Retorna o valor associado a chave key*/
int ss_get_key(LOCALITY_T *local,TSTAMP_T tstamp,KEY_T key,PAIR_T *pair){

	BUCKET_T bucket;

	/*solicitar o bucket ao cache ou ao SA
		bucket e atualizado com o buffer do bucket*/
	state = ss_get_bucket(local,tstamp,&bucket);
	if(state == 0) //get_bucket retorna 1 para falha e 0 para sucesso
		state = extract_value(&bucket,key,pair); //extrai o valor e escreve em pair->value

	return ( (state >= 0) ? 0 : 1);
}

/*prototipo: int ss_put_data(LOCALITY_T,TSTAMP_T,KEY_T,char*)
 *objetivo: adicionar um par chave-valor no bucket passado por parametro*/
int ss_put_data(LOCALITY_T *local,TSTAMP_T tstamp,KEY_T key,char *value){

	int acquired_lock;
	BUCKET_T bucket;

	acquired_lock = 0;
	bucket = NULL;

	/*resgata o buffer do cache
	 *a variavel a ser utilizada deve ser idBucket, nao colocaremos o nome do bucket
	 *com tstamp no cache*/
  fprintf(stderr, "local->idBucket:%s,local->srvName:%s,local->dirName:%s \n",local->idBucket,local->srvName,local->dirName);
	state = ss_get_bucket(local,tstamp,&bucket);
	if(state >= 0){
		if(bucket != NULL){
			/*obtem lock para a linha de cache do bucket*/
			//TODO: definir o servico de lock após acertar o modelo multiversao
			//acquired_lock = get_lock(srvName,dirName,idBucket,&bucket);
			//fprintf(stderr,"[ss_put_pair/iceph.c] acquired_lock:%d\n",acquired_lock);
			//if(acquired_lock){
				//obter nr. maximo de chaves
				/*busca a qtd. max de chaves no bucket para
				delimitar as leituras no mapa de bits*/

				//escreve o par chave-valor no buffer
				/*TODO:MODELO MULTIVERSAO - DEVO CRIAR UM NOVO BUCKET
				  neste ponto continuo escreve somente o par normalmente.
				  a diferenca será no momento que encerrar a transacao*/
				state = write_pair(&bucket,key,value);
				if(state == 1)
					return 1; //erro - interrompe o processo

				fprintf(stdout,"[ss_put_data/iceph.c] bucket pos write_pair:%s\n",bucket);

				//altera o parametro que indica buffer sujo para 1
				state = set_dirty(local->idBucket,1);

				/*debug -> apenas para verificar se está gerando nova versao
				  esta linha deve sair daqui com a implementacao do controle transacional*/
				///state = persist_bucket(local,tstamp,&bucket,strlen(bucket));

				//libera o lock do buffer
				/*state = unlock_buffer(srvName,dirName,idBucket,&bucket);
				if(state == 1){
					fprintf(stderr,"[ss_put_pair/iceph.c] Erro ao liberar bloqueio do Buffer!\n");
					return 0;
				}*/

			/*}else{
				fprintf(stderr,"[ss_put_pair/iceph.c] Sem permissão de escrita no Buffer!\n");
				return 0;
			}*/
		}
	}

	//state retorna valor < 0 se ocorrer erro
	return ((state >= 0) ? 0 : 1);
}

/*prototipo: int ss_remove_pair(char*,char*,char*,TSTAMP_T,KEY_T)
 *objetivo: Remove um par chave-valor do Bucket correspondente ao identificador passado como parametro
 *de entrada, e necessario fornecer tambem o Servidor e Diretorio*/
int ss_remove_data(LOCALITY_T *local,TSTAMP_T tstamp,KEY_T key){

	BUCKET_T bucket;

	bucket = NULL;

	//resgata o buffer do cache
	state = ss_get_bucket(local,tstamp,&bucket);
	if(state >= 0){
		if(bucket != NULL){
			//remove o par do buffer
			state = rm_pair(&bucket,key);

			//altera o parametro que indica buffer sujo para 1
			state = set_dirty(local->idBucket,1);

			/*debug -> apenas para verificar se está gerando nova versao
				  esta linha deve sair daqui com a implementacao do controle transacional*/
			state = persist_bucket(local,tstamp,&bucket,strlen(bucket));
		}
	}
}

/*prototipo int persist_bucket(char* srvName,char* dirName,char *idBucket,BUCKET_T *buffer,size_t len_buffer){
 * descricao: escreve o buffer do bucket em disco, esta funcao deve ficar neste arquivo
 * 						pois a escrita do buffer em disco depende da interface do sa
 * parametros: srvName ->servidor em que o bucket esta armazenado
 *             dirName -> diretorio em que o bucket esta armazenado
 * 						 idBucket -> identificador do bucket
 * 						 buffer -> buffer que sera gravado
 * 						 len_buffer -> tamanho do buffer que sera gravado
 * retorno: inteiro indicativo de sucesso ou falha da operacao*/
int persist_bucket(LOCALITY_T *local,TSTAMP_T tstamp,BUCKET_T *buffer,size_t len_buffer){

	//guarda a versao do bucket que é IDBUCKET+TSTAMP (tstamp sem formatacao)
	char version[IDBUCKET_SIZE+1];
	//guarda tstamp com formatacao - com zeros a esquerda
	char tstamp_c[TSTAMP_SIZE+1];
	//guarda tstamp sem formatacao
	char tstamp_sformat[TSTAMP_SIZE+1];
	unsigned char *tstamp_offset;
	size_t init_pos;

	/*o tstamp por enquanto sera definido como numerico
	*pois ele vira de um relogio local que tratara apenas numeros*/
  //inicializa tstamp_c
	init_str(tstamp_c,'0' ,TSTAMP_SIZE);

	/*converte tstamp para concatenar
	 *antes de concatenar incrementa tstamp em 1*/
	itoc64(tstamp_sformat,TSTAMP_SIZE,++tstamp);

	//conecta-se ao pool/diretorio
	state	=	set_directory(cluster,local->dirName,&ioctx);
	if(state < 0){
		fprintf(stderr,"[persist_bucket/iceph.c] Diretório não localizado!\n");
		return 1;
	}

	//escreve o tstamp da versao no header do bucket
	//init pos marca o ponto inicial para o escrever tstamp no vetor
	init_pos = TSTAMP_SIZE - strlen(tstamp_sformat);
  //escreve o tstamp em tstamp_c formatando com zeros a esquerda
	memmove(tstamp_c+init_pos,tstamp_sformat,strlen(tstamp_sformat));

	//modifica o timestamp no header
	tstamp_offset = *buffer+TSTAMP_OFFSET;
	memmove(tstamp_offset,tstamp_c,TSTAMP_SIZE);

	//obtem o nome do bucket concatenado com tstamp indicando a versao do bucket
	//concatena o nome do bucket com o tstamp
	format_idBucket(version,sizeof(version),local->idBucket,tstamp_sformat);
	fprintf(stderr,"[persist_bucket/iceph.c] version:%s \n",version);
	fprintf(stderr,"[persist_bucket/iceph.c] buffer antes de obj full:%s \n",*buffer);

	//neste momento deve ser criado um novo bucket,o tstamp deve ser passado para a funcao
	state = write_object_full(ioctx,version,*buffer,len_buffer);

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

	lock_flag = '0';
	locked = 0;
	header = *buffer;

	memset(lock_owner,'\0',SRVNAME_SIZE);

	/*check-locked verifica se a linha de cache esta bloqueada
	TODO: implementar servico de lock de acordo: neste momente somente do cache*/
	state = check_locked(idBucket,srvName,&locked,lock_owner);

	if((state == 1) && (locked == 0) && (strlen(lock_owner) == 0) ){
		// conecta-se ao pool/diretório
		state	=	set_directory(cluster,dirName,&ioctx);
		if(state < 0){
			fprintf(stderr,"[get_lock/iceph.c] Diretório não localizado!\n");
			return 1;
		}

		state = read_object(ioctx,idBucket,&lock_flag,LOCK_FLAG_SIZE,0,0);
		if(state < 0)
		  return 0;

		if(lock_flag == '1') //o bucket já esta bloqueado
		  return 0;

		lock_flag = '1';
		//altera o flag em disco
		state = write_object(ioctx,idBucket,&lock_flag,LOCK_FLAG_SIZE,0,0);
		if(state >= 0){
			//altera no buffer
			*(header+LCKFLAG_OFFSET) = lock_flag;
			//altera o parametro locked no buffer_pool
			state = set_locked(idBucket,srvName,lock_flag);
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

	state	=	set_directory(cluster,dirName,&ioctx);
	if(state < 0){
		fprintf(stderr,"[unlock_buffer/iceph.c] Diretório não localizado!\n");
		return 1;
	}

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

	return 0;
}
