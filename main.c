#include <stdio.h>
#include <stdlib.h>
#include <gfx.h>

/**
										T2 Simulador de Mem�ria RAM
										  Felipe da Silva Lima
												  :-D
									funciona mais ou menos a desaloca��o
**/

typedef struct BLOCOS_ALOCADOS  /** duplamente encadeada sem n� cabe�a */
{
    struct BLOCOS_ALOCADOS *ant;

    unsigned id;
    unsigned int ADD_INIT;   /* endereco inicial (os endere�os v�o de 0 at� M-1) */
    unsigned int ADD_FINISH; /* endereco final (os endere�os v�o de 0 at� M-1) */

    struct BLOCOS_ALOCADOS *prox;
}alocado;

typedef struct BLOCOS_DISPONIVEIS   /** simplesmente encadeada sem n� cabe�a */
{
    unsigned b;  /* tamanho de cada bloco */
    unsigned int ADD_INIT;   /* endereco inicial (os endere�os v�o de 0 at� M-1) */

    struct BLOCOS_DISPONIVEIS *prox;
}livre;

void init_livre(livre **no, unsigned int M)
{
    (*no)=malloc(sizeof(livre));
    (*no)->ADD_INIT=0;
    (*no)->b=M;
    (*no)->prox=NULL;
}

void busca_no_alocado(unsigned id,alocado *ptr,alocado **ant,alocado **pont)
{
    while(ptr!=NULL)
    {
        if(ptr->id<id)
        {
            *ant=ptr;
            ptr=ptr->prox;
        }
        else
        {
            if(ptr->id==id)
                *pont=ptr;

            ptr=NULL;
        }
    }
}
void busca_no_livre(unsigned b,livre *ptr,livre **ant,livre **pont)
{
    while(ptr!=NULL)
    {
        if(ptr->b<b)
        {
            *ant=ptr;
            ptr=ptr->prox;
        }
        else
        {
            if(ptr->b==b)
                *pont=ptr;

            ptr=NULL;
        }
    }
}
void insere_no_liv(livre **lista,livre **atual, livre **ant)
{
    if(*lista==NULL)
    {
        *lista=*atual;
        (*lista)->prox=NULL;
    }
    else
    {
        if((*ant)->b > (*atual)->b) /** inserir no primeiro no da lista*/
        {
            (*atual)->prox=*lista;
            (*lista)=*atual;
        }
        else
        {
            (*atual)->prox=(*ant)->prox;
            (*ant)->prox=(*atual);
        }
    }
}
void insere_no_aloc(int init,int finish,unsigned ID,alocado **alo)
{
	alocado *aux=*alo;
	alocado *novo=NULL;

	if(*alo==NULL)
	{
		novo=malloc(sizeof(alocado));
		novo->ADD_INIT=init;
		novo->ADD_FINISH=finish;
		novo->id=ID;
		novo->ant=NULL;
		novo->prox=NULL;
		*alo=novo;
	}
	else
	{
		while(aux->prox!=NULL)
			aux=aux->prox;

		novo=malloc(sizeof(alocado));
		novo->ADD_INIT=init;
		novo->ADD_FINISH=finish;
		novo->id=ID;
		novo->prox=aux->prox;
		aux->prox=novo;
		novo->ant=aux;
	}
}

void remove_no_liv(int *init,int *finish,unsigned int b,livre **liv,livre **atual,livre **prev, livre **aux)
{
	if(*atual==NULL) /**bloco com tamanho nao exato*/
	{
		if((*prev)==*liv && ((*liv)->prox==NULL || b < (*prev)->b)) /**Primeiro no que sera diminuido*/
		{
			*aux=*prev;

			*init=(*aux)->ADD_INIT;
			*finish=b+*init-1;

			(*liv)->b=(*liv)->b-b;
			(*liv)->ADD_INIT=(*liv)->ADD_INIT+b;
		}
		else
		{
			if(b > (*prev)->b)
			{
				*aux=(*prev)->prox;
				(*prev)->prox=(*aux)->prox;

				*init=(*aux)->ADD_INIT;
				*finish=*init+b-1;

				(*aux)->b=(*aux)->b-b;
				(*aux)->ADD_INIT=(*aux)->ADD_INIT+b;

				busca_no_livre((*aux)->b,*liv,prev,atual); /** busca para saber onde colocar o no de volta prev possui o no anterior aonde deve ser alocado*/
				insere_no_liv(liv,aux,prev);
			}
		}
	}
	else
	{
		if(*atual==*liv)  /**� o primeiro da lista*/
		{
			*init=(*atual)->ADD_INIT;
			*finish=(*atual)->b+*init-1;

			*liv=(*atual)->prox;
			free(*atual);
			*atual=NULL;
		}
		else /**N�o � o primeiro no da lista*/
		{
			*init=(*atual)->ADD_INIT;
			*finish=b+*init-1;

			(*prev)->prox=(*atual)->prox;
			free(*atual);
			*atual=NULL;
		}
	}
}

