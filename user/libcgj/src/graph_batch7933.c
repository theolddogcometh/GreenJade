/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch7933: strtol base octal id stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_strtol_base_oct_id_7933(void);
 *     - Return soft strtol octal base constant (8).
 *   uint32_t __gj_strtol_base_oct_id_7933  (alias)
 *   __libcgj_batch7933_marker = "libcgj-batch7933"
 *
 * Exclusive continuum CREATE-ONLY (7931-7940: strtol base stubs —
 * auto_id, bin_id, oct_id, dec_id, hex_id, min_id, max_id, base_ok,
 * base_errorish, continuum + batch_id_7940). Unique
 * gj_strtol_base_oct_id_7933 surface only; no multi-def. No parent
 * wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch7933_marker[] = "libcgj-batch7933";

/* Soft strtol base: octal (base 8). */
#define B7933_BASE_OCT  ((uint32_t)8u)

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b7933_oct_id(void)
{
	return B7933_BASE_OCT;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_strtol_base_oct_id_7933 - soft strtol octal base constant.
 *
 * Always returns 8. Catalog id only; does not call strtol. No parent
 * wires.
 */
uint32_t
gj_strtol_base_oct_id_7933(void)
{
	(void)NULL;
	return b7933_oct_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_strtol_base_oct_id_7933(void)
    __attribute__((alias("gj_strtol_base_oct_id_7933")));
