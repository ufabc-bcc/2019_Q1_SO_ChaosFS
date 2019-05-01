#include <stdbool.h>
#include <string.h>
#include <time.h>

//! NÃO ESQUECER DE ATUALIZAR A HEADER (include/utils.h) SE ADD UMA NOVA FUNÇÃO!!

//! Funções utilitárias que não estão diretamente relacionados ao
//! filesystem em si.
//! A ideia é, funções que podem ser usadas em qualquer lugar.

bool compara_nome (const char *a, const char *b) {
    char *ma = (char*)a;
    char *mb = (char*)b;
    //Joga fora barras iniciais
    while (ma[0] != '\0' && ma[0] == '/')
        ma++;
    while (mb[0] != '\0' && mb[0] == '/')
        mb++;
    //Cuidado! Pode ser necessário jogar fora também barras repetidas internas
    //quando tiver diretórios
    return strcmp(ma, mb) == 0;
}

