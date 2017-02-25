#include "include/cache.h"

/*prototipo unsigned int get_time()
 * descricao: incrementa o rel. logico local
 * parametros:
 * retorno: timestamp que marca o tempo em que a operacao ocorreu*/
unsigned int get_time();

/*prototipo int check_limit()
 * descricao: verifica se o limite do cache foi atingido
 * parametros:
 * retorno: 0 (nao atingido) 1 (atingido)*/
int check_limit();

/*prototipo HEAPLRU_T search_data(char*)
 * descricao: localiza um bucket no cache
 * parametros: char* key chave relacionado ao item na hash table (idBucket)
 * retorno: a linha de cache obtida por meio do algoritmo lru*/
HEAPLRU_T *search_data(char* key);

/*prototipo LINE_T *exec_policy()
 * descricao: executa a politica de renovacao de cache baseada na LRU
 * parametros:
 * retorno: a linha de cache obtida por meio do algoritmo lru*/
LINE_T *exec_policy();

/*prototipo int get_line(LINE_T)
 * descricao: obtém uma linha de cache para alocar o bucket
 * parametros: LINE_T **line aponta para a a area de memoria alocada para linha de cache
 * retorno: um inteiro indicativo de falha ou sucesso*/
int get_line(LINE_T **line);

int error;

//indica a posicao no vetor que mantem o cache
int CACHE_INDX = 0;
//mantém o relogio logico local
unsigned int CLOCK = 0;

/*prototipo int init_cache()
 * descricao: inicializa o cache no ALOCS, esta funcao e chamada na funcao init no common.c
 * parametros:
 * retorno: um inteiro indicativo de falha ou sucesso*/
int init_cache(){

	LINE_T *line;
  int pos;

	CACHE_INDX = 0;

	//inicializa a lista de buffers sujos
	DIRTY_LINES = NULL;

	//cria a htable
	create_htable();

	//cria o heap para o lru
	create_heap();

	//cria o cache
  CACHE = (LINE_T *) xmalloc(sizeof(LINE_T) * CACHE_LIMIT);

	for(pos = 0;pos < CACHE_LIMIT;pos++){
 	  line = CACHE+pos;

 	  line->pin_count = 0;
 	  line->dirty = 0;
 	  line->locked = 0;

 	  line->data = (unsigned char *) xmalloc(sizeof(unsigned char) * LIMIT_SIZE_BUCKET);
  }

	line = NULL;

	fprintf(stdout,"Cache inicilizado!\n");
}

/*prototipo int fin_cache()
 * descricao: finalizar o cache no ALOCS, esta funcao e chamada na funcao init no common.c
 * parametros:
 * retorno: um inteiro indicativo de falha ou sucesso*/
int fin_cache(){

	NODELOC_T *node,*next_node;
	int pos;

	pos = 0;

	next_node = NULL;

  //libera memoria utilizada pela hash table
	for(pos = 0;pos < HTSIZE;pos++){
    node = (LOCATOR+pos)->node;
		while(node){
			next_node = node->next;
			free(node);

			node = next_node;
		}
	}
	free(LOCATOR);

	/*libera a memoria utilizada por BUFFER_POOL
	 *pos inicia em zero*/
	free(CACHE);

	//libera memoria utilizada pelo heap
	free(HLRU);
}

/*prototipo unsigned int get_time()
 * descricao: incrementa o rel. logico local
 * parametros:
 * retorno: timestamp que marca o tempo em que a operacao ocorreu*/
unsigned int get_time(){
  return CLOCK++;
}

/*prototipo int check_limit()
 * descricao: verifica se o limite do cache foi atingido
 * parametros:
 * retorno: 0 (nao atingido) 1 (atingido)*/
int check_limit(){
  //printf("[check_limit] CACHE_INDX < CACHE_LIMIT:%d\n",CACHE_INDX < CACHE_LIMIT);
  return ((CACHE_INDX < CACHE_LIMIT)?0:1);
}

/*prototipo HEAPLRU_T search_data(char*)
 * descricao: localiza um bucket no cache
 * parametros: char* key chave relacionado ao item na hash table (idBucket)
 * retorno: a linha de cache obtida por meio do algoritmo lru*/
HEAPLRU_T *search_data(char* key){

	NODELOC_T *node;
	HEAPLRU_T *it_heap;

	node = NULL;it_heap = NULL;

	node = search_node_ht(key);
  if(node)
    it_heap = node->it_heap;

	return it_heap;
}

/*prototipo LINE_T *exec_policy()
 * descricao: executa a politica de renovacao de cache baseada na LRU
 * parametros:
 * retorno: a linha de cache obtida por meio do algoritmo lru*/
