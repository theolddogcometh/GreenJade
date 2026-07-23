/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Clean-room glibc-shaped <sys/uio.h> for libcgj (GreenJade freestanding libc).
 * Not GNU glibc source; dual MIT OR Apache-2.0 only.
 *
 * Scope
 * -----
 * struct iovec, readv/writev (and preadv/pwritev when present). Shared by
 * sockets, splice, and process_madvise.
 *
 * Design notes
 * ------------
 * iov_base/iov_len layout is LP64 Linux. Total length across vectors is
 * limited by SSIZE_MAX / kernel caps.
 *
 * Non-goals
 * ---------
 * Guaranteed atomicity of multi-vector writes across all fd types.
 */
#pragma once

#include <stddef.h>
#include <sys/types.h>

#ifdef __cplusplus
extern "C" {
#endif

struct iovec {
    void  *iov_base;
    size_t iov_len;
};

ssize_t readv(int nFd, const struct iovec *pIov, int nIovcnt);
ssize_t writev(int nFd, const struct iovec *pIov, int nIovcnt);
ssize_t preadv(int nFd, const struct iovec *pIov, int nIovcnt, off_t off);
ssize_t pwritev(int nFd, const struct iovec *pIov, int nIovcnt, off_t off);
ssize_t preadv2(int nFd, const struct iovec *pIov, int nIovcnt, off_t off,
                int nFlags);
ssize_t pwritev2(int nFd, const struct iovec *pIov, int nIovcnt, off_t off,
                 int nFlags);
ssize_t process_vm_readv(pid_t pid, const struct iovec *pLocal,
                         unsigned long cLocal, const struct iovec *pRemote,
                         unsigned long cRemote, unsigned long uFlags);
ssize_t process_vm_writev(pid_t pid, const struct iovec *pLocal,
                          unsigned long cLocal, const struct iovec *pRemote,
                          unsigned long cRemote, unsigned long uFlags);

#ifdef __cplusplus
}
#endif
