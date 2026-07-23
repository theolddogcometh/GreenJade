/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch7931: strtol base auto id stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_strtol_base_auto_id_7931(void);
 *     - Return soft strtol auto-base constant (0).
 *   uint32_t __gj_strtol_base_auto_id_7931  (alias)
 *   __libcgj_batch7931_marker = "libcgj-batch7931"
 *
 * Exclusive continuum CREATE-ONLY (7931-7940: strtol base stubs —
 * auto_id, bin_id, oct_id, dec_id, hex_id, min_id, max_id, base_ok,
 * base_errorish, continuum + batch_id_7940). Unique
 * gj_strtol_base_auto_id_7931 surface only; no multi-def. No parent
 * wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch7931_marker[] = "libcgj-batch7931";

/* Soft strtol base: 0 means auto-detect (0x / 0 prefix). */
#define B7931_BASE_AUTO  ((uint32_t)0u)

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b7931_auto_id(void)
{
	return B7931_BASE_AUTO;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_strtol_base_auto_id_7931 - soft strtol auto-base constant.
 *
 * Always returns 0. Catalog id only; does not call strtol. No parent
 * wires.
 */
uint32_t
gj_strtol_base_auto_id_7931(void)
{
	(void)NULL;
	return b7931_auto_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_strtol_base_auto_id_7931(void)
    __attribute__((alias("gj_strtol_base_auto_id_7931")));
