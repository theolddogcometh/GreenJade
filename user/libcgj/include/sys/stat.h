/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Clean-room glibc-shaped <sys/stat.h> for libcgj (GreenJade freestanding libc).
 * Not GNU glibc source; dual MIT OR Apache-2.0 only.
 *
 * Scope
 * -----
 * struct stat / statx-shaped fields, S_IF* mode bits, and stat/fstat/lstat/
 * fstatat/chmod/mkdir/mknod/utimensat family used by shells and package tools.
 *
 * Design notes
 * ------------
 * Layout targets Linux LP64 so host-built objects agree on st_size/st_ino
 * widths. *64 aliases are identity on product arches.
 *
 * Non-goals
 * ---------
 * Bit-identical padding with every historical glibc stat version node.
 * See docs/GLIBC_COMPAT.md.
 */
#pragma once

#include <stdint.h>
#include <sys/types.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Linux x86_64 struct stat (partial; enough for st_mode/size) */
struct stat {
    dev_t     st_dev;
    ino_t     st_ino;
    nlink_t   st_nlink;
    mode_t    st_mode;
    uid_t     st_uid;
    gid_t     st_gid;
    unsigned  __pad0;
    dev_t     st_rdev;
    off_t     st_size;
    blksize_t st_blksize;
    blkcnt_t  st_blocks;
    long      st_atim[2];
    long      st_mtim[2];
    long      st_ctim[2];
    long      __unused[3];
};

#define S_IFMT   0170000
#define S_IFSOCK 0140000
#define S_IFLNK  0120000
#define S_IFREG  0100000
#define S_IFBLK  0060000
#define S_IFDIR  0040000
#define S_IFCHR  0020000
#define S_IFIFO  0010000
#define S_ISUID  0004000
#define S_ISGID  0002000
#define S_ISVTX  0001000
#define S_IRWXU  00700
#define S_IRUSR  00400
#define S_IWUSR  00200
#define S_IXUSR  00100
#define S_IRWXG  00070
#define S_IRGRP  00040
#define S_IWGRP  00020
#define S_IXGRP  00010
#define S_IRWXO  00007
#define S_IROTH  00004
#define S_IWOTH  00002
#define S_IXOTH  00001

#define S_ISREG(m)  (((m) & S_IFMT) == S_IFREG)
#define S_ISDIR(m)  (((m) & S_IFMT) == S_IFDIR)
#define S_ISLNK(m)  (((m) & S_IFMT) == S_IFLNK)
#define S_ISCHR(m)  (((m) & S_IFMT) == S_IFCHR)
#define S_ISBLK(m)  (((m) & S_IFMT) == S_IFBLK)
#define S_ISFIFO(m) (((m) & S_IFMT) == S_IFIFO)
#define S_ISSOCK(m) (((m) & S_IFMT) == S_IFSOCK)

/* Linux statx (subset shape for mask/mode/size) */
struct statx_timestamp {
    int64_t  tv_sec;
    uint32_t tv_nsec;
    int32_t  __reserved;
};

struct statx {
    uint32_t stx_mask;
    uint32_t stx_blksize;
    uint64_t stx_attributes;
    uint32_t stx_nlink;
    uint32_t stx_uid;
    uint32_t stx_gid;
    uint16_t stx_mode;
    uint16_t __spare0[1];
    uint64_t stx_ino;
    uint64_t stx_size;
    uint64_t stx_blocks;
    uint64_t stx_attributes_mask;
    struct statx_timestamp stx_atime;
    struct statx_timestamp stx_btime;
    struct statx_timestamp stx_ctime;
    struct statx_timestamp stx_mtime;
    uint32_t stx_rdev_major;
    uint32_t stx_rdev_minor;
    uint32_t stx_dev_major;
    uint32_t stx_dev_minor;
    uint64_t stx_mnt_id;
    uint64_t __spare2[13];
};