LINE_T *exec_policy(){

	HEAPLRU_T *it_heap;
	LINE_T *line;

  line = NULL;
  it_heap = NULL;
  error = 0;

  it_heap = (HEAPLRU_T *) xmalloc(sizeof(HEAPLRU_T));

  error = get_least_used(&it_heap);

  if(!it_heap)
    return NULL;

  //recupera a linha de cache
  line = it_heap->line;

  if(!line)
    return NULL;

  //encontrou linha menos utilizada mas esta em uso no momento
  /*printf("[exec_policy] it_heap->indx:%d\n",it_heap->indx);
  printf("[exec_policy] line->key:%d\n",line->key);
  printf("[exec_policy] line->data:%s\n",line->data);*/

  if((line)&&(line->pin_count > 0)){
    /*extrai itens ate encontrar linha que nao esteja em uso
		adiciona it_heap novamente no heap incrementando o tstamp pela
		 propriedade do heap minimo ele ira para o final do heap*/
    while((line)&&(line->pin_count > 0)){
      /*necessario inserir por a funcao get_least_used decrementa
			 *o tamanho do heap,*/
			it_heap = insert_heap_lru(line->key,get_time(),&line);
			error = get_least_used(&it_heap);
      line = it_heap->line;
    }
  }

  free(it_heap);

  return line;
}

/*prototipo int get_line(LINE_T)
 * descricao: obtém uma linha de cache para alocar o bucket
 * parametros: LINE_T **line aponta para a a area de memoria alocada para linha de cache
 * retorno: um inteiro indicativo de falha ou sucesso*/
int get_line(LINE_T **line){

  error = 0;

  if(check_limit() == 0) //nao chegou no limite
    *line = CACHE+CACHE_INDX++;
  else{ //exec_policy executa lru
    *line = exec_policy();
		if(*line){
      //os elementos no heap não são removidos são sobrescritos
			//printf("[get_line] line->key:%d\n",(*line)->key);
      //printf("[get_line] line->data:%s\n",(*line)->data);
      if((*line)->dirty == 1){  //cache modificado e nao escrito em disco
		    error = enq_data_cache((*line)->key,*line,CURRENT_SERVER);
				if(error){
					fprintf(stderr, "Falha ao incluir Bucket, retirado do cache, na fila de atualizações!%s\n");
					return 1;
				}
			}

			//remove o bucket do cache
			error = rm_data_cache(*line);
    }
  }

  return ((*line)?0:1); //0 indica execucao sem erros
}

/*prototipo int put_data_cache(char*,LOCALITY*,unsigned char**)
 * descricao: adiciona um bucket no cache retornando um ponteiro para o buffer designado
 * parametros: key ->   idBucket do Bucket que esta sendo adicionado ao cache
 * 						 local ->  localizacao do bucket
 * 						 buffer-> espaco no cache designado para o bucket
 * retorno: hit (1 cache 0 sem cache)*/
int put_data_cache(char* key,LOCALITY_T *local,unsigned char** buffer){

	HEAPLRU_T *it_heap;
  LINE_T *line;
  NODELOC_T *new_node;

	line = NULL;
  new_node = NULL;

	/*para adicionar dados no cache
   *1) verificar se há espaço no heap
   *  1.1)se há espaco
   *     adiciona
   *  1.2)se não espaco
   *     execRLU
   *     adiciona
  */
	error = get_line(&line);
  if(!error){ //executou sem erros
    //printf("[put_data]encontrou linha\n");
    strcpy(line->key,key);
    line->pin_count += 1;
    line->local = *local;

		*buffer = line->data;

    //adiciona a ref da linha de cache no heap
    it_heap = insert_heap_lru(key,get_time(),&line);
		if(it_heap){
      //aloca memoria para o novo node
      new_node = (NODELOC_T *) xmalloc(sizeof(NODELOC_T));

      strcpy(new_node->key,key);
      new_node->it_heap = it_heap;
      new_node->next = NULL;

			error = insert_node_ht(new_node);
      if(error){
				fprintf(stderr,"[put_data_cache/cache.c] Erro ao inserir na hash table. A inserção do dado no cache falhou.\n");
				return 0;
      }

    }else{
			fprintf(stderr,"[put_data_cache/cache.c] Erro ao inserir no heap. A inserção do dado no cache falhou.\n");
			return 0;
		}
  }else{
		fprintf(stderr,"[put_data_cache/cache.c] Falha ao buscar linha de cache disponível!\n");
    return 0;
	}

	return 1;
}

/*prototipo: int get_data_cache(char* key, unsigned char* buffer)
 * descricao: localiza o cache referente ao bucket identificado por parametro
 * parametros: key -> id do Bucket que que sera pesquisado
 * 						 buffer -> aponta para o buffer do bucket no cache
 * retorno: (1 cache 0 sem cache)*/
