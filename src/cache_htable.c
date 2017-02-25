#include "include/cache_htable.h"

#define GET_HTSIZE() (ceil(CACHE_LIMIT/2))

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

  fprintf(stderr,"valor do hash:%u\n",h);
  return h;
}

// A utility function to create an empty Hash of given capacity
void create_htable(){

  int pos;

  HTSIZE = GET_HTSIZE();
  //inicializa o indice
  LOCATOR = (LOCATOR_T *) xmalloc(sizeof(LOCATOR_T) * HTSIZE);

  //inicializacao da tabela hash
  for(pos = 0;pos < HTSIZE;pos++)
    (LOCATOR+pos)->node = NULL;

}

NODELOC_T *search_node_ht(char *key){

	LOCATOR_T *item;
  NODELOC_T *node;

  node = NULL;item = NULL;

	item = LOCATOR+get_hash(key);
  fprintf(stderr, "LOCATOR[0]:%s\n",item->node->key);
	if(item == NULL){
    fprintf(stderr, "item null\n");
    return NULL;
  }

  if(!item->node)
     return NULL;

  node = item->node;
  if(strcmp(node->key,key) == 0) //se as chaves coincidirem
  	return node;
  else if(node->next){
    node = node->next;
    while((node != NULL)&&(strcmp(node->key,key) != 0))
  		node = node->next;
  }

  return node;

}

int insert_node_ht(NODELOC_T *new_node){

  LOCATOR_T *item;
  NODELOC_T *node;

  //busca a posicao na tabela hash
  item = LOCATOR+get_hash(new_node->key);

  if(!item)  //por alguma razao nao encontrar a tab hash
    return 1;

  if(!item->node) //posicao livre no hash
    item->node = new_node;
  else{
    /*este loop sera utilizado em caso de colisao do hash
      percorre a lista encadeada ate encontrar o final da lista*/
    node = item->node;
    while((node->next != NULL))
      node = node->next;

    node->next = new_node;
  }

  return 0;

}

//verificar a necessidade desta funcao para a impl. no alocs
int remove_node_ht(char *key){

	LOCATOR_T *item;
  NODELOC_T *node;

  item = NULL;node = NULL;

  //busca a posicao na tabela hash
	item = LOCATOR+get_hash(key);

	if(!item)
	  return 1;

  node = item->node;
  if(!node)
    return 1;

  if(!node->next){  //se a lista escandeada tiver apenas um elemento
    free(node);
    item->node = NULL;
  }else{
    //se a lista escandeada tiver mais de um elemento
    strcpy(node->key,node->next->key);
    node->it_heap = node->next->it_heap;
    node->next = node->next->next;
  }

  return 0;

}
