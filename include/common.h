#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>
#include <inttypes.h>
#include <time.h>

// Data types
// Não é obrigado a usar
// Basicamente dando nomes alternativos para tipos já existentes,
// use apenas se quiser
typedef int8_t    s8;
typedef int16_t   s16;
typedef int32_t   s32;
typedef int64_t   s64;

typedef uint8_t   u8;
typedef uint16_t  u16;
typedef uint32_t  u32;
typedef uint64_t  u64;

typedef volatile s8  vs8;
typedef volatile s16 vs16;
typedef volatile s32 vs32;
typedef volatile s64 vs64;

typedef volatile u8  vu8;
typedef volatile u16 vu16;
typedef volatile u32 vu32;
typedef volatile u64 vu64;

typedef uintptr_t uPtr;
typedef size_t Size;

typedef char byte;

#define INODE_SIZE sizeof(inode)
/* Tamnanho do bloco do dispositivo */
#define TAM_BLOCO 4096
/* A atual implementação utiliza apenas um bloco para todos os inodes
   de todos os arquivos do sistema. Ou seja, cria um limite rígido no
   número de arquivos e tamanho do dispositivo. */
#define MAX_FILES 1024
/* Quantidade necessária para o superbloco juntamente com a quantidade
   máxima de arquivos. Os arquivos nesta implementação também tem 
   apenas 1 bloco no máximo de tamanho. */
#define MAX_BLOCOS (QTD_BLOCOS_SUPERBLOCO + MAX_FILES)
/* Parte da sua tarefa será armazenar e recuperar corretamente os
   direitos dos arquivos criados */
#define DIREITOS_PADRAO 0644
/* Formula utilizada para calcular a quantidade de blocos ocupados 
   pelosuperbloco e que são necessários para criar, pelo menos, 
   1024 arquivos. */
#define QTD_BLOCOS_SUPERBLOCO (1024*INODE_SIZE/(TAM_BLOCO-INODE_SIZE))+1

#define TAM_DISCO (MAX_BLOCOS * TAM_BLOCO)
/* Um inode guarda todas as informações relativas a um arquivo como
   por exemplo nome, direitos, tamanho, bloco inicial, ... */
typedef struct {
    char nome[250];
    uid_t uid;
    gid_t gid;
    u16 direitos;
    u16 tamanho;
    u16 bloco;
    time_t data_acesso;
    time_t data_modific;
} inode;