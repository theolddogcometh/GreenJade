/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1662: TID integer hash (u64).
 *
 * Surface (unique symbols):
 *   uint64_t gj_tid_hash_u64(uint64_t tid);
 *     — Avalanche mix of a thread id into a uint64 table key. Pure
 *       data; not a cryptographic hash. tid 0 is a valid input.
 *   uint64_t __gj_tid_hash_u64  (alias)
 *   __libcgj_batch1662_marker = "libcgj-batch1662"
 *
 * Distinct from gj_pid_hash_u32 (batch1661). Does NOT redefine prior
 * batch symbols — avoid multi-def.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stdint.h>

const char __libcgj_batch1662_marker[] = "libcgj-batch1662";

/* ---- freestanding helpers ---------------------------------------------- */

/*
 * SplitMix64-style finalizer. Constants are well-known public
 * avalanche mixers; implementation is clean-room.
 */
static uint64_t
b1662_mix64(uint64_t u64X)
{
	u64X ^= u64X >> 30;
	u64X *= 0xbf58476d1ce4e5b9ull;
	u64X ^= u64X >> 27;
	u64X *= 0x94d049bb133111ebull;
	u64X ^= u64X >> 31;
	return u64X;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_tid_hash_u64 — map a TID into a well-distributed uint64.
 *
 * Suitable for thread-table bucketing and TID→slot probes. Pure
 * arithmetic; no task_struct, no TLS, no kernel state.
 */
uint64_t
gj_tid_hash_u64(uint64_t u64Tid)
{
	return b1662_mix64(u64Tid);
}

/* ---- underscored alias ------------------------------------------------- */

uint64_t __gj_tid_hash_u64(uint64_t u64Tid)
    __attribute__((alias("gj_tid_hash_u64")));
