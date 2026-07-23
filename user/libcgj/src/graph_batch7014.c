/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch7014: getrandom GRND_NONBLOCK presence stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_grnd_has_nonblock_u_7014(uint32_t flags);
 *     - Return 1 if flags has GRND_NONBLOCK (0x1) set, else 0.
 *   uint32_t __gj_grnd_has_nonblock_u_7014  (alias)
 *   __libcgj_batch7014_marker = "libcgj-batch7014"
 *
 * Exclusive continuum CREATE-ONLY (7011-7020: getrandom flags stubs —
 * nonblock_id, random_id, insecure_id, has_nonblock, has_random,
 * has_insecure, flags_ok, flags_pack, flags_errorish, batch_id_7020).
 * Unique gj_grnd_has_nonblock_u_7014 surface only; no multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch7014_marker[] = "libcgj-batch7014";

/* GRND_NONBLOCK bit (0x1). */
#define B7014_GRND_NONBLOCK ((uint32_t)0x1u)

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b7014_grnd_has_nonblock(uint32_t u32Flags)
{
	return ((u32Flags & B7014_GRND_NONBLOCK) != 0u) ? 1u : 0u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_grnd_has_nonblock_u_7014 - 1 if flags includes GRND_NONBLOCK.
 *
 * flags: getrandom(2) flags argument
 *
 * Returns 1 when bit 0x1 is set, else 0. Soft bit test only; does not
 * call getrandom. No parent wires.
 */
uint32_t
gj_grnd_has_nonblock_u_7014(uint32_t u32Flags)
{
	(void)NULL;
	return b7014_grnd_has_nonblock(u32Flags);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_grnd_has_nonblock_u_7014(uint32_t u32Flags)
    __attribute__((alias("gj_grnd_has_nonblock_u_7014")));
