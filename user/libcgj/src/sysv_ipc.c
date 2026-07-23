/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * SysV IPC soft wrappers: shm/sem/msg syscall (x86_64 Linux numbers).
 * ftok lives in graph_batch13 — keep it out. Clean-room public ABI.
 */
#include <errno.h>
#include <stdarg.h>
#include <stddef.h>
#include <stdint.h>
#include <sys/msg.h>
#include <sys/sem.h>
#include <sys/shm.h>
#include <sys/types.h>
#include <time.h>

#define NR_shmget     29
#define NR_shmat      30
#define NR_shmctl     31
#define NR_shmdt      67
#define NR_msgget     68
#define NR_msgsnd     69
#define NR_msgrcv     70
#define NR_msgctl     71
#define NR_semget     64
#define NR_semop      65
#define NR_semctl     66
#define NR_semtimedop 220

static long
sys6(long nr, long a0, long a1, long a2, long a3, long a4, long a5)
{
    register long r10 __asm__("r10") = a3;
    register long r8 __asm__("r8") = a4;
    register long r9 __asm__("r9") = a5;
    long ret;

    __asm__ volatile("syscall"
                     : "=a"(ret)
                     : "a"(nr), "D"(a0), "S"(a1), "d"(a2), "r"(r10), "r"(r8),
                       "r"(r9)
                     : "rcx", "r11", "memory");
    return ret;
}

static long
sys_ret(long r)
{
    if (r < 0 && r > -4096) {
        errno = (int)(-r);
        return -1;
    }
    return r;
}

int
shmget(key_t key, size_t cb, int nShmflg)
{
    return (int)sys_ret(
        sys6(NR_shmget, (long)key, (long)cb, nShmflg, 0, 0, 0));
}

void *
shmat(int nShmid, const void *pShmaddr, int nShmflg)
{
    long r;

    if (nShmid < 0) {
        errno = EINVAL;
        return (void *)(intptr_t)-1;
    }
    r = sys6(NR_shmat, nShmid, (long)(uintptr_t)pShmaddr, nShmflg, 0, 0, 0);
    if (r < 0 && r > -4096) {
        errno = (int)(-r);
        return (void *)(intptr_t)-1;
    }
    return (void *)(uintptr_t)r;
}

int
shmdt(const void *pShmaddr)
{
    if (pShmaddr == NULL) {
        errno = EINVAL;
        return -1;
    }
    return (int)sys_ret(
        sys6(NR_shmdt, (long)(uintptr_t)pShmaddr, 0, 0, 0, 0, 0));
}

int
shmctl(int nShmid, int nCmd, struct shmid_ds *pBuf)
{
    if (nShmid < 0) {
        errno = EINVAL;
        return -1;
    }
    /* IPC_STAT / IPC_SET require buffer; IPC_RMID may pass NULL. */
    if (pBuf == NULL && (nCmd == IPC_STAT || nCmd == IPC_SET)) {
        errno = EFAULT;
        return -1;
    }
    return (int)sys_ret(
        sys6(NR_shmctl, nShmid, nCmd, (long)(uintptr_t)pBuf, 0, 0, 0));
}

int
semget(key_t key, int nNsems, int nSemflg)
{
    if (nNsems < 0) {
        errno = EINVAL;
        return -1;
    }
    return (int)sys_ret(
        sys6(NR_semget, (long)key, nNsems, nSemflg, 0, 0, 0));
}

int
semop(int nSemid, struct sembuf *pSops, size_t nNsops)
{
    if (nSemid < 0 || pSops == NULL || nNsops == 0) {
        errno = EINVAL;
        return -1;
    }
    return (int)sys_ret(sys6(NR_semop, nSemid, (long)(uintptr_t)pSops,
                             (long)nNsops, 0, 0, 0));
}

int
semtimedop(int nSemid, struct sembuf *pSops, size_t nNsops,
           const struct timespec *pTimeout)
{
    long r;

    if (nSemid < 0 || pSops == NULL || nNsops == 0) {
        errno = EINVAL;
        return -1;
    }
    r = sys6(NR_semtimedop, nSemid, (long)(uintptr_t)pSops, (long)nNsops,
             (long)(uintptr_t)pTimeout, 0, 0);
    if (r < 0 && r > -4096 && (int)(-r) == ENOSYS) {
        /* Soft fallback: ignore timeout when kernel lacks semtimedop. */
        if (pTimeout == NULL) {
            return semop(nSemid, pSops, nNsops);
        }
        /* Timed path unavailable — still try untimed if timeout is zero. */
        if (pTimeout->tv_sec == 0 && pTimeout->tv_nsec == 0) {
            return semop(nSemid, pSops, nNsops);
        }
        errno = ENOSYS;
        return -1;
    }
    return (int)sys_ret(r);
}

int
semctl(int nSemid, int nSemnum, int nCmd, ...)
{
    va_list ap;
    long arg = 0;

    if (nSemid < 0) {
        errno = EINVAL;
        return -1;
    }
    va_start(ap, nCmd);
    /* Soft: always pull a long-sized 4th arg (union semun / int / ptr). */
    arg = va_arg(ap, long);
    va_end(ap);
    return (int)sys_ret(sys6(NR_semctl, nSemid, nSemnum, nCmd, arg, 0, 0));
}

int
msgget(key_t key, int nMsgflg)
{
    return (int)sys_ret(sys6(NR_msgget, (long)key, nMsgflg, 0, 0, 0, 0));
}

int
msgsnd(int nMsqid, const void *pMsgp, size_t cbMsgsz, int nMsgflg)
{
    if (nMsqid < 0 || pMsgp == NULL) {
        errno = EINVAL;
        return -1;
    }
    return (int)sys_ret(sys6(NR_msgsnd, nMsqid, (long)(uintptr_t)pMsgp,
                             (long)cbMsgsz, nMsgflg, 0, 0));
}

ssize_t
msgrcv(int nMsqid, void *pMsgp, size_t cbMsgsz, long nMsgtyp, int nMsgflg)
{
    if (nMsqid < 0 || pMsgp == NULL) {
        errno = EINVAL;
        return -1;
    }
    return (ssize_t)sys_ret(sys6(NR_msgrcv, nMsqid, (long)(uintptr_t)pMsgp,
                                 (long)cbMsgsz, nMsgtyp, nMsgflg, 0));
}

int
msgctl(int nMsqid, int nCmd, struct msqid_ds *pBuf)
{
    if (nMsqid < 0) {
        errno = EINVAL;
        return -1;
    }
    if (pBuf == NULL && (nCmd == IPC_STAT || nCmd == IPC_SET)) {
        errno = EFAULT;
        return -1;
    }
    return (int)sys_ret(
        sys6(NR_msgctl, nMsqid, nCmd, (long)(uintptr_t)pBuf, 0, 0, 0));
}
