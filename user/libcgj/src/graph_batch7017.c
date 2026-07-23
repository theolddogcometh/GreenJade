/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch7017: getrandom flags validity stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_grnd_flags_ok_u_7017(uint32_t flags);
 *     - Return 1 if flags uses only known GRND_* bits (0x1|0x2|0x4),
 *       else 0.
 *   uint32_t __gj_grnd_flags_ok_u_7017  (alias)
 *   __libcgj_batch7017_marker = "libcgj-batch7017"
 *
 * Exclusive continuum CREATE-ONLY (7011-7020: getrandom flags stubs —
 * nonblock_id, random_id, insecure_id, has_nonblock, has_random,
 * has_insecure, flags_ok, flags_pack, flags_errorish, batch_id_7020).
 * Unique gj_grnd_flags_ok_u_7017 surface only; no multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch7017_marker[] = "libcgj-batch7017";

/*
 * Known getrandom flags mask:
 *   GRND_NONBLOCK (0x1) | GRND_RANDOM (0x2) | GRND_INSECURE (0x4) = 0x7
 */
#define B7017_GRND_KNOWN_MASK ((uint32_t)0x7u)

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b7017_grnd_flags_ok(uint32_t u32Flags)
{
	return ((u32Flags & ~B7017_GRND_KNOWN_MASK) == 0u) ? 1u : 0u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_grnd_flags_ok_u_7017 - 1 if flags has no unknown bits.
 *
 * flags: getrandom(2) flags argument
 *
 * Returns 1 when only bits in {0x1, 0x2, 0x4} are set (including 0),
 * else 0. Soft catalog check; does not call getrandom. No parent wires.
 */
uint32_t
gj_grnd_flags_ok_u_7017(uint32_t u32Flags)
{
	(void)NULL;
	return b7017_grnd_flags_ok(u32Flags);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_grnd_flags_ok_u_7017(uint32_t u32Flags)
    __attribute__((alias("gj_grnd_flags_ok_u_7017")));