int get_data_cache(char* key,unsigned char** buffer){

	HEAPLRU_T *it_heap;
  LINE_T *line;

  line = NULL;it_heap = NULL;

	it_heap = search_data(key);
  if(it_heap){
    /*printf("[get_data] it_heap->indx:%d\n",it_heap->indx);
    printf("[get_data] it_heap->key:%d\n",it_heap->key);
    if(!it_heap)
      printf("[get_data] falha no heap\n");*/

    line = it_heap->line;
    line->pin_count += 1;

    //atualiza o atributo tstamp base para o lru
		update_tstamp_lru(it_heap->indx,get_time());

		//buffer aponta para a linha disponivel
		*buffer = line->data;

		return 1;
  }
	return 0;
}

/*prototipo: int rm_data_cache(LINE_T*)
 * descricao: remove do cache o bucket identificado por get_least_used()
 * parametros: HEAPLRU_T* -> ponteiro para o item do heap
 * retorno: um inteiro indicativo de falha ou sucesso na remocao*/
int rm_data_cache(LINE_T *line){

  LOCALITY_T new_local;

	memset(line->data,'\0',LIMIT_SIZE_BUCKET);

	//inicializa linha de cache
	line->pin_count = 0;
	line->dirty = 0;
	//line->locked = 0;
	line->local = new_local;

	//remove o node da tabela hash
	error = remove_node_ht(line->key);

	return error;

}

/*prototipo: int check_locked(char *key,char *lock_owner, int *locked, char* lock_owner)
 * descricao: busca o valor do parametro locked no buffer_pool e compara o
 * 						lock_owner passado por parametro com o registrado no buffer manager
 * parametros: key -> chave para encontrar o buffer
 * 						 lock_owner -> srvName que fica como o lock_owner
 * retorno: um inteiro indicativo da obtencao do lock pelo lock_owner*/
int check_locked(char *key,char *requester, int *locked, char* lock_owner){

	/*TODO
	  repensar esta funcao*/

	NODELOC_T *node;
  LINE_T *line;
  HEAPLRU_T *it_heap;
  int state;

  line = NULL;node = NULL;

	node = search_node_ht(key);

  if(node){
		line = node->it_heap->line;
		if(line)
			*locked = line->locked;

		//memmove(lock_owner,frame->lock_owner,LCKOWNER_SIZE);
	}

	return 0;
}

/*prototipo: int find_dirty_buffers()
 * descricao: percorre o buffer pool em busca de buffers alterados (sujos)
 * 						os buffer sujos sao apontados na lista DIRTY_BUFFERS que e acessada
 * 						pela interface do sa
 * parametros:
 * retorno: qtd de buffers alterados */
int find_dirty_buffers(){

	LINE_T *line;
	DIRTYLINES_T *lst_node,*list;
	LOCALITY_T local;
	int amount;

	amount = 0;

	//dirty_list = DIRTY_BUFFERS;

	line = CACHE+0;
  while(line < (CACHE+CACHE_LIMIT)){
		local = line->local;
		if(strlen(local.srvName) > 0){
			if(line->dirty == 1){
				lst_node = (DIRTYLINES_T *) xmalloc(sizeof(DIRTYLINES_T));

				lst_node->local = line->local;
				lst_node->data = line->data;
				lst_node->next = NULL;

				list = DIRTY_LINES;
				if(list){
					while(list)
						list = list->next;

					list->next = lst_node;
				}else
					DIRTY_LINES = lst_node;

				amount++;
			}
		}
		line++;
	}

	return amount;
}

/*prototipo: int set_lock(char*,char*,int)
 * descricao: altera o valor do parametro locked no buffer_pool
 * parametros: key -> chave para encontrar o buffer
 * 						 lock -> valor que sera atribuido ao parametro
 * retorno: um inteiro indicativo de falha ou sucesso da operacao*/
int set_locked(char *key,char *lock_owner,int locked){

	NODELOC_T *node;
	LINE_T *line;

	line = NULL;node = NULL;

	node = search_node_ht(key);

	if(node){
		line = node->it_heap->line;
		line->locked = locked;
		//memmove(frame->lock_owner,lock_owner,LCKOWNER_SIZE);
	}

	return 0;
}

/*prototipo: int set_dirty(char*,int)
 * descricao: altera o valor do parametro dirty no buffer_pool
 * parametros: key -> chave para encontrar o buffer
 * 						 dirty -> valor que sera atribuido ao parametro
 * retorno: um inteiro indicativo de falha ou sucesso da operacao*/
int set_dirty(char *key, int dirty){

	LINE_T *line;
  HEAPLRU_T *it_heap;

	line = NULL;it_heap = NULL;

	it_heap = search_data(key);
  if(it_heap){
		line = it_heap->line;
		line->dirty = dirty;
	}

	return 0;
}
