/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Clean-room glibc-shaped sys/shm.h (subset). Not GNU glibc.
 */
#pragma once

#include <stddef.h>
#include <sys/ipc.h>
#include <sys/types.h>

#ifdef __cplusplus
extern "C" {
#endif

#define SHM_R 0400
#define SHM_W 0200

#define SHM_RDONLY 010000
#define SHM_RND    020000
#define SHM_REMAP  040000
#define SHM_EXEC   0100000

#define SHM_LOCK   11
#define SHM_UNLOCK 12
#define SHM_STAT   13
#define SHM_INFO   14

#define SHMLBA 4096

typedef unsigned long shmatt_t;

struct shmid_ds {
    struct ipc_perm shm_perm;
    size_t          shm_segsz;
    time_t          shm_atime;
    time_t          shm_dtime;
    time_t          shm_ctime;
    pid_t           shm_cpid;
    pid_t           shm_lpid;
    shmatt_t        shm_nattch;
    unsigned long   __unused1;
    unsigned long   __unused2;
};

int   shmget(key_t key, size_t cb, int nShmflg);
void *shmat(int nShmid, const void *pShmaddr, int nShmflg);
int   shmdt(const void *pShmaddr);
int   shmctl(int nShmid, int nCmd, struct shmid_ds *pBuf);

#ifdef __cplusplus
}
#endif
