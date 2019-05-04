/*
 * Emilio Francesquini <e.francesquini@ufabc.edu.br>
 * 2019-02-03
 *
 * Este código foi criado como parte do enunciado do projeto de
 * programação da disciplina de Sistemas Operacionais na Universidade
 * Federal do ABC. Você pode reutilizar este código livremente
 * (inclusive para fins comerciais) desde que sejam mantidos, além
 * deste aviso, os créditos aos autores e instituições.
 *
 * Licença: CC-BY-SA 4.0
 *
 * O código abaixo implementa (parcialmente) as bases sobre as quais
 * você deve construir o seu projeto. Ele provê um sistema de arquivos
 * em memória baseado em FUSE. Parte do conjunto minimal de funções
 * para o correto funcionamento do sistema de arquivos está
 * implementada, contudo nem todos os seus aspectos foram tratados
 * como, por exemplo, datas, persistência e exclusão de arquivos.
 *
 * Em seu projeto você precisará tratar exceções, persistir os dados
 * em um único arquivo e aumentar os limites do sistema de arquivos
 * que, para o código abaixo, são excessivamente baixos (ex. número
 * total de arquivos).
 *
 */


#define FUSE_USE_VERSION 31

#include "chaosfs.h"
#include "utils.h"

/* Disco - A variável abaixo representa um disco que pode ser acessado
   por blocos de tamanho TAM_BLOCO com um total de MAX_BLOCOS. Você
   deve substituir por um arquivo real e assim persistir os seus
   dados! */
byte *disco;

//guarda os inodes dos arquivos
inode *superbloco;

#define DISCO_OFFSET(B) (B * TAM_BLOCO)


/* Preenche os campos do superbloco de índice isuperbloco */
void preenche_bloco (int isuperbloco, const char *nome, mode_t mode, nlink_t link,
                     uint16_t tamanho, uint16_t bloco, const byte *conteudo) {
    char *mnome = (char*)nome;
    //Joga fora a(s) barras iniciais
    while (mnome[0] != '\0' && mnome[0] == '/')
        mnome++;

    time_t times = time(NULL);
    if (times == -1)
        times = 0;


    strcpy(superbloco[isuperbloco].nome, mnome);
    superbloco[isuperbloco].mode = mode;
    superbloco[isuperbloco].link = link;
    superbloco[isuperbloco].tamanho = tamanho;
    superbloco[isuperbloco].bloco = bloco;
    superbloco[isuperbloco].uid = getuid();
    superbloco[isuperbloco].gid = getgid();
    superbloco[isuperbloco].data_acesso   = times;
    superbloco[isuperbloco].data_modific  = times;

    if (conteudo != NULL)
        memcpy(disco + DISCO_OFFSET(bloco), conteudo, tamanho);
    else
        memset(disco + DISCO_OFFSET(bloco), 0, tamanho);
}

/* A função getattr_chaosfs devolve os metadados de um arquivo cujo
   caminho é dado por path. Devolve 0 em caso de sucesso ou um código
   de erro. Os atributos são devolvidos pelo parâmetro stbuf */
static int getattr_chaosfs(const char *path, struct stat *stbuf,
                           struct fuse_file_info *fi) {
    memset(stbuf, 0, sizeof(struct stat));

    // printf("ASKED DIR: %s\n", path );
    //Busca arquivo na lista de inodes
    for (int i = 0; i < MAX_BLOCOS; i++) {
        if (superbloco[i].bloco != 0 //Bloco sendo usado
            && compara_nome(superbloco[i].nome, path)) { //Nome bate

            stbuf->st_mode = superbloco[i].mode;
            stbuf->st_nlink = superbloco[i].link;
            stbuf->st_size = superbloco[i].tamanho;

            // Pega id do usuario e do grupo que o arquivo pertence
            stbuf->st_uid = superbloco[i].uid;
            stbuf->st_gid = superbloco[i].gid;

            // Pega a ultima data de modificação e acesso
            stbuf->st_mtime = superbloco[i].data_modific;
            stbuf->st_atime = superbloco[i].data_acesso;
            return 0; //OK, arquivo encontrado
        }
    }

    //Erro arquivo não encontrado
    return -ENOENT;
}


/* Devolve ao FUSE a estrutura completa do diretório indicado pelo
   parâmetro path. Devolve 0 em caso de sucesso ou um código de
   erro. Atenção ao uso abaixo dos demais parâmetros. */
