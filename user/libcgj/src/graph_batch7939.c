/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch7939: strtol base errorish/unknown stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_strtol_base_errorish_u_7939(uint32_t base);
 *     - Return 1 if base is not a valid soft strtol base
 *       (not 0 and not in 2..36), else 0.
 *   uint32_t __gj_strtol_base_errorish_u_7939  (alias)
 *   __libcgj_batch7939_marker = "libcgj-batch7939"
 *
 * Exclusive continuum CREATE-ONLY (7931-7940: strtol base stubs —
 * auto_id, bin_id, oct_id, dec_id, hex_id, min_id, max_id, base_ok,
 * base_errorish, continuum + batch_id_7940). Unique
 * gj_strtol_base_errorish_u_7939 surface only; no multi-def. No parent
 * wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch7939_marker[] = "libcgj-batch7939";

/* Soft strtol bases: 0 (auto) or 2..36 (explicit). Base 1 is invalid. */
#define B7939_BASE_AUTO  ((uint32_t)0u)
#define B7939_BASE_MIN   ((uint32_t)2u)
#define B7939_BASE_MAX   ((uint32_t)36u)

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b7939_base_errorish(uint32_t u32Base)
{
	if (u32Base == B7939_BASE_AUTO) {
		return 0u;
	}
	if (u32Base >= B7939_BASE_MIN && u32Base <= B7939_BASE_MAX) {
		return 0u;
	}
	return 1u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_strtol_base_errorish_u_7939 - 1 if base is not a valid strtol base.
 *
 * base: soft strtol base code
 *
 * Returns 1 when base is not 0 and not in 2..36 (e.g. 1 or >36);
 * else 0. Soft catalog check; does not call strtol. No parent wires.
 */
uint32_t
gj_strtol_base_errorish_u_7939(uint32_t u32Base)
{
	(void)NULL;
	return b7939_base_errorish(u32Base);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_strtol_base_errorish_u_7939(uint32_t u32Base)
    __attribute__((alias("gj_strtol_base_errorish_u_7939")));
