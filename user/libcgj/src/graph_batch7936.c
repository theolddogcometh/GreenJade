/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch7936: strtol explicit base minimum id stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_strtol_base_min_id_7936(void);
 *     - Return soft minimum explicit strtol base (2).
 *   uint32_t __gj_strtol_base_min_id_7936  (alias)
 *   __libcgj_batch7936_marker = "libcgj-batch7936"
 *
 * Exclusive continuum CREATE-ONLY (7931-7940: strtol base stubs —
 * auto_id, bin_id, oct_id, dec_id, hex_id, min_id, max_id, base_ok,
 * base_errorish, continuum + batch_id_7940). Unique
 * gj_strtol_base_min_id_7936 surface only; no multi-def. No parent
 * wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch7936_marker[] = "libcgj-batch7936";

/* Soft strtol: minimum valid explicit base (base 2). */
#define B7936_BASE_MIN  ((uint32_t)2u)

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b7936_min_id(void)
{
	return B7936_BASE_MIN;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_strtol_base_min_id_7936 - soft minimum explicit strtol base.
 *
 * Always returns 2. Catalog bound only; does not call strtol. No parent
 * wires.
 */
uint32_t
gj_strtol_base_min_id_7936(void)
{
	(void)NULL;
	return b7936_min_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_strtol_base_min_id_7936(void)
    __attribute__((alias("gj_strtol_base_min_id_7936")));
