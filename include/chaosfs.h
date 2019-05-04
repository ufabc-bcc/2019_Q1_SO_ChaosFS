#pragma once

/// Aqui ficam includes, defines e struct definition do FS


// Inclui a bibliteca fuse, base para o funcionamento do nosso FS
#include <fuse.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include "common.h"

void preenche_bloco (int isuperbloco, const char *nome, uint16_t direitos, uint16_t tamanho, uint16_t bloco, const byte *conteudo);


static int getattr_chaosfs(const char *path, struct stat *stbuf,
                           struct fuse_file_info *fi);


static int readdir_chaosfs(const char *path, void *buf, fuse_fill_dir_t filler,
                           off_t offset, struct fuse_file_info *fi,
                           enum fuse_readdir_flags flags);


static int open_chaosfs(const char *path, struct fuse_file_info *fi);


static int read_chaosfs(const char *path, char *buf, size_t size,
                        off_t offset, struct fuse_file_info *fi);


static int write_chaosfs(const char *path, const char *buf, size_t size,
                         off_t offset, struct fuse_file_info *fi);


static int truncate_chaosfs(const char *path, off_t size, struct fuse_file_info *fi);


static int mknod_chaosfs(const char *path, mode_t mode, dev_t rdev);


static int chmod_chaosfs(const char* path, mode_t mode, struct fuse_file_info *fi);


static int chown_chaosfs(const char* path, uid_t uid, gid_t gid, struct fuse_file_info *fi);


static int fsync_chaosfs(const char *path, int isdatasync,
                         struct fuse_file_info *fi);


static int utimens_chaosfs(const char *path, const struct timespec ts[2],
                           struct fuse_file_info *fi);


static int create_chaosfs(const char *path, mode_t mode,
                          struct fuse_file_info *fi);


void init_chaosfs();