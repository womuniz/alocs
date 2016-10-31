#include "include/cache.h"

unsigned int BUFF_SIZE;

/*prototipo int init_cache()
 * descricao: inicializa o cache no ALOCS, esta funcao e chamada na funcao init no common.c
 * parametros: 
 * retorno: um inteiro indicativo de falha ou sucesso*/
int init_cache(){
	
	LOCATOR_T *loc_ptr;
	FRAME_T *frame;
  int pos;
	
	BUFF_SIZE = 0;
	
	//inicializa a lista de buffers sujos
	DIRTY_BUFFERS = NULL;
	
	//inicializa o indice
	LOCATOR = (LOCATOR_T *) xmalloc(sizeof(LOCATOR_T) * HT_SIZE);

  loc_ptr = LOCATOR+0;
	
  //inicializacao de node
  while((loc_ptr != NULL) && (loc_ptr < LOCATOR+HT_SIZE)){
    loc_ptr->node = NULL;
		loc_ptr++;
  }

	//inicializa o buffer_pool
	BUFFER_POOL = (FRAME_T *) xmalloc(sizeof(FRAME_T) * BUFFER_LIMIT);
	for(pos = 0;pos < BUFFER_LIMIT;pos++){
		frame = BUFFER_POOL+pos;
		
		frame->pin_count = 0;
		frame->dirty = 0;
		frame->locked = 0;
		memset(frame->path,'\0',PATH_SIZE+1);
		
		frame->buffer = (unsigned char *) xmalloc(sizeof(unsigned char) * LIMIT_SIZE_BUCKET);
	}

	frame = NULL; 
	
	fprintf(stdout,"Cache inicilizado!\n");
}

/*prototipo int init_cache()
 * descricao: inicializa o cache no ALOCS, esta funcao e chamada na funcao init no common.c
 * parametros: 
 * retorno: um inteiro indicativo de falha ou sucesso*/
int fin_cache(){
	
	LOCATOR_T *loc_ptr;
	NODELOC_T *node,*next_node;
	FRAME_T *frame;
	int pos;
	
	pos = 0;
	
	next_node = NULL;
	frame = NULL;
	
  loc_ptr = LOCATOR+0;
  
	//libera a memoria utilizada por LOCATOR
	while((loc_ptr != NULL) && (loc_ptr < LOCATOR+HT_SIZE)){
		node = loc_ptr->node;
		while(node != NULL){
			next_node = node->next;
			
			node->frame = NULL;
			free(node);
			
			node = next_node;
		}
		loc_ptr++;
	}
  free(LOCATOR);
	
	/*libera a memoria utilizada por BUFFER_POOL
	 *pos inicia em zero*/
	//frame = BUFFER_POOL+pos;
	for(pos = 0;pos < BUFFER_LIMIT;pos++){
		frame = BUFFER_POOL+pos;
		
		if(frame->buffer != NULL)
			free(frame->buffer);
	}
		
	free(BUFFER_POOL);
}

/*prototipo int put_buffer(char* key,char *path,unsigned char** buffer)
 * descricao: adiciona um bucket no cache retornando um ponteiro para o buffer designado
 * parametros: key ->   idBucket do Bucket que esta sendo adicionado ao cache
 * 						 path ->  path do bucket no metadados por enquanto nao esta sendo utilizado
 * 						 buffer-> espaco no cache designado para o bucket
 * 						 int write_oper,int acquired_lock (ESTAS VARIAVEIS DEVEM SER UTILIZADAS NO ICEPH)
 * retorno: um inteiro indicativo de falha ou sucesso na obtencao do cache*/
