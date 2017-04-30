#ifndef PROTOCOLO_H
    #define PROTOCOLO_H
    
    /*
     * Lê as posições de entrada do tabuleiro
     * diretamente de um arquivo e retorna
     * a matriz formada.
     */
    int **recebeTabuleiro(const char);

    /*
     * Cria, aleatoriamente um tabuleiro
     * e retorna a matriz formada.
     */
    int **geraTabuleiro();

    /*
     * Imprime tabuleiros.
     */
    void imprimeTabuleiro(int**);

    int marcaAtaque(int**, int*);

#endif
