#include "include/hashmap.h"


int data_cmp_char(void const *lhs, void const *rhs){
    data_t const *const l = lhs;
    data_t const *const r = rhs;
    int res;

    res = strcmp(l->key,r->key);

    if(res < 0)
       return -1;
    else if(res > 0)
       return 1;

    return 0;

}

int data_cmp_int(void const *lhs, void const *rhs){
    data_t const *const l = lhs;
    data_t const *const r = rhs;

    if(l->key < r->key)
       return -1;
    else if(l->key > r->key)
       return 1;

    return 0;
}

int create_map(data_t **map,size_t size){

  *map = (data_t *)xmalloc(size);

}

int add_item_int(data_t *map,const char *key, int value,int limit){

  static int indx=0;

  if(indx < limit){
    (map+indx)->key = key;
    (map+indx)->value = value;

    indx++;
  }else{
     fprintf(stderr,"Limite do array atingido!!\n");
     return 1;
  }

}

/*int add_item_char(data_t **map,int key, const char *value,int limit){

  *map++;
  if(*map <= *map+limit){
    (*map)->key = key;
    (*map)->value = value;
  }else
    return 1;

}*/

const data_t *get_item_int(data_t *search,data_t *map,int limit){

   const data_t *res;

   res = bsearch(search,map,COUNT_ITMAP(sizeof(data_t),limit), SIZE_ITMAP(sizeof(data_t)), data_cmp_int);

   return res;
}


const data_t *get_item_char(data_t *search,data_t *map,int limit){

  const data_t *res;

  res = bsearch(search,map,COUNT_ITMAP(sizeof(data_t),limit), SIZE_ITMAP(sizeof(data_t)), data_cmp_char);

  return res;

}

/*void main(){

    data_t *hmap;
    int error;
    int limit = 4;
    data_t sKey;
    const data_t *res;

    error = create_map(&hmap,(sizeof(data_t) * limit));

    error = add_item_int(hmap,"SERVIDOR1", 10,limit);
    printf("key:%s value:%d\n",hmap->key,hmap->value);
    error = add_item_int(hmap,"SERVIDOR2", 20,limit);
    printf("key:%s value:%d\n",(hmap+1)->key,(hmap+1)->value);
    error = add_item_int(hmap,"SERVIDOR3", 30,limit);
    printf("key:%s value:%d\n",(hmap+2)->key,(hmap+2)->value);
    error = add_item_int(hmap,"SERVIDOR4", 40,limit);
    printf("key:%s value:%d\n",(hmap+3)->key,(hmap+3)->value);
    error = add_item_int(hmap,"SERVIDOR5", 50,limit);

    printf("buscar SERVIDOR4\n");

    sKey.key = "SERVIDOR4";
    res = get_item_char(&sKey,hmap,limit);

    if(res != NULL)
      printf("key:%s value:%d\n",res->key,res->value);
    else
      printf("item nao encontrado\n");

    free(hmap);

}*/
