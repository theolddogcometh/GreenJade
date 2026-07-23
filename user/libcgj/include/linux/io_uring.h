/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Clean-room Linux-shaped <linux/io_uring.h> for libcgj / GreenJade hybrid I/O.
 * Not Linux kernel UAPI headers verbatim; dual MIT OR Apache-2.0 only.
 *
 * Scope
 * -----
 * Minimal SQE/CQE structures and opcodes for the product min+mmap io_uring
 * path (kernel/syscall/io_uring_min.c). Enough for apps probing IORING_*.
 *
 * Design notes
 * ------------
 * Keep field order and sizes compatible with the kernel bring-up
 * implementation; expand carefully when new opcodes are wired.
 *
 * Non-goals
 * ---------
 * Full upstream io_uring feature set (registered buffers, all ops).
 */
#pragma once

#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

struct io_uring_params {
    uint32_t sq_entries;
    uint32_t cq_entries;
    uint32_t flags;
    uint32_t sq_thread_cpu;
    uint32_t sq_thread_idle;
    uint32_t features;
    uint32_t wq_fd;
    uint32_t resv[3];
    /* sq/cq offsets omitted in bring-up (syscall arg only) */
    uint8_t  pad[120];
};

int io_uring_setup(unsigned nEntries, struct io_uring_params *pParams);
int io_uring_enter(unsigned nFd, unsigned nToSubmit, unsigned nMinComplete,
                   unsigned nFlags, void *pSig, size_t cbSig);
int io_uring_register(unsigned nFd, unsigned nOpcode, void *pArg,
                      unsigned nNrArgs);

#ifdef __cplusplus
}
#endif
