/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch7367: fallocate mode validity stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_falloc_modes_ok_u_7367(uint32_t mode);
 *     - Return 1 if mode uses only known FALLOC_FL_* bits
 *       (0x01|0x02|0x08|0x10), else 0.
 *   uint32_t __gj_falloc_modes_ok_u_7367  (alias)
 *   __libcgj_batch7367_marker = "libcgj-batch7367"
 *
 * Exclusive continuum CREATE-ONLY (7361-7370: fallocate mode stubs —
 * keep_size_id, punch_hole_id, collapse_range_id, zero_range_id,
 * has_keep_size, has_punch_hole, modes_ok, modes_pack, modes_errorish,
 * batch_id_7370).
 * Unique gj_falloc_modes_ok_u_7367 surface only; no multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch7367_marker[] = "libcgj-batch7367";

/*
 * Known fallocate mode bits for this continuum (libcgj fcntl.h set):
 *   KEEP_SIZE (0x01) | PUNCH_HOLE (0x02) | COLLAPSE_RANGE (0x08) |
 *   ZERO_RANGE (0x10) = 0x1B
 */
#define B7367_FALLOC_KNOWN_MASK ((uint32_t)0x1Bu)

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b7367_falloc_modes_ok(uint32_t u32Mode)
{
	return ((u32Mode & ~B7367_FALLOC_KNOWN_MASK) == 0u) ? 1u : 0u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_falloc_modes_ok_u_7367 - 1 if mode has no unknown bits.
 *
 * mode: fallocate(2) mode argument
 *
 * Returns 1 when only bits in {0x01, 0x02, 0x08, 0x10} are set
 * (including 0), else 0. Soft catalog check; does not call fallocate.
 * No parent wires.
 */
uint32_t
gj_falloc_modes_ok_u_7367(uint32_t u32Mode)
{
	(void)NULL;
	return b7367_falloc_modes_ok(u32Mode);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_falloc_modes_ok_u_7367(uint32_t u32Mode)
    __attribute__((alias("gj_falloc_modes_ok_u_7367")));
