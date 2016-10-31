#include "include/common.h"

int ret;


/*prototipo void set_offset_slot(unsigned char**,int)
 * descricao: grava o valor do offset no slot
 * parametros: src ->ponteiro para a posicao do offset no slot
 * 						 offset ->valor que sera gravado*/
void set_offset_slot(unsigned char **src,unsigned int offset){

  char off_c[OFF_SLOT_SIZE+1];
	
	memset(off_c,'\0',OFF_SLOT_SIZE+1);
	
	ntochr(off_c,offset);
	
	memmove(OFF_POS_SLOT(*src),off_c,OFF_SLOT_SIZE);
	
}

/*prototipo int get_offset_slot(unsigned char*)
 * descricao: recupera o valor do offset no slot
 * parametros: src ->ponteiro para a posicao do offset no slot
 * retorno: um inteiro correspondente ao offset*/
unsigned int get_offset_slot(unsigned char *src){

	int offset;
	char off_c[OFF_SLOT_SIZE+1];
  
	offset = 0;
	memset(off_c,'\0',OFF_SLOT_SIZE+1);
	
	memmove(off_c,OFF_POS_SLOT(src),OFF_SLOT_SIZE);
	if(strlen(off_c) > 0)
		offset = chrton(off_c);
	
	return offset;
}

/*prototipo void set_len_slot(unsigned char**,int)
 * descricao: grava o valor do length no slot
 * parametros: src ->ponteiro para a posicao do length no slot
 * 						 length ->valor que sera gravado*/
void set_len_slot(unsigned char **src,unsigned int length){

  char len_c[LEN_SLOT_SIZE+1];
	
	memset(len_c,'\0',LEN_SLOT_SIZE+1);
	
	ntochr(len_c,length);
  
	memmove(LEN_POS_SLOT(*src),len_c,LEN_SLOT_SIZE);
}

/*prototipo int get_len_slot(unsigned char*)
 * descricao: recupera o valor do length no slot
 * parametros: src ->ponteiro para a posicao do length no slot
 * retorno: um inteiro correspondente ao length*/
unsigned int get_len_slot(unsigned char *src){

  int len;
	char len_c[LEN_SLOT_SIZE+1];
	
	len = 0;
	memset(len_c,'\0',LEN_SLOT_SIZE+1);
	
	memmove(len_c,LEN_POS_SLOT(src),LEN_SLOT_SIZE);
	if(strlen(len_c) > 0)
		len = chrton(len_c);
	
	return len;
}

/*prototipo void set_key_slot(unsigned char**,KEY_T)
 * descricao: grava o valor da chave no slot
 * parametros: src ->ponteiro para a posicao da chave no slot
 * 						 key ->chave que sera gravado*/
void set_key_slot(unsigned char **src,KEY_T key){

  unsigned char key_c[KEY_SLOT_SIZE];
	size_t len_keyc;
	
	memset(key_c,'\0',KEY_SLOT_SIZE);
	
	itoc64(key_c,KEY_SLOT_SIZE,key);
	//a funcao itoc64 nao completa a mascara toda 
	len_keyc = strlen(key_c);
	memset(key_c+len_keyc,' ',KEY_SLOT_SIZE - len_keyc);
	
	memmove(KEY_POS_SLOT(*src),key_c,KEY_SLOT_SIZE);
}

/*prototipo KEY_T get_key_slot(unsigned char*)
 * descricao: recupera o valor da chave no slot
 * parametros: src ->ponteiro para a posicao da chave no slot
 * retorno: um inteiro correspondente a chave*/
KEY_T get_key_slot(unsigned char *src){

  KEY_T key;
	unsigned char key_c[KEY_SLOT_SIZE];
	
	memset(key_c,'\0',KEY_SLOT_SIZE);
	
	memmove(key_c,KEY_POS_SLOT(src),KEY_SLOT_SIZE);
	key = ctoi(key_c,0);
	
	return key;
}

void get_numkey_bucket(BUCKET_T *buff_bucket, unsigned char *numKeysChr){
	memcpy(numKeysChr,*buff_bucket+LOCK_FLAG_SIZE,BYTES_LIMIT);
}

