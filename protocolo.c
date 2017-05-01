#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <netdb.h>

#include "protocolo.h"

int**
recebeTabuleiro(char *arquivo)
{
    //  Abre o arquivo em modo leitura e verifica sucesso   //
    FILE *entrada = fopen(arquivo, "r");
    if(entrada == NULL) {
        perror("Arquivo.");                                                             // Em caso de erro, imprime o erro.
        return NULL;                                                                    // Retorna matriz nula.
    }

    //  Aloca espaço para o tabuleiro   //
    int **tabuleiro = (int **) malloc(TAMANHO * sizeof(int *));
    for(int i = 0; i < TAMANHO; i++)
        tabuleiro[i] = (int *) malloc(TAMANHO * sizeof(int));

    //  "Esvazia" todas as posições do tabuleiro
    for(int i = 0; i < TAMANHO; i++)
        for(int j = 0; j < TAMANHO; j++)
            tabuleiro[i][j] = 0;

    //  Itera dentro do arquivo e coloca as peças nas posições  //
    char linha[4] = {0};                                                                // Recebe a linha.
    while(fgets(linha, 6, entrada) != NULL)  {                                          // Lê linha do arquivo.
        int horizontal, vertical;
        trataCoordenadas(linha, &horizontal, &vertical);                                // Trata as coordenadas recebidas.

        tabuleiro[horizontal][vertical] = 1;                                            // Preenche a posição recebida no tabuleiro.
    }

    return tabuleiro;
}

/*
int**
geraTabuleiro()
{
    //  Aloca espaço para o tabuleiro   //
    int **tabuleiro = (int **) malloc(TAMANHO * sizeof(int *));
    for(int i = 0; i < TAMANHO; i++)
        tabuleiro[i] = (int *) malloc(TAMANHO * sizeof(int));

    //  "Esvazia" todas as posições do tabuleiro
    for(int i = 0; i < TAMANHO; i++)
        for(int j = 0; j < TAMANHO; j++)
            tabuleiro[i][j] = 0;

    //////////////////////////////////////////////
    //
    //
    //
    //////////////////////////////////////////////
    
    srand(time(NULL));

    int pos = rand() % 1;                                                               // 0 = horizontal, 1 = vertical.


}///////////////////////////////////////////////////////////////////////
*/
    
void
imprimeTabuleiro(int **tabuleiro)
{
    printf("   ");
    for(int i = 0; i < (2 * TAMANHO); i++)    {
        if(!(i % 2))                                                                    // Iterador com valor par.
            printf("%c", ((i / 2) + 97));                                               // Imprime as posições verticais.
        else
            printf(" ");
    }
    printf("\n   ");
    for(int i = 0; i < ((2 * TAMANHO) - 1); i++)
        printf("-");
    printf("\n");
    for(int i = 0; i < TAMANHO; i++)    {
        printf("%2d|", (i + 1));
        for(int j = 0; j < TAMANHO; j++)    {
            if(tabuleiro[i][j] == 1)
                printf("x ");
            else if(tabuleiro[i][j] == -1)
                printf("o ");
            else
                printf("  ");
        }
        printf("\b|\n");
    }
    printf("   ");
    for(int i = 0; i < ((2 * TAMANHO) - 1); i++)
        printf("-");
    printf("\n");
}

int
marcaAtaque(int **tabuleiro, int *efetividade, int horizontal, int vertical)
{
    //  Verifica se é recebimento de ataque //
    if(*efetividade == -1)  {
        //  Verifica se existe peça nas coordenadas //
        if(tabuleiro[horizontal][vertical] == 1)   {
            *efetividade = 1;                                                           // Acertou o ataque.
            tabuleiro[horizontal][vertical] = -1;                                       // Marca como visitada e sem parte de navio.
        }
        else
            *efetividade = 0;                                                           // Não acertou o ataque.
    } 

    //  Verifica se é envio de ataque bem sucedido  //
    else if(*efetividade && (tabuleiro[horizontal][vertical] == 0))
        tabuleiro[horizontal][vertical] = 1;                                            // Marca como visitada e acertada.
    //  Verifica se é envio de ataque mal sucedido  //
    else if(!(*efetividade) && (tabuleiro[horizontal][vertical] == 0))    
        tabuleiro[horizontal][vertical] = -1;

    //  Retorna o número de peças restantes //
    int restantes = 30;
    for(int i = 0; i < TAMANHO; i++)
        for(int j = 0; j < TAMANHO; j++)
            if(tabuleiro[i][j])
                restantes--;

    return restantes;
}

void 
trataCoordenadas(char *linha, int *horizontal, int *vertical)
{
    *vertical = linha[0] - 97;                                                          // Posição horizontal. (97 = 'a' em ascii)

    if((linha[3] == '\0') || (linha[3] == '\n'))
        *horizontal = linha[2] - 49;                                                    // Posição vertical, caso menor ou igual a 9.
    else
        *horizontal = ((linha[2] - 48) * TAMANHO) + (linha[3] - 48) - 1;                // Posição vertical, caso maior que 9.
}

int 
hostname_to_ip(char *hostname, char *ip)
{
    struct addrinfo hints, *servinfo, *p;
    struct sockaddr_in *h;

    int rv;

    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;

    if((rv = getaddrinfo(hostname, "http", &hints, &servinfo)) != 0)
    {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
        return 1;
    }

    for(p = servinfo; p != NULL; p = p->ai_next)
    {
        h = (struct sockaddr_in *) p->ai_addr;
        strcpy(ip, inet_ntoa(h->sin_addr));
    }

    freeaddrinfo(servinfo);

    return 0;

}

