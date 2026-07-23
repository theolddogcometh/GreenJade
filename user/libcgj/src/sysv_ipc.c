/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * SysV IPC graph: shm/sem/msg syscall wrappers (x86_64 Linux numbers).
 * Clean-room public ABI — no GPL source.
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

#define NR_shmget    29
#define NR_shmat     30
#define NR_shmctl    31
#define NR_shmdt     67
#define NR_msgget    68
#define NR_msgsnd    69
#define NR_msgrcv    70
#define NR_msgctl    71
#define NR_semget    64
#define NR_semop     65
#define NR_semctl    66
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
    return (int)sys_ret(sys6(NR_shmget, (long)key, (long)cb, nShmflg, 0, 0, 0));
}

void *
shmat(int nShmid, const void *pShmaddr, int nShmflg)
{
    long r;

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
    return (int)sys_ret(sys6(NR_shmdt, (long)(uintptr_t)pShmaddr, 0, 0, 0, 0,
                             0));
}

int
shmctl(int nShmid, int nCmd, struct shmid_ds *pBuf)
{
    return (int)sys_ret(sys6(NR_shmctl, nShmid, nCmd, (long)(uintptr_t)pBuf, 0,
                             0, 0));
}

int
semget(key_t key, int nNsems, int nSemflg)
{
    return (int)sys_ret(sys6(NR_semget, (long)key, nNsems, nSemflg, 0, 0, 0));
}

int
semop(int nSemid, struct sembuf *pSops, size_t nNsops)
{
    return (int)sys_ret(sys6(NR_semop, nSemid, (long)(uintptr_t)pSops,
                             (long)nNsops, 0, 0, 0));
}

int
semtimedop(int nSemid, struct sembuf *pSops, size_t nNsops,
           const struct timespec *pTimeout)
{
    long r;

    r = sys6(NR_semtimedop, nSemid, (long)(uintptr_t)pSops, (long)nNsops,
             (long)(uintptr_t)pTimeout, 0, 0);
    if (r < 0 && r > -4096 && (int)(-r) == ENOSYS) {
        if (pTimeout == NULL) {
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

    va_start(ap, nCmd);
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
    return (int)sys_ret(sys6(NR_msgsnd, nMsqid, (long)(uintptr_t)pMsgp,
                             (long)cbMsgsz, nMsgflg, 0, 0));
}

ssize_t
msgrcv(int nMsqid, void *pMsgp, size_t cbMsgsz, long nMsgtyp, int nMsgflg)
{
    return (ssize_t)sys_ret(sys6(NR_msgrcv, nMsqid, (long)(uintptr_t)pMsgp,
                                 (long)cbMsgsz, nMsgtyp, nMsgflg, 0));
}

int
msgctl(int nMsqid, int nCmd, struct msqid_ds *pBuf)
{
    return (int)sys_ret(sys6(NR_msgctl, nMsqid, nCmd, (long)(uintptr_t)pBuf, 0,
                             0, 0));
}
