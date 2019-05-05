#include "utils.h"

//! NÃO ESQUECER DE ATUALIZAR A HEADER (include/utils.h) SE ADD UMA NOVA FUNÇÃO!!

//! Funções utilitárias que não estão diretamente relacionados ao
//! filesystem em si.
//! A ideia é, funções que podem ser usadas em qualquer lugar.


bool compara_nome (const char *a, const char *b) {
    // return cmp_nomes_rs(a, b);
    char *ma = (char*)a;
    char *mb = (char*)b;

    size_t sa = 0, sb = 0;

    for (size_t i = 0, j = 0; i < strlen(ma) && j < strlen(mb); i++, j++) {
        if (ma[i] == '/') sa = i;
        if (mb[j] == '/') sb = j;
    }
    //
    //
    // if (strcmp(ma, "/") == 0 && strcmp(ma, "/") == 0) return true;
    //
    // //Joga fora barras
    // for (int i = 0; i < strlen(ma) && ma[0] != '\0'; i++) {
    //     if (ma[i] == '/')
    //         ma++;
    // }
    //
    // for (int i = 0; i < strlen(ma) && mb[0] != '\0'; i++) {
    //     if (mb[i] == '/')
    //         mb++;
    // }
    //
    // // while (ma[0] != '\0' && ma[0] == '/')
    // //     ma++;
    // // while (mb[0] != '\0' && mb[0] == '/')
    // //     mb++;
    // //Cuidado! Pode ser necessário jogar fora também barras repetidas internas
    // //quando tiver diretórios
    return strcmp((ma+sa), (mb+sb)) == 0;
}
