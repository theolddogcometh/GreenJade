/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch7787: glob flags validity stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_glob_flags_ok_u_7787(uint32_t flags);
 *     - Return 1 if flags uses only known GLOB_* bits
 *       (0x1|0x2|0x10), else 0.
 *   uint32_t __gj_glob_flags_ok_u_7787  (alias)
 *   __libcgj_batch7787_marker = "libcgj-batch7787"
 *
 * Exclusive continuum CREATE-ONLY (7781-7790: glob flags stubs —
 * err_id, mark_id, nocheck_id, has_err, has_mark, has_nocheck,
 * flags_ok, flags_pack, flags_errorish, batch_id_7790).
 * Unique gj_glob_flags_ok_u_7787 surface only; no multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch7787_marker[] = "libcgj-batch7787";

/*
 * Known glob flag bits for this continuum:
 *   GLOB_ERR (0x1) | GLOB_MARK (0x2) | GLOB_NOCHECK (0x10) = 0x13
 */
#define B7787_GLOB_KNOWN_MASK ((uint32_t)0x13u)

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b7787_glob_flags_ok(uint32_t u32Flags)
{
	return ((u32Flags & ~B7787_GLOB_KNOWN_MASK) == 0u) ? 1u : 0u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_glob_flags_ok_u_7787 - 1 if flags has no unknown bits.
 *
 * flags: glob(3) flags argument
 *
 * Returns 1 when only bits in {0x1, 0x2, 0x10} are set (including 0),
 * else 0. Soft catalog check; does not call glob(3). No parent wires.
 */
uint32_t
gj_glob_flags_ok_u_7787(uint32_t u32Flags)
{
	(void)NULL;
	return b7787_glob_flags_ok(u32Flags);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_glob_flags_ok_u_7787(uint32_t u32Flags)
    __attribute__((alias("gj_glob_flags_ok_u_7787")));
