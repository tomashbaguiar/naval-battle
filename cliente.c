#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>

#include "protocolo.h"

#define ARQUIVO "arquivo.txt"

//  ./cliente <ip/nome> <porta>

int
main(int argc, char **argv)
{
    //  Testa os argumentos de entrada  //
    if(argc != 3)   {
        fprintf(stderr, "Utilização:\n\t./cliente <ip/nome> <porta>\n");
        return 1;
    }

    int porta = atoi(argv[2]);                                                          // Porta do servidor.
    
    //  Cria, e verifica, socket para se comunicar com o servidor    //
    int sock = socket(AF_INET6, SOCK_STREAM, 0);                                        // Socket criado como IPv6 e TCP.
    if(sock == -1)    {
        perror("socket()");                                                             // Imprime erro na criação do socket.
        return 1;
    }

    //  Coloca as informações previamente conhecidas do servidor    //
    socklen_t slen = sizeof(struct sockaddr_in6);                                       // Recebe o tamanho da estrutura do servidor.
    struct sockaddr_in6 servidor;                                                       // Estrutura que guarda as informações do servidor.
    memset(&servidor, 0, sizeof(servidor));                                             // Zera a estrutura do servidor.
    servidor.sin6_family = AF_INET6;                                                    // Protocolo de rede do servidor.
    servidor.sin6_port = htons(porta);                                                  // Recebe o número da porta do servidor.

    //  Traduz domínios para endereço IP    //
    char ip[100] = {0};                                                                 // Vetor que recebe o endereço de IP .
    if(hostname_to_ip(argv[1], ip) != 0)    {                                           // Verifica erro na traduçao para IP.
        fprintf(stderr, "Erro ao resolver dominio %s.\n", argv[1]);
        return 1;
    }
    inet_pton(AF_INET6, ip, &servidor.sin6_addr);                                       // Variável "in6_addr" recebe o endereço do servidor.

    //  Conecta o cliente ao servidor e testa falha de conexão  //
    if(connect(sock, (struct sockaddr *) &servidor, slen) < 0)    {
        perror("connect()");
        close(sock);
        return 1;
    }

    //  Recebe de um arquivo o tabuleiro para jogar //
    int **meuTabuleiro = recebeTabuleiro(ARQUIVO);
    if(meuTabuleiro == NULL)   {                                                        // Verifica erro na definição do tabuleiro.
        fprintf(stderr, "Falha ao posicionar a frota.\n");
        close(sock);
        return 1;
    }

    //  Aloca espaço para guardar tabuleiro (desconhecido) do adversário    //
    int **advTabuleiro = (int **) malloc(TAMANHO * sizeof(int *));
    for(int i = 0; i < TAMANHO; i++)
        advTabuleiro[i] = (int *) malloc(TAMANHO * sizeof(int));

    //  Coloca as casas do tabuleiro do servidor como não-visitadas //
    for(int i = 0; i < TAMANHO; i++)
        for(int j = 0; j < TAMANHO; j++)
            advTabuleiro[i][j] = 0;

    //  Loop até que o jogo encerre ou seja encerrado //
    while(1)  {
        char comando[5] = {0};
        int horizontal = 0;                                                             // Guarda a coordenada horizontal.
        int vertical = 0;                                                               // Guarda a coordenada vertical.
        fprintf(stdout, "Digite 'P' se quiser ver os tabuleiros.\n");
        fprintf(stdout, "Digite as coordenadas para atacar.\n");
        int k = 0;
        char c = 0;
        while((c = fgetc(stdin)) != '\n')
            comando[k++] = c;
        comando[k] = '\0';
        
        //   Verifica desejo de impressão de tabuleiros  //
        if(!strcmp(comando, "P"))    {
           imprimeTabuleiro(meuTabuleiro);
           imprimeTabuleiro(advTabuleiro);
        }
        //  Trata as coordenadas    //
        else    {
            trataCoordenadas(comando, &horizontal, &vertical);                          // Recebe as coordenadas.
        
            //  Envia ataque ao servidor    //
            if(send(sock, (char *) &comando, (size_t) strlen(comando), 0) == -1)  {         // Verifica erro de envio.
                perror("send-ataque()");
                break;
            }

            //  Recebe efetividade do ataque    //
            int efetivo = 0;
            int recebido = recv(sock, (int *) &efetivo, (size_t) sizeof(efetivo), 0);
            if(recebido < 0)    {
                perror("recv-efetividade()");
                break;
            }
            else if(recebido == 0)  {
                fprintf(stderr, "Conexão com servidor caiu.\n");
                break;
            }

            //  Verifica efetividade    //
            int restantes = marcaAtaque(advTabuleiro, &efetivo, horizontal, vertical);      // Recebe quantas peças faltam para vitória.

            //  Verifica se venceu  //
            if(!restantes)  {
                fprintf(stdout, "VOCÊ VENCEU!!!!\n");
                break;
            }

            //  Recebe coordenadas de contra-ataque //
            recebido = recv(sock, (char *) &comando, (size_t) sizeof(comando), 0);
            if(recebido < 0)    {
                perror("recv-contra-ataque()");
                break;
            }
            else if(recebido == 0)  {
                fprintf(stderr, "Conexão com servidor caiu.\n");
                break;
            }
        
            trataCoordenadas(comando, &horizontal, &vertical);
        
            efetivo = -1;
            restantes = marcaAtaque(meuTabuleiro, &efetivo, horizontal, vertical);          // Recebe quantas peças faltam para derrota.

            //  Envia efetividade   //
            if(send(sock, (int *) &efetivo, (size_t) sizeof(efetivo), 0) == -1)   {         // Verifica erro no envio.
                perror("send-contra-ataque()");
                break;
            }

            //  Verifica se perdeu  //
            if(!restantes)  {
                fprintf(stdout, "VOCÊ FOI DERROTADO.\n");
                break;
            }
        }
    }

    close(sock);                                                                        // Fecha o socket.
    for(int i = 0; i < TAMANHO; i++)
        free(meuTabuleiro[i]);
    free(meuTabuleiro);                                                                 // Libera a memória alocada para tabuleiro.
    for(int i = 0; i < TAMANHO; i++)
        free(advTabuleiro[i]);
    free(advTabuleiro);                                                                 // Libera a memória alocada para tabuleiro.

    return 0;
}

