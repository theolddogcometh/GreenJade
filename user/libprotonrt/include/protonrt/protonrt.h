/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Clean-room Linux personality helpers (cold path / host).
 * Pure C11. Dual license: MIT OR Apache-2.0 (no GPL source).
 * Soft!=product. G-AC-1: no Linux .ko product AC.
 *
 * Linked into the kernel smoke path as cold stubs; product shape is a
 * userspace lib + door server. See:
 *   docs/PROTON_PERSONALITY.md
 *   docs/LINUX_ABI_HYBRID.md
 *
 * ABI: keep proton_rt_query / protonrt_cold_linux signatures stable.
 * Version and feature bits may grow; do not reorder or shrink fields.
 *
 * C2 lib residual (Soft!=product; dual MIT OR Apache-2.0; stamp-free):
 *   Freestanding host/smoke soft surface for A0-shaped wine-class probes.
 *   Soft residual != product multi-server confine, != Dual DoD A/B close,
 *   != bar3, != Deck Top 50 PASS. Agent never closes Dual DoD.
 *   Dual DoD A (UDX USB) OPEN; Dual DoD B (UDX NIC) OPEN until DUT proof.
 *   Product path = hot+cold ABI + userspace UDX/DDI hosts (not this soft
 *   FD table depth alone). Kernel owners before fall-through:
 *   protonrt_cold_link / vfs_ram / net_lo / io_uring_min.
 *   H1: no IRQ/timer eth poll from this lib (run_loop_only elsewhere).
 *   H2: residual law comments only - no stamp storms; storm=0.
 *   H3: process-death thr AS not this unit; no clone into dead AS.
 *   Comment law: Soft!=product ASCII != only (never Softneq unicode);
 *   pure C block comments; no nested star-slash mid-comment.
 *   greppable: Soft!=product | G-AC-1 | dual_dod_a=OPEN | dual_dod_b=OPEN
 *   greppable: protonrt: soft residual | C2 residual | product_path=UDX_DDI
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
 * Soft residual advertise only; Soft!=product (never product DoD close).
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
 * Soft depth (1.10; Soft!=product; C2 lib residual): freestanding
 * host/smoke soft path for A0-shaped surfaces - FD table with
 * pipe/eventfd/socketpair/epoll/memfd kinds, write/dup/poll/ioctl/
 * fsync/madvise soft success, identity/time/rlimit probes.
 * Soft path does not close Dual DoD A/B (both OPEN), bar3, or multi-
 * server confine. Kernel product path (protonrt_cold_link / vfs_ram /
 * net_lo / io_uring_min) owns real vfs and min rings before fall-through.
 * G-AC-1: soft cold NRs are not .ko product AC. product_path=UDX_DDI.
 *
 * Linked into kernel smoke via the tree build (cold_linux.c) or host.
 */
int64_t protonrt_cold_linux(uint64_t u64Nr, uint64_t a0, uint64_t a1,
                            uint64_t a2, uint64_t a3, uint64_t a4,
                            uint64_t a5);