static int readdir_chaosfs(const char *path, void *buf, fuse_fill_dir_t filler,
                           off_t offset, struct fuse_file_info *fi,
                           enum fuse_readdir_flags flags) {
    (void) offset;
    (void) fi;

    filler(buf, ".", NULL, 0, 0);
    filler(buf, "..", NULL, 0, 0);

    // printf("ASKED DIR: %s\n", path );
    bool done = false;
    for (int i = 0; i < MAX_BLOCOS; i++) {
        if (superbloco[i].bloco != 0 && compara_nome(path, superbloco[i].nome)) { //Bloco ocupado!
            if (S_ISDIR(superbloco[i].mode)) {
                uint16_t* v = (uint16_t *) (disco + DISCO_OFFSET(superbloco[i].bloco));

                if (v == NULL) {
                    done = false;
                    break;
                }

                filler(buf, ".", NULL, 0, 0);
                filler(buf, "..", NULL, 0, 0);

                for (uint16_t j = 0; ; j++) {
                    if ((v+j) == NULL) {
                        break;
                    } else { done = true; }
                    printf("%d\n", v[j]);
                    filler(buf, superbloco[v[j]].nome, NULL, 0, 0);
                }

            } else {
                return -ENOTDIR;
            }
        }
    }

    if (done) {
        return 0;
    }
    return -ENOENT;
}

/* Abre um arquivo. Caso deseje controlar os arquvos abertos é preciso
   implementar esta função */
static int open_chaosfs(const char *path, struct fuse_file_info *fi) {
    return 0;
}

/* Função chamada quando o FUSE deseja ler dados de um arquivo
   indicado pelo parâmetro path. Se você implementou a função
   open_chaosfs, o uso do parâmetro fi é necessário. A função lê size
   bytes, a partir do offset do arquivo path no buffer buf. */
static int read_chaosfs(const char *path, char *buf, size_t size,
                        off_t offset, struct fuse_file_info *fi) {

    //Procura o arquivo
    for (int i = 0; i < MAX_BLOCOS; i++) {
        if (superbloco[i].bloco == 0) //bloco vazio
            continue;
        if (compara_nome(path, superbloco[i].nome)) {//achou!
            size_t len = superbloco[i].tamanho;
            superbloco[i].data_acesso = time(NULL);

            if (offset >= len) {//tentou ler além do fim do arquivo
                return 0;
            }
            if (offset + size > len) {
                memcpy(buf,
                       disco + DISCO_OFFSET(superbloco[i].bloco),
                       len - offset);
                return len - offset;
            }

            memcpy(buf,
                   disco + DISCO_OFFSET(superbloco[i].bloco), size);
            return size;
        }
    }
    //Arquivo não encontrado
    return -ENOENT;
}

/* Função chamada quando o FUSE deseja escrever dados em um arquivo
   indicado pelo parâmetro path. Se você implementou a função
   open_chaosfs, o uso do parâmetro fi é necessário. A função escreve
   size bytes, a partir do offset do arquivo path no buffer buf. */
static int write_chaosfs(const char *path, const char *buf, size_t size,
                         off_t offset, struct fuse_file_info *fi) {

    for (int i = 0; i < MAX_BLOCOS; i++) {
        if (superbloco[i].bloco == 0) { //bloco vazio
            continue;
        }
        if (compara_nome(path, superbloco[i].nome)) {//achou!
            // Cuidado! Não checa se a quantidade de bytes cabe no arquivo!

            memcpy(disco + DISCO_OFFSET(superbloco[i].bloco) + offset, buf, size);
            superbloco[i].tamanho = offset + size;
            superbloco[i].data_modific = time(NULL);
            return size;
        }
    }

    //Se chegou aqui não achou. Entao cria
    //Acha o primeiro bloco vazio
    for (int i = 0; i < MAX_BLOCOS; i++) {
        if (superbloco[i].bloco == 0) {//ninguem usando
            preenche_bloco (i, path, DIREITOS_PADRAO | S_IFREG, 1, size, i + QTD_BLOCOS_SUPERBLOCO, buf);
            return size;
        }
    }

    return -EIO;
}


/* Altera o tamanho do arquivo apontado por path para tamanho size
   bytes */
static int truncate_chaosfs(const char *path, off_t size, struct fuse_file_info *fi) {
    if (size > TAM_BLOCO)
        return EFBIG;

    //procura o arquivo
    int findex = -1;
    for(int i = 0; i < MAX_BLOCOS; i++) {
        if (superbloco[i].bloco != 0
            && compara_nome(path, superbloco[i].nome)) {
            findex = i;
            break;
        }
    }
    if (findex != -1) {// arquivo existente
        superbloco[findex].tamanho = size;
        return 0;
    } else {// Arquivo novo
        //Acha o primeiro bloco vazio
        for (int i = 0; i < MAX_BLOCOS; i++) {
            if (superbloco[i].bloco == 0) {//ninguem usando
                preenche_bloco (i, path, DIREITOS_PADRAO | S_IFREG, 1, size, i + QTD_BLOCOS_SUPERBLOCO, NULL);
                break;
            }
        }
    }
    return 0;
}

