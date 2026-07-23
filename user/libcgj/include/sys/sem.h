/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Clean-room glibc-shaped sys/sem.h (subset). Not GNU glibc.
 */
#pragma once

#include <stddef.h>
#include <sys/ipc.h>
#include <sys/types.h>
#include <time.h>

#ifdef __cplusplus
extern "C" {
#endif

#define GETPID  11
#define GETVAL  12
#define GETALL  13
#define GETNCNT 14
#define GETZCNT 15
#define SETVAL  16
#define SETALL  17

#define SEM_STAT 18
#define SEM_INFO 19

struct sembuf {
    unsigned short sem_num;
    short          sem_op;
    short          sem_flg;
};

struct semid_ds {
    struct ipc_perm sem_perm;
    time_t          sem_otime;
    unsigned long   __unused1;
    time_t          sem_ctime;
    unsigned long   __unused2;
    unsigned long   sem_nsems;
    unsigned long   __unused3;
    unsigned long   __unused4;
};

union semun {
    int             val;
    struct semid_ds *buf;
    unsigned short  *array;
    struct seminfo  *__buf;
};

struct seminfo {
    int semmap;
    int semmni;
    int semmns;
    int semmnu;
    int semmsl;
    int semopm;
    int semume;
    int semusz;
    int semvmx;
    int semaem;
};

int semget(key_t key, int nNsems, int nSemflg);
int semop(int nSemid, struct sembuf *pSops, size_t nNsops);
int semtimedop(int nSemid, struct sembuf *pSops, size_t nNsops,
               const struct timespec *pTimeout);
int semctl(int nSemid, int nSemnum, int nCmd, ...);

#ifdef __cplusplus
}
#endif
