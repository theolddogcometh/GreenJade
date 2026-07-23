/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Clean-room glibc-shaped <fcntl.h> for libcgj (GreenJade freestanding libc).
 * Not GNU glibc source; dual MIT OR Apache-2.0 only.
 *
 * Scope
 * -----
 * open/openat/openat2, fcntl, creat, flock/lockf shapes, fallocate/posix_fadvise,
 * and Linux splice/tee/vmsplice. O_* / F_* / AT_* / RESOLVE_* numbers match
 * Linux so flag bits survive the hybrid ABI boundary.
 *
 * Design notes
 * ------------
 * open_how is the openat2(2) structure. LFS creat64/fallocate64 are identity
 * on LP64. lockf commands (F_ULOCK/F_LOCK/…) coexist with fcntl F_GETLK.
 *
 * Non-goals
 * ---------
 * Full advisory lock fairness across all filesystems; soft fills may ENOSYS.
 * See docs/GLIBC_COMPAT.md.
 */
#pragma once

#include <stdint.h>
#include <sys/types.h>

/* ---- open(2) flags (Linux octal values) -------------------------------- */

#define O_RDONLY    0
#define O_WRONLY    1
#define O_RDWR      2
#define O_CREAT     0100
#define O_EXCL      0200
#define O_NOCTTY    0400
#define O_TRUNC     01000
#define O_APPEND    02000
#define O_NONBLOCK  04000
#define O_DIRECTORY 0200000
#define O_CLOEXEC   02000000

/* ---- *at(2) dirfd / symlink flags -------------------------------------- */

#define AT_FDCWD            (-100)
#define AT_SYMLINK_NOFOLLOW 0x100
#define AT_REMOVEDIR        0x200
#define AT_SYMLINK_FOLLOW   0x400
#define AT_EMPTY_PATH       0x1000
#define AT_EACCESS          0x200

/* ---- openat2(2) -------------------------------------------------------- */

struct open_how {
    uint64_t flags;
    uint64_t mode;
    uint64_t resolve;
};

#define RESOLVE_NO_XDEV       0x01
#define RESOLVE_NO_MAGICLINKS 0x02
#define RESOLVE_NO_SYMLINKS   0x04
#define RESOLVE_BENEATH       0x08
#define RESOLVE_IN_ROOT       0x10

#define RENAME_NOREPLACE  (1 << 0)
#define RENAME_EXCHANGE   (1 << 1)
#define RENAME_WHITEOUT   (1 << 2)

/* ---- fcntl commands / FD_CLOEXEC / flock ------------------------------- */

#define F_DUPFD     0
#define F_GETFD     1
#define F_SETFD     2
#define F_GETFL     3
#define F_SETFL     4
#define F_GETLK     5
#define F_SETLK     6
#define F_SETLKW    7
#define FD_CLOEXEC  1

#define F_RDLCK 0
#define F_WRLCK 1
#define F_UNLCK 2

/* lockf(3) commands */
#define F_ULOCK 0
#define F_LOCK  1
#define F_TLOCK 2
#define F_TEST  3

struct flock {
    short l_type;
    short l_whence;
    off_t l_start;
    off_t l_len;
    pid_t l_pid;
};

/* fallocate(2) mode flags (Linux) */
#define FALLOC_FL_KEEP_SIZE      0x01
#define FALLOC_FL_PUNCH_HOLE     0x02
#define FALLOC_FL_COLLAPSE_RANGE 0x08
#define FALLOC_FL_ZERO_RANGE     0x10

/* splice / tee / vmsplice flags (Linux) */
#define SPLICE_F_MOVE     1
#define SPLICE_F_NONBLOCK 2
#define SPLICE_F_MORE     4
#define SPLICE_F_GIFT     8

#ifdef __cplusplus
extern "C" {
#endif

struct iovec; /* full def in sys/uio.h */

/* posix_fadvise advice */
#define POSIX_FADV_NORMAL     0
#define POSIX_FADV_RANDOM     1
#define POSIX_FADV_SEQUENTIAL 2
#define POSIX_FADV_WILLNEED   3
#define POSIX_FADV_DONTNEED   4
#define POSIX_FADV_NOREUSE    5

int open(const char *szPath, int nFlags, ...);
int openat(int nDfd, const char *szPath, int nFlags, ...);
int openat2(int nDfd, const char *szPath, const struct open_how *pHow,
            size_t cb);
int fcntl(int nFd, int nCmd, ...);
int fcntl64(int nFd, int nCmd, ...);
int creat(const char *szPath, mode_t mode);
int creat64(const char *szPath, mode_t mode);
int fallocate(int nFd, int nMode, off_t off, off_t cbLen);
int fallocate64(int nFd, int nMode, off_t off, off_t cbLen);
int posix_fallocate(int nFd, off_t off, off_t cbLen);
int posix_fadvise(int nFd, off_t off, off_t cbLen, int nAdvice);
int lockf(int nFd, int nCmd, off_t cbLen);

/* Linux zero-copy pipe helpers (glibc places these in fcntl.h) */
ssize_t splice(int nFdin, off_t *pOffIn, int nFdout, off_t *pOffOut,
               size_t cbLen, unsigned uFlags);
ssize_t tee(int nFdin, int nFdout, size_t cbLen, unsigned uFlags);
ssize_t vmsplice(int nFd, const struct iovec *pIov, unsigned long cSegs,
                 unsigned uFlags);

#ifdef __cplusplus
}
#endif
