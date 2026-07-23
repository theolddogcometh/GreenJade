/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch7011: getrandom GRND_NONBLOCK flag id stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_grnd_nonblock_id_7011(void);
 *     - Return soft GRND_NONBLOCK constant (0x1).
 *   uint32_t __gj_grnd_nonblock_id_7011  (alias)
 *   __libcgj_batch7011_marker = "libcgj-batch7011"
 *
 * Exclusive continuum CREATE-ONLY (7011-7020: getrandom flags stubs —
 * nonblock_id, random_id, insecure_id, has_nonblock, has_random,
 * has_insecure, flags_ok, flags_pack, flags_errorish, batch_id_7020).
 * Unique gj_grnd_nonblock_id_7011 surface only; no multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch7011_marker[] = "libcgj-batch7011";

/* GRND_NONBLOCK: return immediately if entropy is unavailable (0x1). */
#define B7011_GRND_NONBLOCK ((uint32_t)0x1u)

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b7011_grnd_nonblock_id(void)
{
	return B7011_GRND_NONBLOCK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_grnd_nonblock_id_7011 - soft GRND_NONBLOCK flag constant.
 *
 * Always returns 0x1 (Linux GRND_NONBLOCK). Catalog id only; does not
 * call getrandom. No parent wires.
 */
uint32_t
gj_grnd_nonblock_id_7011(void)
{
	(void)NULL;
	return b7011_grnd_nonblock_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_grnd_nonblock_id_7011(void)
    __attribute__((alias("gj_grnd_nonblock_id_7011")));
