/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch8123: strncpy n-zero ok stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_strncpy_n_zero_ok_8123(void);
 *     - Return 1 (n == 0 is a valid no-op for strncpy).
 *   uint32_t __gj_strncpy_n_zero_ok_8123  (alias)
 *   __libcgj_batch8123_marker = "libcgj-batch8123"
 *
 * Exclusive continuum CREATE-ONLY (8121-8130: strncpy pad stubs —
 * pad_byte_id, pads_with_nul_id, n_zero_ok, needs_pad, pad_count,
 * copy_len, trunc_no_nul, full_pad, pad_ok, batch_id_8130).
 * Unique gj_strncpy_n_zero_ok_8123 surface only; no multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch8123_marker[] = "libcgj-batch8123";

/* Soft catalog: n == 0 copies nothing and pads nothing. */
#define B8123_STRNCPY_N_ZERO_OK ((uint32_t)1u)

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b8123_strncpy_n_zero_ok(void)
{
	return B8123_STRNCPY_N_ZERO_OK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_strncpy_n_zero_ok_8123 - soft n-zero is-ok catalog flag.
 *
 * Always returns 1. Catalog id only; does not call strncpy(3).
 * No parent wires.
 */
uint32_t
gj_strncpy_n_zero_ok_8123(void)
{
	(void)NULL;
	return b8123_strncpy_n_zero_ok();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_strncpy_n_zero_ok_8123(void)
    __attribute__((alias("gj_strncpy_n_zero_ok_8123")));
