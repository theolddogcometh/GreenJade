/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Clean-room glibc-shaped aio.h — sync freestanding fill-in (pread/pwrite).
 * Not GNU glibc.
 */
#pragma once

#include <signal.h>
#include <stddef.h>
#include <sys/types.h>
#include <time.h>

#ifdef __cplusplus
extern "C" {
#endif

struct aiocb {
    int             aio_fildes;
    int             aio_lio_opcode;
    int             aio_reqprio;
    volatile void  *aio_buf;
    size_t          aio_nbytes;
    struct sigevent aio_sigevent;
    off_t           aio_offset;
    char            __pad[32];
};

struct aioinit {
    int aio_threads;
    int aio_num;
    int aio_locks;
    int aio_usedba;
    int aio_debug;
    int aio_numusers;
    int aio_idle_time;
    int aio_reserved;
};

#define AIO_CANCELED    0
#define AIO_NOTCANCELED 1
#define AIO_ALLDONE     2

#define LIO_READ  0
#define LIO_WRITE 1
#define LIO_NOP   2

#define LIO_WAIT   0
#define LIO_NOWAIT 1

int     aio_read(struct aiocb *pCb);
int     aio_write(struct aiocb *pCb);
int     aio_error(const struct aiocb *pCb);
ssize_t aio_return(struct aiocb *pCb);
int     aio_cancel(int nFd, struct aiocb *pCb);
int     aio_suspend(const struct aiocb *const pList[], int nEnt,
                    const struct timespec *pTimeout);
int     aio_fsync(int nOp, struct aiocb *pCb);
void    aio_init(const struct aioinit *pInit);

int     aio_read64(struct aiocb *pCb);
int     aio_write64(struct aiocb *pCb);
int     aio_error64(const struct aiocb *pCb);
ssize_t aio_return64(struct aiocb *pCb);
int     aio_cancel64(int nFd, struct aiocb *pCb);
int     aio_suspend64(const struct aiocb *const pList[], int nEnt,
                      const struct timespec *pTimeout);
int     aio_fsync64(int nOp, struct aiocb *pCb);

#ifdef __cplusplus
}
#endif
