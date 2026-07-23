/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch7789: glob flags errorish/unknown stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_glob_flags_errorish_u_7789(uint32_t flags);
 *     - Return 1 if flags has any unknown bits outside known GLOB_*
 *       mask (0x13), else 0.
 *   uint32_t __gj_glob_flags_errorish_u_7789  (alias)
 *   __libcgj_batch7789_marker = "libcgj-batch7789"
 *
 * Exclusive continuum CREATE-ONLY (7781-7790: glob flags stubs —
 * err_id, mark_id, nocheck_id, has_err, has_mark, has_nocheck,
 * flags_ok, flags_pack, flags_errorish, batch_id_7790).
 * Unique gj_glob_flags_errorish_u_7789 surface only; no multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch7789_marker[] = "libcgj-batch7789";

/*
 * Known glob flag bits for this continuum:
 *   GLOB_ERR | GLOB_MARK | GLOB_NOCHECK = 0x13
 */
#define B7789_GLOB_KNOWN_MASK ((uint32_t)0x13u)

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b7789_glob_flags_errorish(uint32_t u32Flags)
{
	return ((u32Flags & ~B7789_GLOB_KNOWN_MASK) != 0u) ? 1u : 0u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_glob_flags_errorish_u_7789 - 1 if flags has unknown bits.
 *
 * flags: glob(3) flags argument
 *
 * Returns 1 when any bit outside {0x1, 0x2, 0x10} is set, else 0.
 * Soft inverse of flags_ok; does not call glob(3). No parent wires.
 */
uint32_t
gj_glob_flags_errorish_u_7789(uint32_t u32Flags)
{
	(void)NULL;
	return b7789_glob_flags_errorish(u32Flags);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_glob_flags_errorish_u_7789(uint32_t u32Flags)
    __attribute__((alias("gj_glob_flags_errorish_u_7789")));
