/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Clean-room Linux libaio-shaped surface (syscall wrappers). Not GPL libaio.
 * Not GNU glibc source; dual MIT OR Apache-2.0 only.
 *
 * Scope
 * -----
 * Public declarations for the libcgj (GreenJade freestanding libc) surface
 * matching glibc-shaped include path <libaio.h>. Constants and type sizes
 * follow Linux LP64 (x86_64 / aarch64 product) where ABI numbers matter.
 *
 * Design notes
 * ------------
 * Pure C11 headers only — no inline runtime beyond macros. Implementations
 * live under user/libcgj/src/. Symbol versions are described by libc.map
 * (GLIBC_2.* nodes) for staged libc.so.6.
 *
 * Non-goals
 * ---------
 * Full POSIX/Linux completeness; stubs and soft fills may return ENOSYS
 * until the hybrid ABI path is wired. See docs/GLIBC_COMPAT.md.
 *
 * Soft residual (C2 libcgj libaio.h; Soft!=product; G-AC-1; Dual DoD A/B OPEN):
 *   soft     = io_context/iocb + io_setup/submit/getevents/destroy/cancel wrappers;
 *              freestanding may soft-fill ENOSYS without kernel AIO rings
 *   product  = UDX/DDI + hot/cold ABI hosts; not freestanding class; not .ko AC
 *   honesty  = libaio decls != product async I/O completeness; agent PASS != Dual
 *              DoD close; stamp-free bar honesty (no version stamp); no GPL libaio
 *   law      = dual MIT OR Apache-2.0; no GPL; H2 no stamp storms
 * greppable: libcgj: libaio soft residual Soft!=product
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
