#include "include/cache_heap.h"

void swap(HEAPLRU_T *item1, HEAPLRU_T *item2);
void pq_heap_inc_key(int indx,int key);
void min_heapify(int indx);


int HSIZE = 0;

//TODO:lembrar que na troca tem a atualizacao da  estrutura de dados do heap
/* fazer duas consultas na tab. hash para buscar os items correspondentes aos itens movimentados
   no heap, pois a ref. deles deverá ser atualizada na est. de dados do hash*/
void swap(HEAPLRU_T *item1, HEAPLRU_T *item2){

    printf("[swap] item->key:%d\n",item1->key);
    unsigned int tstamp;
    char* key;
    NODELOC_T *node1,*node2;
    LINE_T *line;

    node1 = node2 = NULL;

    /*o valor do atributo indx do heap nao deve ser modificado
     *ele é utilizado nas operações de atualizacao e para manter a propriedade do heap minimo*/

    //busca a ref do item1 na ht
    node1 = search_node_ht(item1->key);
    //busca a ref do item2 na ht
    node2 = search_node_ht(item2->key);

    //copia valores para variaveis temporarias
    strcpy(key,item1->key);
    tstamp = item1->tstamp;
    line = item1->line;

    //item1 recebe valores de item 2
    strcpy(item1->key,item2->key);
    item1->tstamp = item2->tstamp;
    item1->line = item2->line;
    //troca a ref do lru do item1 na ht
    node1->it_heap = item2;

    //item2 recebe valores de item1
    strcpy(item2->key,key);
    item2->tstamp = tstamp;
    item2->line = line;
    //troca a ref do lru do item1 na ht
    node2->it_heap = item1;
}

/*void print_heap(){
    int pos;

    for(pos = 0;pos < CACHE_LIMIT;pos++)
        printf("indx:%d tstamp:%lu - key:%d\n",pos,HLRU[pos].tstamp,HLRU[pos].key);
}*/

//cria o heap com os frames
void create_heap(){

  HEAPLRU_T *it_heap,new_itheap;
  int pos;

  //inicializa o heap para o lru
  HLRU = (HEAPLRU_T *) xmalloc(sizeof(HEAPLRU_T) * CACHE_LIMIT);

  for(pos = 0;pos < CACHE_LIMIT;pos++){
    it_heap = HLRU+pos;

    *it_heap = new_itheap;
  }
}

void min_heapify(int indx){

  int left,right,lwest;

  //este processo demora O(lg n)
  /*na ordenacao o valor a ser trocado flutua de cima para baixo
  *ao contrario da insercao*/
  left = HLEFT(indx,HSIZE);
  right = HRIG(indx,HSIZE);

  lwest = indx;

  if(HLRU[left].tstamp < HLRU[indx].tstamp)
	lwest = left;

  if(HLRU[right].tstamp < HLRU[lwest].tstamp)
    lwest = right;

  if(lwest != indx){
    swap(&HLRU[lwest], &HLRU[indx]);
    min_heapify(lwest);
  }
}

int get_least_used(HEAPLRU_T **it_heap){

  HEAPLRU_T *lused;

  lused = *it_heap;

  if(HSIZE == 0){
    printf("heap vazio!\n"); //ajustar esta mensagem para o padrao do alocs
    return 1;
  }

  *lused = *(HLRU+0);
  strcpy(HLRU[0].key,HLRU[HSIZE - 1].key);
  HLRU[0].tstamp = HLRU[HSIZE - 1].tstamp;
  HLRU[0].line = HLRU[HSIZE - 1].line;

  --HSIZE;

  min_heapify(0);

  fprintf(stdout,"[get_least_used]tstamp mim:%d\n",lused->tstamp);
  return 0;
}

/*a atualizao tera custo de 2+lg(N) lg(N) corresponde ao ajuste da propriedade heap minimo
  e o 2 e equivalente a duas consultas no hash para atualizacao da estrutura de dados do hash na troca de posicoes
  para manter a propriedade do heap minimo*/
void update_tstamp_lru(int indx,unsigned int tstamp){

  int left;

  left = HLEFT(indx,HSIZE);

  HLRU[indx].tstamp = tstamp;

  printf("[update tstamp] HLRU[indx].tstamp:%lu,indx:%d\n",HLRU[indx].tstamp,indx); //ajustar esta mensagem para o padrao do alocs

  /*quando o indice e zero o parent e ele mesmo
    se o tstamp for maior que o tstamp do el. filho
    a condicao (HLRU[HPAR(indx)] > HLRU[indx]) nao ajusta
    a ordem*/
  if(indx > 0){
    while((HLRU[HPAR(indx)].tstamp > HLRU[indx].tstamp)){
      swap(&HLRU[HPAR(indx)],&HLRU[indx]);
      indx = HPAR(indx);
    }
  }else{
    //para heap minimo
    if(HLRU[left].tstamp < HLRU[indx].tstamp)
       min_heapify(0);
  }

}

HEAPLRU_T *insert_heap_lru(char* key,unsigned int tstamp,LINE_T **line){

  int indx = HSIZE++;

  HLRU[indx].indx = indx;
  strcpy(HLRU[indx].key,key);
  HLRU[indx].line = *line;

  /*a prop. heap minimo e regida pelo tstamp
   *conforme o tstamp a posicao da ref do cache no heap e ajustada*/
  update_tstamp_lru(indx,tstamp);

  printf("[insert_heap] HLRU[indx].key:%s HLRU[indx].tstamp:%lu indx:%d\n",HLRU[indx].key,HLRU[indx].tstamp,indx); //ajustar esta mensagem para o padrao do alocs

  return &HLRU[indx];
}