void remove_livre(livre **liv,livre **no,livre **prev)
{
	if(*liv!=NULL)
	{
		if(*no==*liv)
		{
			*liv=(*liv)->prox;
		}
		else
		{
			(*prev)->prox=(*no)->prox;
			free(*no);
			*no=NULL;
		}
	}
}

unsigned aloca_bloco(alocado **alo,livre **liv, unsigned ID)
{
    unsigned int b=0;
    alocado *ant=*alo,*pont=NULL;
    livre *prev=*liv, *atual=NULL, *aux=NULL;
    int init,finish;
    while(b<=0)
    {
        printf("Informe o tamanho que queres alocar (Maior que 0)\n");
        scanf("%u", &b);
    }

    busca_no_alocado(ID,*alo,&ant,&pont);  /** busca na lista alocada pra saber onde inserir usando o campo ID */
    busca_no_livre(b,*liv,&prev,&atual); /** busca na lista livre usando o campo b atual esta com o ponteiro para o bloco que sera usado para alocar prev pro anterior dele*/

    if(prev->prox==NULL && prev->b<b)/** o prev aponta para o ultimo elemento da lista, logo, nao tem bloco do tamanho que se deseja alocar*/
        printf("N�o � poss�vel alocar um bloco desse tamanho\n");
    else
    {
		remove_no_liv(&init,&finish,b,liv,&atual,&prev,&aux);
		insere_no_aloc(init,finish,ID,alo);
        ID++;

        printf("Mem�ria alocada com sucesso!\n\n");
    }
    return(ID);
}

int verif_adj(livre **liv,livre **aux) //verifica se o n� que ser� inserido � adjacente � algum n� da lista de mem�ria livre e retorna 1 se for. Caso n�o seja, retorna 0. aux tem o n� que foi removido dos alocados e se n�o for adjacente, ser� inserido diretamente de acordo com o seu tamanho
{
	livre *lista=*liv;
	livre *no;
	int flag=0;
	int retorno=0;

	if(lista!=NULL)
	{
		while(lista!=NULL && flag==0)
		{
			if(lista->ADD_INIT==(*aux)->b+(*aux)->ADD_INIT) /*caso o end inicial for igual ao final do n� removido*/
			{
				lista->b=lista->b+(*aux)->b;

				if( (*aux)->ADD_INIT<=lista->ADD_INIT)
					lista->ADD_INIT=(*aux)->ADD_INIT;

				flag=1;
				retorno=1;
			}
			if(lista->prox!=NULL)
			{
				if( lista->ADD_INIT+lista->b==(*aux)->ADD_INIT && (*aux)->ADD_INIT+(*aux)->b==lista->prox->ADD_INIT ) //caso seja inserido uma memoria entre duas e as duas s�o adjacentes � ela, uma pelo end final e outra pelo end inicial
				{
					lista->b=lista->b+(*aux)->b+lista->prox->b;
					no=lista->prox;
					remove_livre(&lista,&no,&lista);
					if( (*aux)->ADD_INIT<lista->ADD_INIT)
						lista->ADD_INIT=(*aux)->ADD_INIT;
					flag=1;
					retorno=1;
				}
				else
				{
					if(lista->prox->ADD_INIT==(*aux)->b+(*aux)->ADD_INIT) //caso meu segundo n� seja adjacente mas o primeiro n�o
					{
						lista->prox->b=lista->prox->b+(*aux)->b;
						if( (*aux)->ADD_INIT<lista->prox->ADD_INIT)
							lista->prox->ADD_INIT=(*aux)->ADD_INIT;
						flag=1;
						retorno=1;
					}
					if( lista->ADD_INIT+lista->b==(*aux)->ADD_INIT) //caso apenas meu primeiro n� seja adjacente e seja end final igual ao inicial do removido
					{
						lista->b=lista->b+(*aux)->b;
						if( (*aux)->ADD_INIT<=lista->ADD_INIT)
							lista->ADD_INIT=(*aux)->ADD_INIT;
						flag=1;
						retorno=1;
					}
				}
			}
			lista=lista->prox;
		}
	}
	return(retorno);
}

