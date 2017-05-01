#ifndef PROTOCOLO_H
    #define PROTOCOLO_H

    #define TAMANHO 10
    
    /*
     * Lê as posições de entrada do tabuleiro
     * diretamente de um arquivo e retorna
     * a matriz formada.
     */
    int **recebeTabuleiro(char*);

    /*
     * Cria, aleatoriamente um tabuleiro
     * e retorna a matriz formada.
     */
    //int **geraTabuleiro();

    /*
     * Imprime tabuleiros.
     */
    void imprimeTabuleiro(int**);

    /*
     * Marca o ataque realizado no tabuleiro
     */
    int marcaAtaque(int**, int*, int, int);

    /*
     * Trata coordenadas recebidas
     */
    void trataCoordenadas(char*, int*, int*);

    /*
     * Retorna a existência de peça
     * em determinada coordenada
     */
    int existePeca(int**, int, int);

    /*
     * Traduz nomes de domínios pra IP
     */
    int hostname_to_ip(char*, char*);

#endif
