/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch7829: wordexp flags errorish/unknown stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_wordexp_flags_errorish_u_7829(uint32_t flags);
 *     - Return 1 if flags has any unknown bits outside known WRDE_*
 *       mask (0x7), else 0.
 *   uint32_t __gj_wordexp_flags_errorish_u_7829  (alias)
 *   __libcgj_batch7829_marker = "libcgj-batch7829"
 *
 * Exclusive continuum CREATE-ONLY (7821-7830: wordexp flags stubs —
 * dooffs_id, append_id, nocmd_id, has_dooffs, has_append, has_nocmd,
 * flags_ok, flags_pack, flags_errorish, batch_id_7830).
 * Unique gj_wordexp_flags_errorish_u_7829 surface only; no multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch7829_marker[] = "libcgj-batch7829";

/*
 * Known wordexp flag bits for this continuum:
 *   WRDE_DOOFFS | WRDE_APPEND | WRDE_NOCMD = 0x7
 */
#define B7829_WRDE_KNOWN_MASK ((uint32_t)0x7u)

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b7829_wordexp_flags_errorish(uint32_t u32Flags)
{
	return ((u32Flags & ~B7829_WRDE_KNOWN_MASK) != 0u) ? 1u : 0u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_wordexp_flags_errorish_u_7829 - 1 if flags has unknown bits.
 *
 * flags: wordexp(3) flags argument
 *
 * Returns 1 when any bit outside {0x1, 0x2, 0x4} is set, else 0.
 * Soft inverse of flags_ok; does not call wordexp(3). No parent wires.
 */
uint32_t
gj_wordexp_flags_errorish_u_7829(uint32_t u32Flags)
{
	(void)NULL;
	return b7829_wordexp_flags_errorish(u32Flags);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_wordexp_flags_errorish_u_7829(uint32_t u32Flags)
    __attribute__((alias("gj_wordexp_flags_errorish_u_7829")));
