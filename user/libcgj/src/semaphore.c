/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * POSIX semaphores — process-private and process-shared (futex).
 * Named: sem_open via shm_open + mmap. Not GNU glibc.
 */
#include <errno.h>
#include <fcntl.h>
#include <semaphore.h>
#include <stdarg.h>
#include <stdint.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <time.h>
#include <unistd.h>

#define NR_futex 202
#define FUTEX_WAIT 0
#define FUTEX_WAKE 1
#define FUTEX_PRIVATE_FLAG 128

#define CGJ_SEM_MAGIC 0x53454d31 /* 'SEM1' */

static long
sys6(long nr, long a0, long a1, long a2, long a3, long a4, long a5)
{
    register long r10 __asm__("r10") = a3;
    register long r8 __asm__("r8") = a4;
    register long r9 __asm__("r9") = a5;
    long ret;

    __asm__ volatile(
        "syscall"
        : "=a"(ret)
        : "a"(nr), "D"(a0), "S"(a1), "d"(a2), "r"(r10), "r"(r8), "r"(r9)
        : "rcx", "r11", "memory");
    return ret;
}

static int
sem_ok(const sem_t *pSem)
{
    return pSem != NULL && pSem->nMagic == CGJ_SEM_MAGIC;
}

static int
futex_op_wait(sem_t *pSem, int nExpect, const struct timespec *pRel)
{
    int nOp = FUTEX_WAIT;

    if (pSem->nPshared == 0) {
        nOp |= FUTEX_PRIVATE_FLAG;
    }
    return (int)sys6(NR_futex, (long)(uintptr_t)&pSem->nValue, nOp, nExpect,
                     (long)(uintptr_t)pRel, 0, 0);
}

static int
futex_op_wake(sem_t *pSem, int nCount)
{
    int nOp = FUTEX_WAKE;

    if (pSem->nPshared == 0) {
        nOp |= FUTEX_PRIVATE_FLAG;
    }
    return (int)sys6(NR_futex, (long)(uintptr_t)&pSem->nValue, nOp, nCount, 0, 0,
                     0);
}

int
sem_init(sem_t *pSem, int nPshared, unsigned uValue)
{
    if (pSem == NULL) {
        errno = EINVAL;
        return -1;
    }
    if (uValue > 0x7fffffffU) {
        errno = EINVAL;
        return -1;
    }
    pSem->nValue = (int)uValue;
    pSem->nPshared = (nPshared != 0) ? 1 : 0;
    pSem->nMagic = CGJ_SEM_MAGIC;
    pSem->nPad = 0;
    return 0;
}

int
sem_destroy(sem_t *pSem)
{
    if (!sem_ok(pSem)) {
        errno = EINVAL;
        return -1;
    }
    pSem->nMagic = 0;
    pSem->nValue = 0;
    return 0;
}

int
sem_trywait(sem_t *pSem)
{
    int nCur;

    if (!sem_ok(pSem)) {
        errno = EINVAL;
        return -1;
    }
    for (;;) {
        nCur = pSem->nValue;
        if (nCur <= 0) {
            errno = EAGAIN;
            return -1;
        }
        if (__sync_bool_compare_and_swap(&pSem->nValue, nCur, nCur - 1)) {
            return 0;
        }
    }
}

int
sem_wait(sem_t *pSem)
{
    int nCur;
    long r;

    if (!sem_ok(pSem)) {
        errno = EINVAL;
        return -1;
    }
    for (;;) {
        nCur = pSem->nValue;
        if (nCur > 0) {
            if (__sync_bool_compare_and_swap(&pSem->nValue, nCur, nCur - 1)) {
                return 0;
            }
            continue;
        }
        r = futex_op_wait(pSem, 0, NULL);
        if (r < 0 && r != -11 && r != -4) {
            (void)r;
        }
    }
}