#define STATX_TYPE       0x00000001u
#define STATX_MODE       0x00000002u
#define STATX_NLINK      0x00000004u
#define STATX_UID        0x00000008u
#define STATX_GID        0x00000010u
#define STATX_ATIME      0x00000020u
#define STATX_MTIME      0x00000040u
#define STATX_CTIME      0x00000080u
#define STATX_INO        0x00000100u
#define STATX_SIZE       0x00000200u
#define STATX_BLOCKS     0x00000400u
#define STATX_BASIC_STATS 0x000007ffu
#define STATX_BTIME      0x00000800u
#define STATX_MNT_ID     0x00001000u
#define STATX_ALL        0x00000fffu

#ifndef AT_EMPTY_PATH
#define AT_EMPTY_PATH 0x1000
#endif
#ifndef AT_NO_AUTOMOUNT
#define AT_NO_AUTOMOUNT 0x800
#endif
#ifndef AT_STATX_SYNC_AS_STAT
#define AT_STATX_SYNC_AS_STAT 0x0000
#endif
#ifndef AT_STATX_FORCE_SYNC
#define AT_STATX_FORCE_SYNC 0x2000
#endif
#ifndef AT_STATX_DONT_SYNC
#define AT_STATX_DONT_SYNC 0x4000
#endif

/* file_handle for name_to_handle_at / open_by_handle_at */
struct file_handle {
    unsigned int handle_bytes;
    int          handle_type;
    unsigned char f_handle[];
};

#define MAX_HANDLE_SZ 128

int fstat(int nFd, struct stat *pSt);
int stat(const char *szPath, struct stat *pSt);
int lstat(const char *szPath, struct stat *pSt);
int fstatat(int nDfd, const char *szPath, struct stat *pSt, int nFlags);
int newfstatat(int nDfd, const char *szPath, struct stat *pSt, int nFlags);
/* LFS aliases (identity on x86_64) */
int stat64(const char *szPath, struct stat *pSt);
int fstat64(int nFd, struct stat *pSt);
int lstat64(const char *szPath, struct stat *pSt);
int fstatat64(int nDfd, const char *szPath, struct stat *pSt, int nFlags);
/* Legacy glibc versioned stat entry points */
int __xstat(int nVer, const char *szPath, struct stat *pSt);
int __fxstat(int nVer, int nFd, struct stat *pSt);
int __lxstat(int nVer, const char *szPath, struct stat *pSt);
int __fxstatat(int nVer, int nDfd, const char *szPath, struct stat *pSt,
               int nFlags);
int __xstat64(int nVer, const char *szPath, struct stat *pSt);
int __fxstat64(int nVer, int nFd, struct stat *pSt);
int __lxstat64(int nVer, const char *szPath, struct stat *pSt);
int __fxstatat64(int nVer, int nDfd, const char *szPath, struct stat *pSt,
                 int nFlags);
int statx(int nDfd, const char *szPath, int nFlags, unsigned uMask,
          struct statx *pStx);
int name_to_handle_at(int nDfd, const char *szPath, struct file_handle *pHandle,
                      int *pMountId, int nFlags);
int open_by_handle_at(int nMountFd, struct file_handle *pHandle, int nFlags);
int mkdir(const char *szPath, mode_t mode);
int mkdirat(int nDfd, const char *szPath, mode_t mode);
int mknod(const char *szPath, mode_t mode, dev_t dev);
int mknodat(int nDfd, const char *szPath, mode_t mode, dev_t dev);
int mkfifo(const char *szPath, mode_t mode);
int mkfifoat(int nDfd, const char *szPath, mode_t mode);
int chmod(const char *szPath, mode_t mode);
int fchmod(int nFd, mode_t mode);
int fchmodat(int nDfd, const char *szPath, mode_t mode, int nFlags);
int fchmodat2(int nDfd, const char *szPath, mode_t mode, int nFlags);

#ifdef __cplusplus
}
#endif

