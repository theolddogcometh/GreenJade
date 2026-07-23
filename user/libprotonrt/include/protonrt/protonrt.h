/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Clean-room Linux personality helpers (cold path / host).
 * Pure C11. Dual license: MIT OR Apache-2.0 (no GPL source).
 *
 * Linked into the kernel smoke path as cold stubs; product shape is a
 * userspace lib + door server. See:
 *   docs/PROTON_PERSONALITY.md
 *   docs/LINUX_ABI_HYBRID.md
 *
 * ABI: keep proton_rt_query / protonrt_cold_linux signatures stable.
 * Version and feature bits may grow; do not reorder or shrink fields.
 */
#pragma once

#include <stdint.h>

#define PROTON_RT_VERSION_MAJOR 1
#define PROTON_RT_VERSION_MINOR 10

/* Feature bits advertised by proton_rt_query (libprotonrt 1.10). */
#define PROTON_FEAT_FUTEX         (1u << 0)
#define PROTON_FEAT_NAMED_SHM     (1u << 1)
#define PROTON_FEAT_SOCKETPAIR    (1u << 2)
#define PROTON_FEAT_EVENTFD       (1u << 3)
#define PROTON_FEAT_EPOLL         (1u << 4)
#define PROTON_FEAT_PIPE          (1u << 5)
#define PROTON_FEAT_MEMFD         (1u << 6)
#define PROTON_FEAT_LINUX_COMPAT  (1u << 10)

struct proton_rt_info {
    uint32_t u32Version;  /* (major << 16) | minor */
    uint32_t u32Features; /* PROTON_FEAT_* mask */
};

/**
 * Fill *pOut with runtime version and feature bits.
 * Returns 0 on success, -1 if pOut is NULL.
 */
int proton_rt_query(struct proton_rt_info *pOut);

/**
 * Handle one cold Linux x86_64 syscall for host / early smoke.
 *
 * Args: Linux NR in u64Nr; a0..a5 are the usual syscall registers.
 * Returns a Linux-style signed result (negative errno on failure).
 *
 * Unimplemented NRs in this library return -ENOSYS (38).
 *
 * Soft depth (1.10): freestanding host/smoke soft path for A0-shaped
 * surfaces — FD table with pipe/eventfd/socketpair/epoll/memfd kinds,
 * write/dup/poll/ioctl/fsync/madvise soft success, identity/time/rlimit
 * probes. Kernel product path (protonrt_cold_link / vfs_ram / net_lo /
 * io_uring_min) owns real vfs and min rings before fall-through.
 *
 * Linked into kernel smoke via the tree build (cold_linux.c) or host.
 */
int64_t protonrt_cold_linux(uint64_t u64Nr, uint64_t a0, uint64_t a1,
                            uint64_t a2, uint64_t a3, uint64_t a4,
                            uint64_t a5);