void remove_no_aloc_insere_no_liv(livre **liv, alocado **alo, alocado **ant, alocado **pont,livre **prev,livre **atual,livre **aux)
{
	if(*pont==*alo) /**O primeiro n� da lista de alocados ser� removido*/
	{
		(*aux)->ADD_INIT=(*pont)->ADD_INIT;
		(*aux)->b=(*pont)->ADD_FINISH-(*pont)->ADD_INIT+1;

		*alo=(*pont)->prox;
		free(*pont);
		*pont=NULL;
		if(verif_adj(liv,aux)==0)//caso n�o seja adjacente
		{
			busca_no_livre((*aux)->b,*liv,prev,atual);
			insere_no_liv(liv,aux,prev);
		}
	}
	else
	{
		if((*pont)->prox==NULL) /** Caso seja o �ltimo n� da lista de alocados*/
		{
			(*ant)->prox=(*pont)->prox;

			(*aux)->ADD_INIT=(*pont)->ADD_INIT;
			(*aux)->b=(*pont)->ADD_FINISH-(*pont)->ADD_INIT+1;

			free(*pont);
			*pont=NULL;
			if(verif_adj(liv,aux)==0)//caso n�o seja adjacente
			{
				busca_no_livre((*aux)->b,*liv,prev,atual);
				insere_no_liv(liv,aux,prev);
			}
		}
		else
		{
			(*ant)->prox=(*pont)->prox;
			(*pont)->prox->ant=*ant;

			(*aux)->ADD_INIT=(*pont)->ADD_INIT;
			(*aux)->b=(*pont)->ADD_FINISH-(*pont)->ADD_INIT+1;

			free(*pont);
			*pont=NULL;
			if(verif_adj(liv,aux)==0)//caso n�o seja adjacente
			{
				busca_no_livre((*aux)->b,*liv,prev,atual);
				insere_no_liv(liv,aux,prev);
			}
		}
	}
}

void desalocacao(alocado **alo,livre **liv)
{
    alocado *ant=*alo,*pont=NULL;
    livre *prev=*liv,*atual=NULL;
    livre *aux=malloc(sizeof(livre));

    unsigned ID=0;


    if(*alo==NULL)
        printf("Lista vazia. N�o h� o que desalocar\n");
    else
    {
        printf("Informe o ID do bloco que deseja desalocar\n");
        scanf("%u", &ID);
        busca_no_alocado(ID,*alo,&ant,&pont);

        if(pont==NULL)
            printf("Memoria n�o encontrada\n");
        else
        {
            remove_no_aloc_insere_no_liv(liv,alo,&ant,&pont,&prev,&atual,&aux);
            printf("Memoria desalocada com sucesso!\n");
        }
    }
}


void imprime_liv(livre *liv)
{
    printf("Livres: \n");
    if(liv==NULL)
        printf("N�o h� mem�ria livre\n");
    else
    {
        while(liv!=NULL)
        {
            printf("Endereco inicial: %d::", liv->ADD_INIT);
            printf("Tamanho: %u\n", liv->b);
            liv=liv->prox;
        }
    }
}

void imprime_alo(alocado *alo)
{
    printf("Alocados: \n");

    if(alo==NULL)
        printf("Toda a mem�ria est� livre\n");
    else
    {
        while(alo!=NULL)
        {
            printf("Endereco inicial: %d::", alo->ADD_INIT);
            printf("Endereco Final: %d::", alo->ADD_FINISH);
            printf("ID: %u\n", alo->id);
            alo=alo->prox;
        }
    }
}

float calcula_porcentagem(alocado *alo, int m)
{
	float total=0;

	while(alo!=NULL)
	{
		total=total+alo->ADD_FINISH-alo->ADD_INIT+1;
		alo=alo->prox;
	}
	total=(total/m)*100;
	return(total);
}