/* Cria um arquivo comum ou arquivo especial (links, pipes, ...) no caminho
   path com o modo mode*/
static int mknod_chaosfs(const char *path, mode_t mode, dev_t rdev) {
    if (S_ISREG(mode)) { //So aceito criar arquivos normais
        //Veja "man 2
        //mknod" para instruções de como pegar os direitos e demais
        //informações sobre os arquivos
        //Acha o primeiro bloco vazio
        for (int i = 0; i < MAX_BLOCOS; i++) {
            if (superbloco[i].bloco == 0) {//ninguem usando
                preenche_bloco (i, path, DIREITOS_PADRAO | S_IFREG, 1, 0, i + QTD_BLOCOS_SUPERBLOCO, NULL);
                return 0;
            }
        }
        return ENOSPC;
    }
    return EINVAL;
}

static int mkdir_chaosfs(const char* path, mode_t mode) {
    mode_t new_dir_mode = mode | S_IFDIR;
    if (S_ISDIR(new_dir_mode)) {
        for (int i = 0; i < MAX_BLOCOS; i++) {
            if (compara_nome(path, superbloco[i].nome)) {
                return -EEXIST;
            }
            if (superbloco[i].bloco == 0) {//ninguem usando
                preenche_bloco (i, path, new_dir_mode, 2, 0, i + QTD_BLOCOS_SUPERBLOCO, NULL);
                return 0;
            }
        }
        return -ENOSPC;
    }
    return -EINVAL;
}

static int chmod_chaosfs(const char* path, mode_t mode, struct fuse_file_info *fi) {
    // Procura o superbloco do arquivo
    for (int i = 0; i < MAX_BLOCOS; i++) {
        if (superbloco[i].bloco == 0) //bloco vazio
            continue;
        if (compara_nome(path, superbloco[i].nome)) { //achou!
            superbloco[i].mode = mode;
            return 0;
        }
    }

    return -ENOENT;
}

static int chown_chaosfs(const char* path, uid_t uid, gid_t gid, struct fuse_file_info *fi) {
    // Procura o superbloco do arquivo
    for (int i = 0; i < MAX_BLOCOS; i++) {
        if (superbloco[i].bloco == 0) //bloco vazio
            continue;
        if (compara_nome(path, superbloco[i].nome)) { //achou!
            superbloco[i].uid = uid;
            superbloco[i].gid = gid;
            return 0;
        }
    }

    return -ENOENT;
}

/* Sincroniza escritas pendentes (ainda em um buffer) em disco. Só
   retorna quando todas as escritas pendentes tiverem sido
   persistidas */
static int fsync_chaosfs(const char *path, int isdatasync,
                         struct fuse_file_info *fi) {
    //Como tudo é em memória, não é preciso fazer nada.
    // Cuidado! Você vai precisar jogar tudo que está só em memóri no disco
    return 0;
}


/* Ajusta a data de acesso e modificação do arquivo com resolução de nanosegundos */
static int utimens_chaosfs(const char *path, const struct timespec ts[2],
                           struct fuse_file_info *fi) {

    // Procura o superbloco do arquivo
    for (int i = 0; i < MAX_BLOCOS; i++) {
        if (superbloco[i].bloco == 0) //bloco vazio
            continue;
        if (compara_nome(path, superbloco[i].nome)) { //achou!
            superbloco[i].data_modific = time(NULL);
            return 0;
        }
    }

    return -ENOENT;
}


/* Cria e abre o arquivo apontado por path. Se o arquivo não existir
   cria e depois abre*/
static int create_chaosfs(const char *path, mode_t mode,
                          struct fuse_file_info *fi) {
    //Veja "man 2 mknod" para instruções de como pegar os
    //direitos e demais informações sobre os arquivos Acha o primeiro
    //bloco vazio
    for (int i = 0; i < MAX_BLOCOS; i++) {
        if (superbloco[i].bloco == 0) {//ninguem usando
            preenche_bloco (i, path, mode, 1, 0, i + QTD_BLOCOS_SUPERBLOCO, NULL);
            return 0;
        }
    }
    return ENOSPC;
}

/* Para persistir o FS em um disco representado por um arquivo, talvez
   seja necessário "formatar" o arquivo pegando o seu tamanho e
   inicializando todas as posições (ou apenas o(s) superbloco(s))
   com os valores apropriados */
