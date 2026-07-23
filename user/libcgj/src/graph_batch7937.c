/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch7937: strtol explicit base maximum id stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_strtol_base_max_id_7937(void);
 *     - Return soft maximum explicit strtol base (36).
 *   uint32_t __gj_strtol_base_max_id_7937  (alias)
 *   __libcgj_batch7937_marker = "libcgj-batch7937"
 *
 * Exclusive continuum CREATE-ONLY (7931-7940: strtol base stubs —
 * auto_id, bin_id, oct_id, dec_id, hex_id, min_id, max_id, base_ok,
 * base_errorish, continuum + batch_id_7940). Unique
 * gj_strtol_base_max_id_7937 surface only; no multi-def. No parent
 * wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch7937_marker[] = "libcgj-batch7937";

/* Soft strtol: maximum valid explicit base (base 36, digits 0-9A-Z). */
#define B7937_BASE_MAX  ((uint32_t)36u)

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b7937_max_id(void)
{
	return B7937_BASE_MAX;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_strtol_base_max_id_7937 - soft maximum explicit strtol base.
 *
 * Always returns 36. Catalog bound only; does not call strtol. No parent
 * wires.
 */
uint32_t
gj_strtol_base_max_id_7937(void)
{
	(void)NULL;
	return b7937_max_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_strtol_base_max_id_7937(void)
    __attribute__((alias("gj_strtol_base_max_id_7937")));
