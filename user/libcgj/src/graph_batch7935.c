/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch7935: strtol base hexadecimal id stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_strtol_base_hex_id_7935(void);
 *     - Return soft strtol hexadecimal base constant (16).
 *   uint32_t __gj_strtol_base_hex_id_7935  (alias)
 *   __libcgj_batch7935_marker = "libcgj-batch7935"
 *
 * Exclusive continuum CREATE-ONLY (7931-7940: strtol base stubs —
 * auto_id, bin_id, oct_id, dec_id, hex_id, min_id, max_id, base_ok,
 * base_errorish, continuum + batch_id_7940). Unique
 * gj_strtol_base_hex_id_7935 surface only; no multi-def. No parent
 * wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch7935_marker[] = "libcgj-batch7935";

/* Soft strtol base: hexadecimal (base 16). */
#define B7935_BASE_HEX  ((uint32_t)16u)

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b7935_hex_id(void)
{
	return B7935_BASE_HEX;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_strtol_base_hex_id_7935 - soft strtol hexadecimal base constant.
 *
 * Always returns 16. Catalog id only; does not call strtol. No parent
 * wires.
 */
uint32_t
gj_strtol_base_hex_id_7935(void)
{
	(void)NULL;
	return b7935_hex_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_strtol_base_hex_id_7935(void)
    __attribute__((alias("gj_strtol_base_hex_id_7935")));
