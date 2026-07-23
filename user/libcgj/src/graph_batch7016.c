/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch7016: getrandom GRND_INSECURE presence stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_grnd_has_insecure_u_7016(uint32_t flags);
 *     - Return 1 if flags has GRND_INSECURE (0x4) set, else 0.
 *   uint32_t __gj_grnd_has_insecure_u_7016  (alias)
 *   __libcgj_batch7016_marker = "libcgj-batch7016"
 *
 * Exclusive continuum CREATE-ONLY (7011-7020: getrandom flags stubs —
 * nonblock_id, random_id, insecure_id, has_nonblock, has_random,
 * has_insecure, flags_ok, flags_pack, flags_errorish, batch_id_7020).
 * Unique gj_grnd_has_insecure_u_7016 surface only; no multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch7016_marker[] = "libcgj-batch7016";

/* GRND_INSECURE bit (0x4). */
#define B7016_GRND_INSECURE ((uint32_t)0x4u)

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b7016_grnd_has_insecure(uint32_t u32Flags)
{
	return ((u32Flags & B7016_GRND_INSECURE) != 0u) ? 1u : 0u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_grnd_has_insecure_u_7016 - 1 if flags includes GRND_INSECURE.
 *
 * flags: getrandom(2) flags argument
 *
 * Returns 1 when bit 0x4 is set, else 0. Soft bit test only; does not
 * call getrandom. No parent wires.
 */
uint32_t
gj_grnd_has_insecure_u_7016(uint32_t u32Flags)
{
	(void)NULL;
	return b7016_grnd_has_insecure(u32Flags);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_grnd_has_insecure_u_7016(uint32_t u32Flags)
    __attribute__((alias("gj_grnd_has_insecure_u_7016")));