int put_buffer(char* key,char *path,unsigned char** buffer){
	
	FRAME_T *frame;
	NODELOC_T *new_node,*node;
  LOCATOR_T *loc_ptr;
	
	loc_ptr = NULL;
  new_node = node = NULL;
	
	loc_ptr = LOCATOR+get_hash(key);
	
  //gravacao do indice
	if(loc_ptr == NULL)
    return 0;
  
  //aloca memoria para o novo node 
	new_node = (NODELOC_T *) xmalloc(sizeof(NODELOC_T));
  
  //inicializa as areas de memoria
  memmove(new_node->key,key,KEY_SIZE);
	
	//seleciona o frame no buffer pool
	if(BUFF_SIZE < BUFFER_LIMIT){
		frame = BUFFER_POOL+BUFF_SIZE;
		//buffer -> retorna para iceph.c a area disponivel no cache
		if(frame != NULL){
			frame->pin_count++;
			memmove(frame->path,path,PATH_SIZE);
			
			*buffer = frame->buffer;
			new_node->frame = frame;
		}else 
			return 0;

		//incrementa o controle do buffer
		BUFF_SIZE++;
	}else //TODO:acertar renovacao do buffer
		return 0; //se nao tiver espaco no cache retorna 1 e feita a recuperacao a partir do disco
	
	new_node->next = NULL;
	
	//adiciona a referencia do buffer no localizador 
	//verifica se a posicao na tabela ja esta ocupada, se nao tiver inicia uma lista
	node = loc_ptr->node;
	if(node == NULL) 
		loc_ptr->node = new_node;
	else{
		//adiciona no final da lista
		while(node != NULL)
			node = node->next;
		
		node->next = new_node;
	}
	
	return 1;
}

/*prototipo: int get_buffer(char* key, unsigned char* buffer)
 * descricao: localiza o cache referente ao bucket identificado por parametro 
 * parametros: key -> id do Bucket que que sera pesquisado
 * 						 buffer -> aponta para o buffer do bucket no cache	
 * retorno: um inteiro indicativo de falha ou sucesso na busca pelo bucket*/
int get_buffer(char* key,unsigned char** buffer){
	
	NODELOC_T *node,*target;
	LOCATOR_T *loc_ptr;
	FRAME_T *frame;
	
  loc_ptr = NULL;  
  target = node = NULL;

	loc_ptr = LOCATOR+get_hash(key);

	//se a posicao encontrar nao tiver node retorna NULL
  if(!loc_ptr->node)
    return 0;

  //se a chave do primeiro node for a procurada
	if(strcmp(loc_ptr->node->key,key) == 0)
		target = loc_ptr->node;  
	else if(loc_ptr->node->next){	
    //se nao for o primeiro percorre a lista ate encontrar a chave
    node = loc_ptr->node;		
    while((node != NULL)&&(strcmp(node->key,key) != 0))
			node = node->next;
				
		target = node;
	}
	
	if(target != NULL){
		frame = target->frame;
		
		if(frame != NULL){
			frame->pin_count++;	
			*buffer = frame->buffer;
		}
		return 1;
	}else
	  return 0;
	
}

/*prototipo: int remove_buffer(char* key)
 * descricao: remove do cache o bucket identificado pela chave passada por parametro 
 * parametros: key ->   idBucket do Bucket que esta no cache
 * retorno: um inteiro indicativo de falha ou sucesso na obtencao do cache*/
int remove_buffer(char *key){
	
	NODELOC_T *node;
	LOCATOR_T *loc_ptr,*target;
	FRAME_T *frame;
	
  target = NULL;
  node = NULL;
 
	loc_ptr = LOCATOR+get_hash(key);
	
  if(!loc_ptr)
    return 0;

	if(loc_ptr->node != NULL){
    //target aponta para o inicio da lista encadeada em LOCATOR+get_hash(key)
		target = loc_ptr;
    //se a lista encadeada tiver mais de um node o primeiro nao e o desejado
    if((target->node->next) && (strcmp(target->node->key,key) != 0)){  
      node = target->node;
      while((node != NULL)&&(strcmp(node->next->key,key) != 0 ))
				node = node->next;
			
			target->node = node->next;
			node->next = node->next->next;
    }
  }
		
  if(target != NULL){
		memset(target->node->key,'\0',KEY_SIZE);
		
		frame = target->node->frame;
		memset(frame->buffer,'\0',LIMIT_SIZE_BUCKET);
		memset(frame->path,'\0',PATH_SIZE);
		
		free(target->node);
		target->node = NULL;
	}
	else
		return 1;
	
	return 0;
	
}

