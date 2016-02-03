
#include "alocs.h"


int main (int argc, char *argv[]) {

  PAIR_T *pair;
	int opcao, op;
	unsigned int Qtd;
	static char opcoes[] = "gprdch"; 

	char *chave_par = argv[2];
	char *valor_par = argv[3]; // conteudo (valor)
	char *nome_bucket = argv[4];  // objeto = bucket (key)
	char *diretorio = argv[5];  // pool = diretorio (chave_bucket)
	char *servidor = argv[6];  // servidor onde ficara o bucket/chave
	char *quantia;
		
	opterr = 0;  //	trata mensagens de erro
		
	printf("========================================\n\n");
	
	do{
		printf("\t\t\nMenu Alocs Application Client\n");
		printf("p - put_pair Operation\n");
		printf("r - remove_pair Operation\n");
		printf("g - get_pair Operation\n");
		printf("d - drop_bucket Operation\n");
		printf("c - create_bucket Operation\n");
		printf("h - help\n");
		printf("e - exit\n");
				
		init_rados;
		
		op = getopt(argc, argv, opcoes);

		opcao = verifica_opcao(argc, op);	

		switch(opcao) {

			case 'p': {
				put_pair("2015","C COMPLETO E TOTAL"); 
				
				break;
			}
			case 'r': {
				rem_pair("2015");
				
				break;
			}
			case 'g': {
				get_pair("2015",&pair);
				
				printf("pair:%s",pair);
				
				free(pair);
				
				break;
			}
			case 'd': {
				
				drop_bucket("CCOMPUTACAO");
				
				break;
			}
			case 'c': {
				retOper = create_bucket(diretorio,nome_bucket,"2000","2020");
				
				break;
			}
			case 'h': {
				printf("\n #Ajuda: \n");
				uso_correto();
				break;
			}
		  	default: {
		  		printf("\nUso correto:\n");
				uso_correto();		
			}
		}

		fin_rados();
		

		printf("Executado com SUCESSO! \n");
		printf("\n========================================\n");
	} while(opcao='e');	

	return 0;
}
