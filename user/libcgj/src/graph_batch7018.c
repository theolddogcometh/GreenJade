/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch7018: getrandom flags pack stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_grnd_flags_pack_u_7018(uint32_t want_nonblock,
 *                                      uint32_t want_random,
 *                                      uint32_t want_insecure);
 *     - Pack soft GRND_* wants into a flags mask (0x1|0x2|0x4).
 *   uint32_t __gj_grnd_flags_pack_u_7018  (alias)
 *   __libcgj_batch7018_marker = "libcgj-batch7018"
 *
 * Exclusive continuum CREATE-ONLY (7011-7020: getrandom flags stubs —
 * nonblock_id, random_id, insecure_id, has_nonblock, has_random,
 * has_insecure, flags_ok, flags_pack, flags_errorish, batch_id_7020).
 * Unique gj_grnd_flags_pack_u_7018 surface only; no multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch7018_marker[] = "libcgj-batch7018";

/* Soft pack bits matching Linux GRND_* layout. */
#define B7018_GRND_NONBLOCK ((uint32_t)0x1u)
#define B7018_GRND_RANDOM   ((uint32_t)0x2u)
#define B7018_GRND_INSECURE ((uint32_t)0x4u)

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b7018_grnd_flags_pack(uint32_t u32WantNonblock,
                      uint32_t u32WantRandom,
                      uint32_t u32WantInsecure)
{
	uint32_t u32Pack;

	u32Pack = 0u;
	if (u32WantNonblock != 0u)
		u32Pack |= B7018_GRND_NONBLOCK;
	if (u32WantRandom != 0u)
		u32Pack |= B7018_GRND_RANDOM;
	if (u32WantInsecure != 0u)
		u32Pack |= B7018_GRND_INSECURE;
	return u32Pack;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_grnd_flags_pack_u_7018 - soft-pack getrandom flag wants.
 *
 * want_nonblock: non-zero to set GRND_NONBLOCK (0x1)
 * want_random:   non-zero to set GRND_RANDOM (0x2)
 * want_insecure: non-zero to set GRND_INSECURE (0x4)
 *
 * Returns bitmask of requested GRND_* bits. Pure integer pack; does not
 * call getrandom. No parent wires.
 */
uint32_t
gj_grnd_flags_pack_u_7018(uint32_t u32WantNonblock,
                          uint32_t u32WantRandom,
                          uint32_t u32WantInsecure)
{
	(void)NULL;
	return b7018_grnd_flags_pack(u32WantNonblock, u32WantRandom,
	                             u32WantInsecure);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_grnd_flags_pack_u_7018(uint32_t u32WantNonblock,
                                     uint32_t u32WantRandom,
                                     uint32_t u32WantInsecure)
    __attribute__((alias("gj_grnd_flags_pack_u_7018")));
