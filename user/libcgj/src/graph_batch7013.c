/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch7013: getrandom GRND_INSECURE flag id stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_grnd_insecure_id_7013(void);
 *     - Return soft GRND_INSECURE constant (0x4).
 *   uint32_t __gj_grnd_insecure_id_7013  (alias)
 *   __libcgj_batch7013_marker = "libcgj-batch7013"
 *
 * Exclusive continuum CREATE-ONLY (7011-7020: getrandom flags stubs —
 * nonblock_id, random_id, insecure_id, has_nonblock, has_random,
 * has_insecure, flags_ok, flags_pack, flags_errorish, batch_id_7020).
 * Unique gj_grnd_insecure_id_7013 surface only; no multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch7013_marker[] = "libcgj-batch7013";

/* GRND_INSECURE: allow non-blocking insecure bytes early-boot (0x4). */
#define B7013_GRND_INSECURE ((uint32_t)0x4u)

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b7013_grnd_insecure_id(void)
{
	return B7013_GRND_INSECURE;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_grnd_insecure_id_7013 - soft GRND_INSECURE flag constant.
 *
 * Always returns 0x4 (Linux GRND_INSECURE). Catalog id only; does not
 * call getrandom. No parent wires.
 */
uint32_t
gj_grnd_insecure_id_7013(void)
{
	(void)NULL;
	return b7013_grnd_insecure_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_grnd_insecure_id_7013(void)
    __attribute__((alias("gj_grnd_insecure_id_7013")));
