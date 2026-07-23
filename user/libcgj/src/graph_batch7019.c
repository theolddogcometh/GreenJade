/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch7019: getrandom flags errorish/unknown stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_grnd_flags_errorish_u_7019(uint32_t flags);
 *     - Return 1 if flags has any unknown bits outside known GRND_*
 *       mask (0x7), else 0.
 *   uint32_t __gj_grnd_flags_errorish_u_7019  (alias)
 *   __libcgj_batch7019_marker = "libcgj-batch7019"
 *
 * Exclusive continuum CREATE-ONLY (7011-7020: getrandom flags stubs —
 * nonblock_id, random_id, insecure_id, has_nonblock, has_random,
 * has_insecure, flags_ok, flags_pack, flags_errorish, batch_id_7020).
 * Unique gj_grnd_flags_errorish_u_7019 surface only; no multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch7019_marker[] = "libcgj-batch7019";

/*
 * Known getrandom flags mask:
 *   GRND_NONBLOCK | GRND_RANDOM | GRND_INSECURE = 0x7
 */
#define B7019_GRND_KNOWN_MASK ((uint32_t)0x7u)

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b7019_grnd_flags_errorish(uint32_t u32Flags)
{
	return ((u32Flags & ~B7019_GRND_KNOWN_MASK) != 0u) ? 1u : 0u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_grnd_flags_errorish_u_7019 - 1 if flags has unknown bits.
 *
 * flags: getrandom(2) flags argument
 *
 * Returns 1 when any bit outside {0x1, 0x2, 0x4} is set, else 0.
 * Soft inverse of flags_ok; does not call getrandom. No parent wires.
 */
uint32_t
gj_grnd_flags_errorish_u_7019(uint32_t u32Flags)
{
	(void)NULL;
	return b7019_grnd_flags_errorish(u32Flags);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_grnd_flags_errorish_u_7019(uint32_t u32Flags)
    __attribute__((alias("gj_grnd_flags_errorish_u_7019")));
