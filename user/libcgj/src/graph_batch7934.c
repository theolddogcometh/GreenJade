/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch7934: strtol base decimal id stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_strtol_base_dec_id_7934(void);
 *     - Return soft strtol decimal base constant (10).
 *   uint32_t __gj_strtol_base_dec_id_7934  (alias)
 *   __libcgj_batch7934_marker = "libcgj-batch7934"
 *
 * Exclusive continuum CREATE-ONLY (7931-7940: strtol base stubs —
 * auto_id, bin_id, oct_id, dec_id, hex_id, min_id, max_id, base_ok,
 * base_errorish, continuum + batch_id_7940). Unique
 * gj_strtol_base_dec_id_7934 surface only; no multi-def. No parent
 * wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch7934_marker[] = "libcgj-batch7934";

/* Soft strtol base: decimal (base 10). */
#define B7934_BASE_DEC  ((uint32_t)10u)

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b7934_dec_id(void)
{
	return B7934_BASE_DEC;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_strtol_base_dec_id_7934 - soft strtol decimal base constant.
 *
 * Always returns 10. Catalog id only; does not call strtol. No parent
 * wires.
 */
uint32_t
gj_strtol_base_dec_id_7934(void)
{
	(void)NULL;
	return b7934_dec_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_strtol_base_dec_id_7934(void)
    __attribute__((alias("gj_strtol_base_dec_id_7934")));
