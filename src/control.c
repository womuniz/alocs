/*objetivo: modulo de Controle
  funcoes relacionadas ao modulo de controle
  comunicação entre os sistemas de metadados e armazenamento */
#include "include/control.h"

int error;

/*prototipo int load_config()
 * descricao: carrega parametros de funcionamento do alocs
 * parametros: */
int load_config();

/*prototipo int create_config_file
 * descricao: possibilita a criação do arquivo de configuração
 * parametros: */
int create_config_file();

/********************************************
 *inicio funcoes de gerenciamento
 ********************************************/

/*prototipo int create_config_file
 * descricao: possibilita a criação do arquivo de configuração
 * parametros: */
int create_config_file(){

	fprintf(stdout,"\n");
	fprintf(stdout,"Capacidade do Cache: ");scanf("%lu",&CACHE_LIMIT);
	fprintf(stdout,"Limite para o tamanho do Bucket [bytes]: ");scanf("%lu",&LIMIT_SIZE_BUCKET);
	fprintf(stdout,"Identificador do Servidor Corrente: ");scanf("%s",CURRENT_SERVER);
	fprintf(stdout,"\n");

	error = write_config_file();

	return error;
}

/*prototipo int load_config()
 * descricao: carrega parametros de funcionamento do alocs
 * parametros: */
int load_config(){

		char climit[CLIMIT_SLOT_SIZE+1],blimit[BLIMIT_SLOT_SIZE+1];
		unsigned char *buffer;
		uint64_t file_len;

		file_len = CLIMIT_SLOT_SIZE+BLIMIT_SLOT_SIZE+CSERVER_SLOT_SIZE;

		error = read_config_file(&buffer);
		if(!error){
			if(buffer){
				fprintf(stderr,"[load_config/control.c] Tratamento do arquivo.\n");
				//tratamento arquivo
				memmove(climit,buffer,CLIMIT_SLOT_SIZE);
				memmove(blimit,buffer+CLIMIT_SLOT_SIZE,BLIMIT_SLOT_SIZE);

				init_str(CURRENT_SERVER,'\0',CSERVER_SLOT_SIZE);
				memmove(CURRENT_SERVER,buffer+(CLIMIT_SLOT_SIZE+BLIMIT_SLOT_SIZE),CSERVER_SLOT_SIZE);
				fprintf(stdout,"[load_config/control.c] Servidor Corrente:%s\n",CURRENT_SERVER);

				CACHE_LIMIT = ctoi(climit,1);
				LIMIT_SIZE_BUCKET = ctoi(blimit,1);
			}else
				error = create_config_file();
		}else{
			fprintf(stderr,"[load_config/control.c] Erro ao carregar configurações do Alocs!\n");

			return 1;
		}

    free(buffer);
		return 0;
}

int init_api(){



	//inicializa sistema de armazenamento
	error = init_ssystem();
	if(!error){ //inicializa o cache se o sistema de armazenamento iniciar
	  //carrega configuracoes
	  error = load_config();
    if(!error)
		   error = init_cache();

		//inicializa o sistema de metadados
	 	error = init_mds();
  }


	return error;
}

int fin_api(){

	DIRTYLINES_T *lst_dirty;
	char srv_name[SRVNAME_SIZE+1],dir_name[DIRNAME_SIZE+1],id_bucket[IDBUCKET_SIZE+1];
	BUCKET_T buffer;
	//int state;

	//state = 0;

	/*identifica os buffers sujos para atualizar
	  no sistema de armazenamento*/
	/*retOper = find_dirty_buffers();
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
	}*/

	/*if(state == 0){

	}*/

	//finaliza o cache
	error = fin_cache();
	//finaliza o sistema de armazemento
	error = fin_ssystem();
	//finaliz o sistema de metadados
	error = fin_mds();

	return error;
}
