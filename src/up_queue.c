#include "include/storage_types.h"

int enq_data_cache(char *idBucket,LINE_T *line,char *execServer){

  QUEUE_T *lst_node,*list;

  lst_node = (QUEUE_T *) xmalloc(sizeof(QUEUE_T));

  strcpy(lst_node->idBucket,line->key);
  lst_node->local = line->local;
  lst_node->sourcev = line->sourcev;
  lst_node->cachev = line->cachev;
  //lst_node->idTx = line->idTx;
  strcpy(lst_node->execServer,execServer);
  memmove(lst_node->data,line->data,strlen(line->data));

  lst_node->next = NULL;

  list = UPQUEUE;
  if(list){
    while(list)
      list = list->next;

    list->next = lst_node;
  }else
    UPQUEUE = lst_node;

  return 0;
}
