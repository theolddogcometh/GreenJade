/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch7938: strtol base validity stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_strtol_base_ok_u_7938(uint32_t base);
 *     - Return 1 if base is a valid soft strtol base (0 or 2..36),
 *       else 0.
 *   uint32_t __gj_strtol_base_ok_u_7938  (alias)
 *   __libcgj_batch7938_marker = "libcgj-batch7938"
 *
 * Exclusive continuum CREATE-ONLY (7931-7940: strtol base stubs —
 * auto_id, bin_id, oct_id, dec_id, hex_id, min_id, max_id, base_ok,
 * base_errorish, continuum + batch_id_7940). Unique
 * gj_strtol_base_ok_u_7938 surface only; no multi-def. No parent
 * wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch7938_marker[] = "libcgj-batch7938";

/* Soft strtol bases: 0 (auto) or 2..36 (explicit). */
#define B7938_BASE_AUTO  ((uint32_t)0u)
#define B7938_BASE_MIN   ((uint32_t)2u)
#define B7938_BASE_MAX   ((uint32_t)36u)

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b7938_base_ok(uint32_t u32Base)
{
	if (u32Base == B7938_BASE_AUTO) {
		return 1u;
	}
	if (u32Base >= B7938_BASE_MIN && u32Base <= B7938_BASE_MAX) {
		return 1u;
	}
	return 0u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_strtol_base_ok_u_7938 - 1 if base is a valid soft strtol base.
 *
 * base: soft strtol base code
 *
 * Returns 1 when base is 0 (auto) or in 2..36; else 0. Soft catalog
 * check; does not call strtol. No parent wires.
 */
uint32_t
gj_strtol_base_ok_u_7938(uint32_t u32Base)
{
	(void)NULL;
	return b7938_base_ok(u32Base);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_strtol_base_ok_u_7938(uint32_t u32Base)
    __attribute__((alias("gj_strtol_base_ok_u_7938")));