/*prototipo void write_slot(unsigned char**,unsigned int,unsigned int,KEY_T)
 * descricao: atualiza o slot no header
 * parametros: buffer ->ponteiro para a posicao inicial do slot no buffer
 * 						 offset -> offset que sera gravado
 * 						 length -> length que sera gravado
 * 						 key -> valor da chave que sera gravada	
 * retorno: void*/
void write_slot(unsigned char **buffer,unsigned int slot_pos, unsigned int offset,unsigned int length,KEY_T key){
	
  unsigned char *slot;
	size_t len_slot;
	
	//aloca memoria para o slot
	len_slot = sizeof(unsigned char) * SLOT_SIZE;
	slot = (unsigned char*) xmalloc(len_slot);
	
	memset(slot,'\0',len_slot);
	
	//insere no slot o offset para o valor 
	set_offset_slot(&slot,offset);
	//insere no slot o tamanho do valor
	set_len_slot(&slot,length);
	
	//insere no slot a chave que identifica o par
	set_key_slot(&slot,key);
	
	memmove(*buffer+slot_pos,slot,SLOT_SIZE);
	
	free(slot);
}


/*prototipo int find_slot_key(unsigned char**,int,KEY_T)
 * descricao: busca a chave passada por parametro no slots
 * parametros: p_buffer ->ponteiro para a posicao inicial 
 * 												do mapa de bits no header
 *             numKeys -> qtd max de chaves no bucket, para limitar a busca no mapa de bits
 * 						 key -> chave requisitada	
 * retorno: a posicao do slot no mapa de bits*/
int find_slot_key(unsigned char **buffer,int numKeys,KEY_T key){
	
	unsigned char *header;
	int slot_pos,pos,find;
	char bit;
	KEY_T slot_key;
	
	bit = '1';
	//posiciona o ponteiro do header, na posicao inicial do mapa de bits
	header = *buffer;
	/*pos indica a posicao no mapa de bits que sera retornado
	 *comeca em offset -1 pois na iteracao pos e incrementado 
	  antes de utilizar o valor para que seja retornada a posicao exata*/
	pos = -1;
	find = 0;
	slot_pos = numKeys;
	//(pos < numKeys) evita que o loop extrapole o mapa de bits
	while((!find) && (pos < numKeys)){
		bit = *(header+(++pos));
		if(bit == '1'){
			slot_pos =  GET_SLOT_POS(pos,numKeys);
			slot_key = get_key_slot(header+slot_pos);
			
			find = (key == slot_key);
		}
	}
	
	return ((find == 1) ? pos : -1);
}

/*prototipo int extractPair(BUCKET_T *buff_bucket,KEY_T key,PAIR_T *p_pair)
 * descricao: busca a chave passada por parametro no slots
 * parametros: p_buffer ->ponteiro para a posicao inicial 
 * 												do mapa de bits no header
 *             numKeys -> qtd max de chaves no bucket, para limitar a busca no mapa de bits
 * 						 key -> chave requisitada	
 * retorno: a posicao do slot no mapa de bits*/
int extract_pair(BUCKET_T *buff_bucket,KEY_T key,PAIR_T *p_pair){
	
	int slot_pos,bit_pos,numKeys;
	unsigned char numKeysChr[BYTES_LIMIT+1];
	unsigned char key_c[KEY_SLOT_SIZE];
  unsigned char *value,*mbits,*p_body;
	unsigned int slot_length,slot_off,hsize;
	size_t len_value,len_pair;
	
	slot_pos = 0;
	bit_pos = -1;
	
	/*busca a qtd. max de chaves no bucket para 
	delimitar as leituras no mapa de bits*/
	//memcpy(numKeysChr,*buff_bucket+LOCK_FLAG_SIZE,BYTES_LIMIT);
	get_numkey_bucket(buff_bucket,numKeysChr);
	numKeys = chrton(numKeysChr);
	
	hsize = HEADER_SIZE(numKeys);
	
	/*percorre o mapa de bits para localizar a chave
	 *se encontrar a chave, bit_pos e atualizado com a posicao do bit no mapa
	 *header + BYTES_LIMIT indica para desconsiderar os BYTES_LIMIT iniciais do header*/
	//p_bitmap aponta para a posicao inicial do bitmap 
	mbits = *buff_bucket+MBITS_OFFSET;
	bit_pos = find_slot_key(&mbits,numKeys,key);
	if(bit_pos >= 0){
		//obtem posicao do slot a partir da posicao no mapa
		slot_pos = GET_SLOT_POS(bit_pos,numKeys);
		
		slot_off = get_offset_slot(mbits+slot_pos);
		slot_length = get_len_slot(mbits+slot_pos);
		
		//converte o valor da chave e inteiro para char
		itoc64(key_c,KEY_SLOT_SIZE,key);
		
		/*aloca memoria para o par, que e igual ao tamanho de key + separador +
		 *o tamanho do slot + o caracter nulo*/
		len_pair = KEY_SLOT_SIZE + slot_length + 1;
		*p_pair = (PAIR_T) xmalloc(len_pair);
		//inicializa a memoria alocada 
		memset(*p_pair,'\0',len_pair);
		
		//aloca memoria para escrever o valor 
		len_value = (sizeof(unsigned char) * slot_length) + 1;
		value = (unsigned char *) xmalloc(len_value);
		//inicializa a memoria alocada
		memset(value,'\0',len_value);

		//aponta para a posicao do valor do slot
		p_body = *buff_bucket+slot_off;
		//transfere o valor do bucket para value
		memmove(value,p_body,slot_length);
		
		//monta o par chave-valor
		snprintf(*p_pair,len_pair,"%s\n%s",key_c,value);
		
		free(value);
		 
		return 0;
	}
		
	return 1;
}

