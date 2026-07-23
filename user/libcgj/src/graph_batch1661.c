/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1661: PID integer hash (u32).
 *
 * Surface (unique symbols):
 *   uint32_t gj_pid_hash_u32(uint32_t pid);
 *     — Avalanche mix of a process id into a uint32 table key. Pure
 *       data; not a cryptographic hash. pid 0 is a valid input.
 *   uint32_t __gj_pid_hash_u32  (alias)
 *   __libcgj_batch1661_marker = "libcgj-batch1661"
 *
 * Does NOT redefine prior batch symbols — avoid multi-def.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stdint.h>

const char __libcgj_batch1661_marker[] = "libcgj-batch1661";

/* ---- freestanding helpers ---------------------------------------------- */

/*
 * SplitMix-style 32-bit finalizer. Constants are well-known public
 * avalanche mixers; implementation is clean-room.
 */
static uint32_t
b1661_mix32(uint32_t u32X)
{
	u32X ^= u32X >> 16;
	u32X *= 0x7feb352du;
	u32X ^= u32X >> 15;
	u32X *= 0x846ca68bu;
	u32X ^= u32X >> 16;
	return u32X;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_pid_hash_u32 — map a PID into a well-distributed uint32.
 *
 * Suitable for open-addressing / bucket selection over process tables.
 * Pure arithmetic; no /proc, no kernel state.
 */
uint32_t
gj_pid_hash_u32(uint32_t u32Pid)
{
	return b1661_mix32(u32Pid);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_pid_hash_u32(uint32_t u32Pid)
    __attribute__((alias("gj_pid_hash_u32")));
