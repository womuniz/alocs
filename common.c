#include "common.h"

int extractPair(BUCKET_T *buff_bucket,KEY_T key,PAIR_T *p_pair){
	
	int slot_pos,bit_pos,numKeys,slot_off,slot_length;
	char numKeysChr[4];
	char *value;
	BUCKET_T bucket;
	unsigned int pair_size;
  KEY_T slot_key;
  
	bucket = *buff_bucket;
	
	strncpy(numKeysChr,bucket,BYTES_LIMIT);
	numKeys = str_to_int(numKeysChr);
	
	//percorre o mapa de bits para localizar a chave
	slot_pos = 0;
	
	/*percorre o mapa de bits para localizar a chave
	 *se encontrar a chave a variavel slot_key e atualizada e bit_pos
	 *e atualizado com a posicao do bit no mapa*/
	bit_pos = find_slot_key(&bucket,numKeys,key,BYTES_LIMIT,slot_key);
	if(strcmp(slot_key,key) == 0) {
		//obtem posicao do slot a partir da posicao no mapa
		slot_pos = GET_SLOT_POS(bit_pos,numKeys);
		
		slot_off = str_to_int(OFF_POS_SLOT(bucket+slot_pos));
		slot_length = str_to_int(LEN_POS_SLOT(bucket+slot_pos));
		
		/*aloca memoria para o par, que e igual ao tamanho de key +
		 *o tamanho do slot + o caracter nulo*/
		pair_size = strlen(key)+slot_length;
		*p_pair = (PAIR_T ) malloc(pair_size);  
		if(p_pair == NULL){
			fprintf(stderr,"Memory error!\n");
			exit(1); 
		}
		
		//busca o valor no bucket
		strncpy(value,bucket+slot_off,slot_length);
		
		//monta o par chave-valor
		//copia a chave
		strcpy(*p_pair,key);
		//separador da chave e do valor
		strcat(*p_pair,"\n");
		//concantena o valor
		strcat(*p_pair,value);
		
		return 0;
	}
	
	return 0;
	
}

int find_slot_free(unsigned char **p_header,int numKeys,int len_value,int *offset){
	
	unsigned char *header;
	int pos,slot_pos,slot_length,find,prior_offset,prior_length;
	char bit;
	
	slot_length = pos = find = prior_offset = prior_length = 0;
	
	//header aponta para o endereco de memoria de header
	header = *p_header;
	
	bit = '1';
	//o marcador do slot comeca um posicao apos o fim do mapa de bits
	slot_pos = numKeys;
	
	while((!find) && (pos < numKeys)){
    //usa o valor de pos, depois incrementa
		bit = header[pos++];
    slot_pos = GET_SLOT_POS(pos,numKeys);
		
		/*verifica o bit: se igual alimenta offset
		 *se igual 0 obtem a posicao do slot e verifica e a condicao de parada foi aceita*/
		if(bit == '0'){
		  /*considera a possibilidade do slot livre ja ter sido ocupado uma vez
			 * se slot_length for maior que 0, se verdadeiro, verifica se o length
			 * e compativel com o length do value*/
			slot_length = str_to_int(LEN_POS_SLOT(header+slot_pos));
			if((slot_length == 0) || ((slot_length > 0) && (len_value <= slot_length)) ){
				//calcula o offset do slot livre
			  *offset = prior_offset+prior_length;
				find = 1;
			}
		}else{
			//guarda o offset do slot ocupado
			prior_offset = 	str_to_int(OFF_POS_SLOT(header+slot_pos));
			//guarda o length do slot ocupado
			prior_length = str_to_int(LEN_POS_SLOT(header+slot_pos));
		}
  }
	
	return pos;
}

int find_slot_key(unsigned char **p_header,int numKeys,KEY_T key,int pos,KEY_T slot_key){
	
	unsigned char *header;
	int slot_pos;
	char bit;
	
	header = *p_header;
	
	bit = '1';
	slot_pos = numKeys;
	while( (strcmp(slot_key,key) != 0) && (pos < numKeys) ){
		bit = header[pos++];
		
		if(bit == '1'){
			slot_pos =  GET_SLOT_POS(pos,numKeys);
		
			copy_buffer_key(slot_key,KEY_POS_SLOT(header+slot_pos));
		}	
	}
	
	return pos;
}

/*
* into_to_str: converte um valor inteiro de tamanho maximo de: 81450624
* para um char de 4 posicoes mais NULL: char[5]
* Permitindo assim que seja armazenado como string no ceph
*/
void int_to_str(unsigned char* str, int number){

	int i, pos, rem;
	
	memset(str,'\0',5);

	pos = 3;
	while (number >= BASE) {
		rem = number % BASE;
		str[pos] = rem;
		number = number / BASE;
		pos--;  
	}
	
	str[pos] = number;
		
	for(i = 0; i < 4; i++) 
  		str[i] = 32 + str[i];

}

/*
* str_to_int: converte um char[4] para um inteiro de 4 Bytes
*/
int str_to_int(unsigned char* str){

	unsigned int n;

	n =	((str[3] - 32) ) 
		+ ((str[2] - 32) * BASE)
		+ ((str[1] - 32) * BASE * BASE)
		+ ((str[0] - 32) * BASE * BASE * BASE);

	return n;
}

void copy_buffer_key(unsigned char *dest, unsigned char *src){
	
	strncpy(dest,src,KEY_SLOT_SIZE);
	
}