/*prototipo int find_slot_free(unsigned char**,int,int,int*)
 * descricao: busca um slot livre no bucket 
 * parametros: p_buffer ->ponteiro para a posicao inicial 
 * 												do mapa de bits no header
 *             numKeys -> qtd max de chaves no bucket, para limitar a busca no mapa de bits
 * 						 len_value -> length no value necessário para verificar 
 * 													se o tamanho do valor e compativel com o slot
 * 						 offset -> retorna o offset para insercao do valor	
 * retorno: a posicao do slot no mapa de bits*/
int find_slot_free(unsigned char **buffer,int numKeys,int len_value,int *offset){
	
	unsigned char *header;
	int pos,slot_pos,find;
	unsigned int slot_length,prior_offset,prior_length;
	char bit;
	
	slot_length = find = prior_length = 0;
	
	//p_header aponta para o endereco de memoria de header
	header = *buffer;
	
	bit = '1';
	pos = -1;
	slot_pos = numKeys; //o marcador do slot comeca um posicao apos o fim do mapa de bits
	prior_offset = *offset;
	
	while((!find) && (pos < numKeys)){
    //usa o valor de pos, depois incrementa
		bit = *(header+(++pos));
		slot_pos = GET_SLOT_POS(pos,numKeys);
		
		/*verifica o bit: se igual alimenta offset
		 *se igual 0 obtem a posicao do slot e verifica e a condicao de parada foi aceita*/
		if(bit == '0'){
		  /*considera a possibilidade do slot livre ja ter sido ocupado uma vez
			 * se slot_length for maior que 0, se verdadeiro, verifica se o length
			 * e compativel com o length do value*/
			slot_length = get_len_slot(header+slot_pos);
			if((slot_length == 0) || ((slot_length > 0) && (len_value <= slot_length)) ){
				//calcula o offset do slot livre
				*offset = prior_offset+prior_length;
				find = 1;
			}
		}else{
			//guarda o offset do slot ocupado
			prior_offset = get_offset_slot(header+slot_pos);
			//guarda o length do slot ocupado
			prior_length = get_len_slot(header+slot_pos);
		}
  }
	
	return pos;
}

/*prototipo int write_pair_buffer(BUCKET_T *buffer,unsigned int numKeys,KEY_T key,char *value)
 * descricao: busca um slot livre no bucket 
 * parametros: buffer ->buffer que sera modificado
 *             numKeys -> qtd max de chaves no bucket, para limitar a busca no mapa de bits
 * 						 key -> chave que sera escrita
 * 						 value -> valor que sera escrito
 * retorno: inteiro indicativo de sucesso ou falha da operacao*/
