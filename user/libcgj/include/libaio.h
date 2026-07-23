/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Clean-room Linux libaio-shaped surface (syscall wrappers). Not GPL libaio.
 */
#pragma once

#include <signal.h>
#include <stdint.h>
#include <sys/types.h>
#include <time.h>

#ifdef __cplusplus
extern "C" {
#endif

struct io_context;
typedef struct io_context *io_context_t;

enum {
    IO_CMD_PREAD  = 0,
    IO_CMD_PWRITE = 1,
    IO_CMD_FSYNC  = 2,
    IO_CMD_FDSYNC = 3,
    IO_CMD_POLL   = 5,
    IO_CMD_NOOP   = 6,
    IO_CMD_PREADV = 7,
    IO_CMD_PWRITEV = 8
};

struct iocb {
    void     *data;
    unsigned  key;
    short     aio_lio_opcode;
    short     aio_reqprio;
    int       aio_fildes;
    union {
        struct {
            void   *buf;
            unsigned long nbytes;
            long long offset;
        } c;
    } u;
};

struct io_event {
    void          *data;
    struct iocb   *obj;
    long long      res;
    long long      res2;
};

int io_setup(unsigned nNr, io_context_t *pCtx);
int io_destroy(io_context_t ctx);
int io_submit(io_context_t ctx, long nNr, struct iocb **ppIocbs);
int io_cancel(io_context_t ctx, struct iocb *pIocb, struct io_event *pEvent);
int io_getevents(io_context_t ctx, long nMin, long nNr, struct io_event *pEvents,
                 struct timespec *pTimeout);

#ifdef __cplusplus
}
#endif
