/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch7949: strtod INF special token id stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_strtod_special_inf_id_7949(void);
 *     - Return soft INF special-token catalog id (1).
 *   uint32_t __gj_strtod_special_inf_id_7949  (alias)
 *   __libcgj_batch7949_marker = "libcgj-batch7949"
 *
 * Exclusive continuum CREATE-ONLY (7941-7950: strtod parse stubs —
 * is_ws, sign_code, digit_val, hexdigit_val, is_x_marker, is_exp,
 * is_binexp, is_dp, special_inf_id, batch_id_7950).
 * Unique gj_strtod_special_inf_id_7949 surface only; no multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch7949_marker[] = "libcgj-batch7949";

/* Soft catalog id for "inf"/"infinity" special subject class. */
#define B7949_SPECIAL_INF  ((uint32_t)1u)

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b7949_special_inf_id(void)
{
	return B7949_SPECIAL_INF;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_strtod_special_inf_id_7949 - soft INF special-token catalog id.
 *
 * Always returns 1. Catalog tag for inf/infinity subject class in a
 * strtod parse pipeline; does not call strtod(3) or produce floating
 * values. No parent wires.
 */
uint32_t
gj_strtod_special_inf_id_7949(void)
{
	(void)NULL;
	return b7949_special_inf_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_strtod_special_inf_id_7949(void)
    __attribute__((alias("gj_strtod_special_inf_id_7949")));