void freeALL(livre **liv, alocado **alo)
{
	livre *aux;
	alocado *xua;

	while(*liv!=NULL)
	{
		aux=*liv;
		*liv=(*liv)->prox;
		free(aux);
	}
	while(*alo!=NULL)
	{
		xua=*alo;
		*alo=(*alo)->prox;
		free(xua);
	}
	aux=NULL;
	xua=NULL;
}

void desenha(livre *liv, alocado *alo,float porcent)
{
	gfx_init(gfx_get_width(),gfx_get_height(),"RAM"); //pega o tamanho da tela

	char itoa[25]; /**string que armazenar� numeros usando a fun��o sprintf obs: <3 sprintf <3 muito kawai desu essa fun��o*/

	SDL_Event event;
	int quit=0;

	int x=50;
	int x1=150;
	int y=50;


	gfx_set_color(0,255,0);
	gfx_text(10,10,"Memoria livre");
	if(liv==NULL)
	{
		gfx_set_color(0,255,0);
		gfx_text(x,y,"SEM MEMORIA LIVRE");
	}
	while(liv!=NULL)
	{
		sprintf(itoa,"TAM: %u",liv->b);
		gfx_set_color(0,255,0);
		gfx_filled_rectangle(x,y,x1,2*y);
		gfx_set_color(0,0,0);
		gfx_text(x,y+10,itoa);

		liv=liv->prox;

		x=x1+10;
		x1=x1+110;

	}
	x=50;
	x1=150;
	y=150;
	gfx_set_color(255,0,0);
	gfx_text(10,130,"Memoria Ocupada");
	if(alo==NULL)
	{
		gfx_set_color(255,0,0);
		gfx_text(x,y,"SEM MEMORIA OCUPADA");
	}
	while(alo!=NULL)
	{
		sprintf(itoa,"ID:   %u",alo->id);
		gfx_set_color(255,0,0);
		gfx_filled_rectangle(x,y,x1,y+50);
		gfx_set_color(0,0,0);
		gfx_text(x,y+10,itoa);

		alo=alo->prox;

		x=x1+10;
		x1=x1+110;
	}
	gfx_set_color(255,0,0);
	sprintf(itoa,"%.2f%% alocada",porcent);
	gfx_text(250,450,itoa);
	gfx_paint();

	while(!quit)
	{
		while(SDL_PollEvent(&event))
		{
			if((event.type==SDL_QUIT)==1)
				quit=1;
			if((event.type==SDL_KEYDOWN)==1)
				quit=1;
		}
	}
	quit=0;
	gfx_clear();
	gfx_quit();
}

int main()
{
    alocado *alo=NULL;
    livre *liv=malloc(sizeof(livre));
    unsigned ID=1;
    int M=0;
    int opc,pause;
    float porcent=0;
    while(M<10)
    {
        printf("Informe o tamanho m�ximo da mem�ria (Maior ou igual que 10)\n");
        scanf("%d", &M);
    }
    init_livre(&liv,M);
    while(opc!=4)
    {
        system("clear");
        printf("Tamanho definido da mem�ria: %d\n\n", M);
        printf("1 - Alocar Mem�ria\n");
        printf("2 - Desalocar Mem�ria\n");
        printf("3 - Mapa de Mem�ria\n");
        printf("4 - SAIR\n");
        scanf("%d", &opc);
        if(opc==1)
        {
            if(liv==NULL)
                printf("N�o h� mem�ria livre\n");
            else
                ID=aloca_bloco(&alo,&liv,ID);

            //imprime_liv(liv);
            //imprime_alo(alo);
			printf("Digite um numero para sair\n");
            scanf("%d", &pause);
        }
        if(opc==2)
        {
            desalocacao(&alo,&liv);
            printf("Digite um numero para sair\n");
            scanf("%d", &pause);
            if(alo==NULL)
				ID=1;
        }
        if(opc==3)
        {
			porcent=calcula_porcentagem(alo,M);
			printf("Modo texto\n");
            imprime_liv(liv);
            printf("\n\n");
            imprime_alo(alo);
            printf("\n %.2f%% ocupada de memoria\n", porcent);
            desenha(liv,alo,porcent);
        }
    }

    freeALL(&liv,&alo); // desaloca todas listas quando sai do programa

	return 0;
}