/*prototipo: int check_locked(char *key,char *lock_owner, int *locked, char* lock_owner)
 * descricao: busca o valor do parametro locked no buffer_pool e compara o
 * 						lock_owner passado por parametro com o registrado no buffer manager
 * parametros: key -> chave para encontrar o buffer
 * 						 lock_owner -> srvName que fica como o lock_owner
 * retorno: um inteiro indicativo da obtencao do lock pelo lock_owner*/
int check_locked(char *key,char *requester, int *locked, char* lock_owner){
	
	LOCATOR_T *loc_ptr;
	FRAME_T *frame;
	int state;
	
  loc_ptr = NULL;  
  frame = NULL;

	loc_ptr = LOCATOR+get_hash(key);
	
	if(loc_ptr != NULL){
		frame = loc_ptr->node->frame;
		*locked = frame->locked;
		memmove(lock_owner,frame->lock_owner,LCKOWNER_SIZE);
		
		if(frame->locked == 0)
			state = 1;
		else if(frame->locked == 1)
			state = (strcmp(frame->lock_owner,requester) == 0 ? 1 : 0);	
	}
	
	return state;
	
}

/*prototipo: int find_dirty_buffers()
 * descricao: percorre o buffer pool em busca de buffers alterados (sujos)
 * 						os buffer sujos sao apontados na lista DIRTY_BUFFERS que e acessada
 * 						pela interface do sa
 * parametros: 
 * retorno: qtd de buffers alterados */
int find_dirty_buffers(){
	
	int amount;
	FRAME_T *frame;
	DIRTYBUFF_T *lst_node,*list;
	
	amount = 0;
	
	//dirty_list = DIRTY_BUFFERS;
	
	frame = BUFFER_POOL+0;
  while(frame <  (BUFFER_POOL+BUFFER_LIMIT)){
		if(strlen(frame->path) > 0){
			if(frame->dirty == 1){
				lst_node = (DIRTYBUFF_T *) xmalloc(sizeof(DIRTYBUFF_T));
				
				memmove(lst_node->path,frame->path,PATH_SIZE);
				lst_node->buffer = frame->buffer;
				lst_node->next = NULL;
				
				list = DIRTY_BUFFERS;
				if(list != NULL){
					while(list != NULL)
						list = list->next;
					
					list->next = lst_node;
				}else
					DIRTY_BUFFERS = lst_node;
				
				amount++;	
			}
		}
		frame++;
	}
	
	return amount;
}

/*prototipo: int set_lock(char*,char*,int)
 * descricao: altera o valor do parametro locked no buffer_pool 
 * parametros: key -> chave para encontrar o buffer
 * 						 lock -> valor que sera atribuido ao parametro
 * retorno: um inteiro indicativo de falha ou sucesso da operacao*/
int set_locked(char *key,char *lock_owner,int locked){
	
	LOCATOR_T *loc_ptr;
	FRAME_T *frame;
	
  loc_ptr = NULL;  
  frame = NULL;

	loc_ptr = LOCATOR+get_hash(key);
	
	if(loc_ptr != NULL){
		frame = loc_ptr->node->frame;
		frame->locked = locked;
		memmove(frame->lock_owner,lock_owner,LCKOWNER_SIZE);
	}
	
	return 0;
}

/*prototipo: int set_dirty(char*,int)
 * descricao: altera o valor do parametro dirty no buffer_pool 
 * parametros: key -> chave para encontrar o buffer
 * 						 dirty -> valor que sera atribuido ao parametro
 * retorno: um inteiro indicativo de falha ou sucesso da operacao*/
int set_dirty(char *key, int dirty){
	
	LOCATOR_T *loc_ptr;
	FRAME_T *frame;
	
  loc_ptr = NULL;  
  frame = NULL;

	loc_ptr = LOCATOR+get_hash(key);
	
	if(loc_ptr != NULL){
		frame = loc_ptr->node->frame;
		frame->dirty = dirty;
	}
	
	return 0;
}

/*prototipo: unsigned int get_hash(char* str)
 * descricao: atribui um hash para a localizacao do bucket no cache 
 * parametros: str ->   idBucket do Bucket que esta no cache*/
unsigned int get_hash(char* str){

  int i;
  unsigned int number,h;

  number = h = 0;
 
  for(i = 0;i < strlen(str);i++)
     number += *(str+i);

  h = HASH(number);
  
  return h;
}