void init_chaosfs() {
    disco = calloc (MAX_BLOCOS, TAM_BLOCO);
    superbloco = (inode*) disco; //posição 0
    //Cria um arquivo na mão de boas vindas

    // Cria root
    mkdir_chaosfs("/", DIREITOS_PADRAO | S_IFDIR);
    // preenche_bloco(0, "/", DIREITOS_PADRAO | S_IFDIR, 2, 0, 0 + QTD_BLOCOS_SUPERBLOCO, NULL);
    mkdir_chaosfs("/.", DIREITOS_PADRAO | S_IFDIR);
    mkdir_chaosfs("/..", DIREITOS_PADRAO | S_IFDIR);


    // char *nome = "UFABC SO 2019.txt";
    // //Cuidado! pois se tiver acentos em UTF8 uma letra pode ser mais que um byte
    // char *conteudo = "Adoro as aulas de SO da UFABC!\n";
    // //0 está sendo usado pelo superbloco. O primeiro livre é o 1
    // preenche_bloco(0, nome, DIREITOS_PADRAO | S_IFREG, strlen(conteudo), 2 + QTD_BLOCOS_SUPERBLOCO , (byte*)conteudo);



    // mkdir_chaosfs("dir", DIREITOS_PADRAO | S_IFDIR);
    // preenche_bloco(0, "ifo", DIREITOS_PADRAO | S_IFIFO, 0, 1 + QTD_BLOCOS_SUPERBLOCO , NULL);
    // preenche_bloco(0, "chr", DIREITOS_PADRAO | S_IFCHR, 0, 2 + QTD_BLOCOS_SUPERBLOCO , NULL);
//     preenche_bloco(0, "dir", DIREITOS_PADRAO | S_IFDIR, 0, 3 + QTD_BLOCOS_SUPERBLOCO , NULL);
//     preenche_bloco(0, "blk", DIREITOS_PADRAO | S_IFBLK, 0, 4 + QTD_BLOCOS_SUPERBLOCO , NULL);
//     preenche_bloco(0, "reg", DIREITOS_PADRAO | S_IFREG, 0, 5 + QTD_BLOCOS_SUPERBLOCO , NULL);
//     preenche_bloco(0, "lnk", DIREITOS_PADRAO | S_IFLNK, 0, 6 + QTD_BLOCOS_SUPERBLOCO , NULL);
//     preenche_bloco(0, "sock", DIREITOS_PADRAO | S_IFSOCK, 0, 7 + QTD_BLOCOS_SUPERBLOCO , NULL);
//     preenche_bloco(0, "mt", DIREITOS_PADRAO | S_IFMT, 0, 8 + QTD_BLOCOS_SUPERBLOCO , NULL);
}

/* Esta estrutura contém os ponteiros para as operações implementadas
   no FS */
static struct fuse_operations fuse_chaosfs = {
    .create = create_chaosfs,
    .fsync = fsync_chaosfs,
    .getattr = getattr_chaosfs,
    .mknod = mknod_chaosfs,
    .mkdir = mkdir_chaosfs,
    .chmod = chmod_chaosfs,
    .chown = chown_chaosfs,
    .open = open_chaosfs,
    .read = read_chaosfs,
    .readdir = readdir_chaosfs,
    .truncate	= truncate_chaosfs,
    .utimens = utimens_chaosfs,
    .write = write_chaosfs
};

int main(int argc, char *argv[]) {

    // printf("FIFO:\t %6o\n", DIREITOS_PADRAO | S_IFIFO);
    // printf("CHR:\t %6o\n", DIREITOS_PADRAO | S_IFCHR);
    // printf("DIR:\t %6o\n", DIREITOS_PADRAO | S_IFDIR);
    // printf("BLK:\t %6o\n", DIREITOS_PADRAO | S_IFBLK);
    // printf("REG:\t %6o\n", DIREITOS_PADRAO | S_IFREG);
    // printf("LNK:\t %6o\n", DIREITOS_PADRAO | S_IFLNK);
    // printf("SOCK:\t %6o\n", DIREITOS_PADRAO | S_IFSOCK);
    // printf("FMT:\t %6o\n", DIREITOS_PADRAO | S_IFMT);

    printf("Iniciando o ChaosFS...\n");
    printf("\t Tamanho máximo de arquivo = 1 bloco = %d bytes\n", TAM_BLOCO);
    printf("\t Tamanho do inode: %lu\n", sizeof(inode));
    printf("\t Número máximo de arquivos: %d\n", MAX_FILES);

    init_chaosfs();

    return fuse_main(argc, argv, &fuse_chaosfs, NULL);
    // return 0;
}
