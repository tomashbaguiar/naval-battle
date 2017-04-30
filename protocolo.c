#include "protocolo.h"

int**
recebeTabuleiro(const char *arquivo)
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
    while(fgets(linha, 6, entrada) != EOF)  {                                           // Lê linha do arquivo.
        int horizontal = linha[0] - 97;                                                 // Posição horizontal. (97 = 'a' em ascii)

        int vertical;                                                                   // Posição vertical.
        if((linha[3] == '\0') || (linha[3] == '\n'))
            vertical = linha[2] - 49;                                                   // Posição vertical, caso menor ou igual a 9.
        else
            vertical = ((linha[2] - 48) * TAMANHO) + (linha[3] - 48) - 1;               // Posição vertical, caso maior que 9.

        tabuleiro[horizontal][vertical] = 1;                                            // Preenche a posição recebida no tabuleiro.
    }

    return tabuleiro;
}

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



void
imprimeTabuleiro(int **tabuleiro)
{
    printf("    ");
    for(int i = 0; i < (2 * TAMANHO); i++)    {
        if(!(i % 2))                                                                    // Iterador com valor par.
            printf("%c", ((i / 2) + 97));                                               // Imprime as posições verticais.
        else
            printf(" ");
    }
    printf("\n    ");
    for(int i = 0; i < (2 * TAMANHO); i++)
        printf("-");
    for(int i = 0; i < TAMANHO; i++)    {
        printf("|");
        for(int j = 0; j < TAMANHO; j++)    {
            if(tabuleiro[i][j] == 1)
                printf("x ");
            else if(tabuleiro[i][j] == -1)
                printf("o ");
            else
                printf("  ");
        }
        printf("|\n");
    }
    printf("    ");
    for(int i = 0; i < (2 * TAMANHO); i++)
        printf("-");
    printf("\n");
}
