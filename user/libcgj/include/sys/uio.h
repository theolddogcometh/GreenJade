/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Clean-room glibc-shaped sys/uio.h (subset). Not GNU glibc.
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
