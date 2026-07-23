/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch7369: fallocate mode errorish/unknown stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_falloc_modes_errorish_u_7369(uint32_t mode);
 *     - Return 1 if mode has any unknown bits outside known FALLOC_FL_*
 *       mask (0x1B), else 0.
 *   uint32_t __gj_falloc_modes_errorish_u_7369  (alias)
 *   __libcgj_batch7369_marker = "libcgj-batch7369"
 *
 * Exclusive continuum CREATE-ONLY (7361-7370: fallocate mode stubs —
 * keep_size_id, punch_hole_id, collapse_range_id, zero_range_id,
 * has_keep_size, has_punch_hole, modes_ok, modes_pack, modes_errorish,
 * batch_id_7370).
 * Unique gj_falloc_modes_errorish_u_7369 surface only; no multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch7369_marker[] = "libcgj-batch7369";

/*
 * Known fallocate mode bits for this continuum:
 *   KEEP_SIZE | PUNCH_HOLE | COLLAPSE_RANGE | ZERO_RANGE = 0x1B
 */
#define B7369_FALLOC_KNOWN_MASK ((uint32_t)0x1Bu)

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b7369_falloc_modes_errorish(uint32_t u32Mode)
{
	return ((u32Mode & ~B7369_FALLOC_KNOWN_MASK) != 0u) ? 1u : 0u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_falloc_modes_errorish_u_7369 - 1 if mode has unknown bits.
 *
 * mode: fallocate(2) mode argument
 *
 * Returns 1 when any bit outside {0x01, 0x02, 0x08, 0x10} is set,
 * else 0. Soft inverse of modes_ok; does not call fallocate.
 * No parent wires.
 */
uint32_t
gj_falloc_modes_errorish_u_7369(uint32_t u32Mode)
{
	(void)NULL;
	return b7369_falloc_modes_errorish(u32Mode);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_falloc_modes_errorish_u_7369(uint32_t u32Mode)
    __attribute__((alias("gj_falloc_modes_errorish_u_7369")));
