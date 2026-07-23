/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Clean-room glibc-shaped sys/mman.h (subset). Not GNU glibc.
 */
#pragma once

#include <stddef.h>
#include <stdint.h>
#include <sys/types.h>

/* mode_t for shm_open */
#ifndef mode_t
/* provided by sys/types.h */
#endif

#ifdef __cplusplus
extern "C" {
#endif

#define PROT_NONE       0x0
#define PROT_READ       0x1
#define PROT_WRITE      0x2
#define PROT_EXEC       0x4

#define MAP_SHARED      0x01
#define MAP_PRIVATE     0x02
#define MAP_FIXED       0x10
#define MAP_ANONYMOUS   0x20
#define MAP_ANON        MAP_ANONYMOUS
#define MAP_FAILED      ((void *)(intptr_t)-1)

#define MS_ASYNC      1
#define MS_INVALIDATE 2
#define MS_SYNC       4

#define MADV_NORMAL     0
#define MADV_RANDOM     1
#define MADV_SEQUENTIAL 2
#define MADV_WILLNEED   3
#define MADV_DONTNEED   4

void *mmap(void *pAddr, size_t cb, int nProt, int nFlags, int nFd, off_t off);
int   munmap(void *pAddr, size_t cb);
int   mprotect(void *pAddr, size_t cb, int nProt);
int   msync(void *pAddr, size_t cb, int nFlags);
int   madvise(void *pAddr, size_t cb, int nAdvice);
int   posix_madvise(void *pAddr, size_t cb, int nAdvice);

#ifndef MREMAP_MAYMOVE
#define MREMAP_MAYMOVE 1
#endif
#ifndef MREMAP_FIXED
#define MREMAP_FIXED 2
#endif
void *mremap(void *pOld, size_t cbOld, size_t cbNew, int nFlags, ...);

/* memfd / POSIX shm-shaped (clean-room names) */
#define MFD_CLOEXEC 1u
#define MFD_ALLOW_SEALING 2u

int memfd_create(const char *szName, unsigned uFlags);
int memfd_secret(unsigned uFlags);
int shm_open(const char *szName, int nFlags, mode_t mode);
int shm_unlink(const char *szName);

#define MCL_CURRENT 1
#define MCL_FUTURE  2
#define MCL_ONFAULT 4

int mlock(const void *pAddr, size_t cb);
int munlock(const void *pAddr, size_t cb);
int mlockall(int nFlags);
int munlockall(void);
int mincore(void *pAddr, size_t cb, unsigned char *pVec);

#define MLOCK_ONFAULT 1u

int mlock2(const void *pAddr, size_t cb, unsigned int uFlags);
int pkey_alloc(unsigned int uFlags, unsigned int uAccessRights);
int pkey_free(int nPkey);
int pkey_mprotect(void *pAddr, size_t cb, int nProt, int nPkey);
int userfaultfd(int nFlags);

struct iovec; /* sys/uio.h */
ssize_t process_madvise(int nPidfd, const struct iovec *pIov, size_t nVlen,
                        int nAdvice, unsigned int uFlags);
int process_mrelease(int nPidfd, unsigned int uFlags);

#ifdef __cplusplus
}
#endif