int write_pair_buffer(BUCKET_T *buffer,unsigned int numKeys,KEY_T key,char *value){
	
	size_t len_value,hsize,len_head;
	unsigned int offset,slot_pos;
	unsigned char *mbits,*slot;
	int bit_pos;
	
	offset = slot_pos = 0;
	
	// Define o tamanho do header como (qtd * tamanho da tripla) + qtd(mapabit) + NULL
	hsize = HEADER_SIZE(numKeys);
	//o offset comeca uma posicao apos o header
	offset = hsize;
	
	//length do valor do par 	
  len_value = strlen(value);
	
	//mbits aponta para o inicio do mapa de bits
	mbits = *buffer+MBITS_OFFSET;
	/*find_slot_free retorna a posicao no mapa de bits correspondente ao slot livre
	 *offset sera atualizado com o offset do slot livre*/
	bit_pos = find_slot_free(&mbits,numKeys,len_value,&offset);
	if(offset > 0){ 
		//obtem posicao do slot a partir da posicao no mapa
		slot_pos = GET_SLOT_POS(bit_pos,numKeys);
		
		/*se encontrou um slot livre (slot_pos > 0) e o tamanho do valor não
		 *excede o valor maximo entao insere */ 
		if( (LIMIT_SIZE_BUCKET - (offset + len_value)) >= 0 ){
			write_slot(&mbits,slot_pos,offset,len_value,key);
			
			//altera o bit correspondente ao slot no mapa para ocupado
			*(mbits+bit_pos) = '1';
		
			/*apos a alteracao do slot o tamanho do header esta sendo modificado
			  a linha abaixo preenche o restante do header com espacos em branco
			  o strlen do header sempre tem que ser igual a hsize senao disposiona os dados no bucket*/ 
			
			/*atualiza o header
				*o valor sera escrito no bucket somente se a atualizacao do header for bem sucedida*/
			//escreve o valor no buffer
			memmove(*buffer+offset,value,len_value);
		}
	}else{
		//se nao -> identificar se e bucket cheio ou limite de tamanho
		if(bit_pos < 0)
			fprintf(stderr,"[write_pair_buffer/iceph.c] Bucket Cheio!\n");
		else if( ((LIMIT_SIZE_BUCKET - (offset + len_value)) < 0 ) )
			fprintf(stderr,"[write_pair_buffer/iceph.c] O tamanho do par chave-valor excede o disponível do bucket!\n");
		
		return 1;
	}	
	
	return 0;
}

/*prototipo int remove_pair_buffer(BUCKET_T *buffer,unsigned int numKeys,KEY_T key)
 * descricao: remove um par chave-valor do bucket no buffer 
 * parametros: buffer ->buffer que sera modificado
 *             numKeys -> qtd max de chaves no bucket, para limitar a busca no mapa de bits
 * 						 key -> chave que sera escrita
 * retorno: inteiro indicativo de sucesso ou falha da operacao*/
int remove_pair_buffer(BUCKET_T *buffer,unsigned int numKeys,KEY_T key){
	
	int bit_pos;
	unsigned char *mbits,*value;
	size_t hsize,len_value;
	unsigned int offset,slot_pos;

	offset = slot_pos = 0;

	bit_pos = 0;
	
	// Define o tamanho do header como (qtd * tamanho da tripla) + qtd(mapabit) + NULL
	hsize = HEADER_SIZE(numKeys);
	
	//header aponta para o inicio do mapa de bits
	mbits = *buffer+MBITS_OFFSET; 
	
	/*percorre o mapa de bits para localizar a chave
	 *se encontrar a chave, bit_pos e atualizado com a posicao do bit no mapa*/
	bit_pos = find_slot_key(&mbits,numKeys,key);
	if(bit_pos >= 0) {
		//obtem posicao do slot a partir da posicao no mapa
		slot_pos = GET_SLOT_POS(bit_pos,numKeys);
		
		//obtem o length do slot
		len_value = get_len_slot(mbits+slot_pos);
		//obtem o offset do slot
		offset = get_offset_slot(mbits+slot_pos);
		
		//muda o bit de ocupado para livre, liberando o slot
	  *(mbits+bit_pos) = '0';
		
		//aloca memoria para value
		value = (unsigned char *) xmalloc(sizeof(unsigned char) * len_value);
	
		//atualiza o valor do slot
		memset(value,' ',len_value);
		
		//escreve o valor no buffer
		memmove(*buffer+offset,value,len_value);
			
		free(value);
	}else
	  fprintf(stdout,"[remove_pair/iceph.c] Chave não encontrada\n");

}