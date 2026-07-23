/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Minimal Linux io_uring ABI shapes for syscall wrappers. Not kernel headers.
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
