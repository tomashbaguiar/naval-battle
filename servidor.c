#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <time.h>

#include "protocolo.h"

//  ./servidor <porta>

int
main(int argc, char **argv)
{
    //  Testa os argumentos de entrada  //
    if(argc != 2)   {
        fprintf(stderr, "Utilização:\n\t./servidor <porta>\n");
        return 1;
    }

    int porta = atoi(argv[1]);                                                          // Porta do servidor.

    //  Cria, e verifica, socket para se comunicar  //
    int sock = socket(AF_INET6, SOCK_STREAM, 0);                                        // Socket criado como IPv6 e TCP.
    if(sock == -1)    {
        perror("socket()");                                                             // Imprime erro na criação do socket.
        return 1;
    }

    //  Coloca as informações do servidor na estrutura  //
    struct sockaddr_in6 servidor;
    size_t slen = sizeof(servidor);
    memset(&servidor, 0, slen);                                                         // Inicia a estrutura com campos zerados.

    servidor.sin6_family = AF_INET6;                                                    // IPv6.
    servidor.sin6_port = htons(porta);                                                  // Porta do servidor.
    servidor.sin6_addr = in6addr_any;                                                   // Recebe o endereço local.

    //  Liga o socket ao endereço local //
    if(bind(sock, (struct sockaddr *) &servidor, (socklen_t) slen) == -1)    {
        perror("bind()");                                                               // Imprime em caso de erro.
        close(sock);                                                                    // Fecha o socket.
        return 1;                                                                       // Fecha o programa.
    }

    //  Espera conexões com o socket    //
    if(listen(sock, 10) == -1)    {
        perror("listen()");
        close(sock);
        return 1;
    }

    srand(time(NULL));                                                                  // Para gerar números aleatórios.
    //  Loop externo, até que algum cliente se conecte   //
    while(1)    {
        fprintf(stdout, "Esperando conexões...\n");

        //  Aceita conexões no socket   //
        struct sockaddr_in6 cliente;
        socklen_t clen = sizeof(cliente);

        int cli = accept(sock, (struct sockaddr *) &cliente, &clen);
        if(cli < 0) {                                                                   // Testa erro em accept.
            perror("accept()");                                                         // Imprime o erro.
            close(sock);                                                                // Fecha o socket do servidor.
            return 1;                                                                   // Fecha o programa do servidor.
        }

        fprintf(stdout, "Conectado\n");

        //  Gera e posiciona a frota no tabuleiro   //
        int **tabuleiro = geraTabuleiro();

        if(tabuleiro == NULL)   {                                                       // Verifica erro na definição do tabuleiro.
            fprintf(stderr, "Falha ao posicionar a frota.\n");
            close(sock);
            close(cli);
            return 1;
        }

        imprimeTabuleiro(tabuleiro);

        int advContagem = 30;
        int acertou = 0;                                                                // Guarda se última jogada foi efetiva.
        int linha, coluna;
        //  Loop interno, quando tem algum cliente conectado    //
        while(cli != -1)  {
            char coordenadas[4] = {0};                                                  // Recebe as coordenadas.

            //  Recebe coordenadas de defesa    //
            int recebido = recv(cli, (char *) &coordenadas, (size_t) sizeof(coordenadas), 0);
            if(recebido < 0)    {
                perror("recv-contra-ataque()");
                break;
            }
            else if(recebido == 0)  {
                fprintf(stderr, "Conexão com o cliente caiu.\n");
                break;
            }

            int horizontal = 0;
            int vertical = 0;
            trataCoordenadas(coordenadas, &horizontal, &vertical);

            int efetivo = -1;
            int restantes = marcaAtaque(tabuleiro, &efetivo, horizontal, vertical);     // Recebe quantas peças faltam para derrota.

            //  Envia efetividade   //
            if(send(cli, (int *) &efetivo, (size_t) sizeof(efetivo), 0) == -1)  {       // Verifica erro no envio.
                perror("send-efetividade()");
                break;
            }

            //  Verifica se perdeu  //
            if(!restantes)  {
                fprintf(stdout, "SERVIDOR DERROTADO.\n");
                break;
            }

            //  Gera coordenadas para envio //
            if(acertou) {                                                               // Verifica se acertou última jogada.
                int orientacao = rand() % 2;                                            // 0 = vertical, 1 = horizontal.
                if(orientacao)
                    linha++;
                else
                    coluna++;

                if(linha >= 10)
                    linha -= 2;
                if(coluna >= (9 + 97))
                    coluna -= 2;
            }                
            else    {                                                                   // Se errou a última, gera aletoriamente.
                linha = (rand() % TAMANHO) + 1;
                coluna = (rand() % TAMANHO) + 97;
            }
            sprintf(coordenadas, "%c %d", coluna, linha);

            //  Envia ataque ao cliente //
            if(send(cli, (char *) &coordenadas, (size_t) strlen(coordenadas), 0) == -1) {
                perror("send-ataque()");
                break;
            }

            //  Recebe efetividade do ataque    //
            efetivo = 0;
            recebido = recv(cli, (int *) &efetivo, (size_t) sizeof(efetivo), 0);
            if(recebido < 0)    {
                perror("recv-efetividade()");
                break;
            }
            else if(recebido == 0)  {
                fprintf(stderr, "Conexão com o cliente caiu.\n");
                break;
            }

            //  Verifica efetividade    //
            if(efetivo) {
                advContagem--;
                acertou = 1;
            }
            else
                acertou = 0;

            //  Verifica se venceu  //
            if(!advContagem)  {
                fprintf(stdout, "SERVIDOR VENCEU!!!\n");
                break;
            }

            imprimeTabuleiro(tabuleiro);
        }

        close(cli);
        for(int i = 0; i < TAMANHO; i++)
            free(tabuleiro[i]);
        free(tabuleiro);
    }

    close(sock);

    return 0;
}

