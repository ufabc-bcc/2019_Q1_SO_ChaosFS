#pragma once

/// Aqui ficam includes, defines e struct definition do FS

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>
#include <inttypes.h>
#include <time.h>
// Inclui a bibliteca fuse, base para o funcionamento do nosso FS
#include <fuse.h>
#include <string.h>
#include <errno.h>


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

/* Tamnanho do bloco do dispositivo */
#define TAM_BLOCO 4096
/* A atual implementação utiliza apenas um bloco para todos os inodes
   de todos os arquivos do sistema. Ou seja, cria um limite rígido no
   número de arquivos e tamanho do dispositivo. */
#define MAX_FILES (TAM_BLOCO / sizeof(inode))
/* 1 para o superbloco e o resto para os arquivos. Os arquivos nesta
   implementação também tem apenas 1 bloco no máximo de tamanho. */
#define MAX_BLOCOS (1 + MAX_FILES)
/* Parte da sua tarefa será armazenar e recuperar corretamente os
   direitos dos arquivos criados */
#define DIREITOS_PADRAO 0644

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
