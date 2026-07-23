/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch7012: getrandom GRND_RANDOM flag id stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_grnd_random_id_7012(void);
 *     - Return soft GRND_RANDOM constant (0x2).
 *   uint32_t __gj_grnd_random_id_7012  (alias)
 *   __libcgj_batch7012_marker = "libcgj-batch7012"
 *
 * Exclusive continuum CREATE-ONLY (7011-7020: getrandom flags stubs —
 * nonblock_id, random_id, insecure_id, has_nonblock, has_random,
 * has_insecure, flags_ok, flags_pack, flags_errorish, batch_id_7020).
 * Unique gj_grnd_random_id_7012 surface only; no multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch7012_marker[] = "libcgj-batch7012";

/* GRND_RANDOM: draw from /dev/random pool (0x2). */
#define B7012_GRND_RANDOM ((uint32_t)0x2u)

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b7012_grnd_random_id(void)
{
	return B7012_GRND_RANDOM;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_grnd_random_id_7012 - soft GRND_RANDOM flag constant.
 *
 * Always returns 0x2 (Linux GRND_RANDOM). Catalog id only; does not
 * call getrandom. No parent wires.
 */
uint32_t
gj_grnd_random_id_7012(void)
{
	(void)NULL;
	return b7012_grnd_random_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_grnd_random_id_7012(void)
    __attribute__((alias("gj_grnd_random_id_7012")));