int
sem_timedwait(sem_t *pSem, const struct timespec *pAbs)
{
    int nCur;
    long r;
    struct timespec now;
    struct timespec rel;

    if (!sem_ok(pSem) || pAbs == NULL) {
        errno = EINVAL;
        return -1;
    }
    for (;;) {
        nCur = pSem->nValue;
        if (nCur > 0) {
            if (__sync_bool_compare_and_swap(&pSem->nValue, nCur, nCur - 1)) {
                return 0;
            }
            continue;
        }
        if (clock_gettime(CLOCK_REALTIME, &now) != 0) {
            return -1;
        }
        rel.tv_sec = pAbs->tv_sec - now.tv_sec;
        rel.tv_nsec = pAbs->tv_nsec - now.tv_nsec;
        if (rel.tv_nsec < 0) {
            rel.tv_sec--;
            rel.tv_nsec += 1000000000L;
        }
        if (rel.tv_sec < 0 || (rel.tv_sec == 0 && rel.tv_nsec <= 0)) {
            errno = ETIMEDOUT;
            return -1;
        }
        r = futex_op_wait(pSem, 0, &rel);
        if (r < 0 && (int)(-r) == ETIMEDOUT) {
            errno = ETIMEDOUT;
            return -1;
        }
        (void)r;
    }
}

int
sem_post(sem_t *pSem)
{
    int nCur;

    if (!sem_ok(pSem)) {
        errno = EINVAL;
        return -1;
    }
    for (;;) {
        nCur = pSem->nValue;
        if (nCur >= 0x7fffffff) {
            errno = EOVERFLOW;
            return -1;
        }
        if (__sync_bool_compare_and_swap(&pSem->nValue, nCur, nCur + 1)) {
            (void)futex_op_wake(pSem, 1);
            return 0;
        }
    }
}

int
sem_getvalue(sem_t *pSem, int *pSval)
{
    if (!sem_ok(pSem) || pSval == NULL) {
        errno = EINVAL;
        return -1;
    }
    *pSval = pSem->nValue;
    return 0;
}

sem_t *
sem_open(const char *szName, int nFlags, ...)
{
    mode_t mode = 0600;
    unsigned uValue = 0;
    int nFd;
    int nCreated = 0;
    sem_t *pMap;
    struct stat st;

    if (szName == NULL || szName[0] == '\0') {
        errno = EINVAL;
        return SEM_FAILED;
    }
    if ((nFlags & O_CREAT) != 0) {
        va_list ap;

        va_start(ap, nFlags);
        mode = (mode_t)va_arg(ap, unsigned); /* mode_t may promote */
        uValue = va_arg(ap, unsigned);
        va_end(ap);
    }

    nFd = shm_open(szName, nFlags | O_RDWR, mode);
    if (nFd < 0 && (nFlags & O_CREAT) != 0 && errno == ENOENT) {
        nFd = shm_open(szName, nFlags | O_RDWR | O_CREAT, mode);
        nCreated = 1;
    } else if (nFd >= 0 && (nFlags & O_CREAT) != 0 && (nFlags & O_EXCL) != 0) {
        /* already exists with O_EXCL — shm_open should have failed; treat ok */
    }
    if (nFd < 0) {
        return SEM_FAILED;
    }

    if (fstat(nFd, &st) == 0 && st.st_size < (off_t)sizeof(sem_t)) {
        if (ftruncate(nFd, (off_t)sizeof(sem_t)) != 0) {
            (void)close(nFd);
            return SEM_FAILED;
        }
        nCreated = 1;
    } else if ((nFlags & O_CREAT) != 0) {
        if (ftruncate(nFd, (off_t)sizeof(sem_t)) != 0 && errno != EINVAL) {
            /* ignore if already sized */
        }
        if (fstat(nFd, &st) == 0 && st.st_size == 0) {
            (void)ftruncate(nFd, (off_t)sizeof(sem_t));
            nCreated = 1;
        }
    }

    pMap = (sem_t *)mmap(NULL, sizeof(sem_t), PROT_READ | PROT_WRITE, MAP_SHARED,
                         nFd, 0);
    (void)close(nFd);
    if (pMap == MAP_FAILED || pMap == NULL) {
        return SEM_FAILED;
    }

    if (nCreated || !sem_ok(pMap)) {
        if (sem_init(pMap, 1, uValue) != 0) {
            (void)munmap(pMap, sizeof(sem_t));
            return SEM_FAILED;
        }
    }
    return pMap;
}

int
sem_close(sem_t *pSem)
{
    if (pSem == NULL || pSem == SEM_FAILED) {
        errno = EINVAL;
        return -1;
    }
    if (munmap(pSem, sizeof(sem_t)) != 0) {
        return -1;
    }
    return 0;
}

int
sem_unlink(const char *szName)
{
    return shm_unlink(szName);
}
