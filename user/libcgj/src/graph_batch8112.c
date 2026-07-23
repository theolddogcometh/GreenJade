/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch8112: strcpy NUL terminator bytes id stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_strcpy_nul_bytes_id_8112(void);
 *     - Return soft NUL terminator size contribution for strcpy (1).
 *   uint32_t __gj_strcpy_nul_bytes_id_8112  (alias)
 *   __libcgj_batch8112_marker = "libcgj-batch8112"
 *
 * Exclusive continuum CREATE-ONLY (8111-8120: strcpy bounds stubs —
 * min_dst_id, nul_bytes_id, empty_need_id, dst_is_zero, need_fits,
 * dst_ok, need_ok, need_from_len, need_errorish, batch_id_8120).
 * Unique gj_strcpy_nul_bytes_id_8112 surface only; no multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch8112_marker[] = "libcgj-batch8112";

/* Soft strcpy NUL terminator byte count (always 1). */
#define B8112_NUL_BYTES  ((uint32_t)1u)

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b8112_nul_bytes_id(void)
{
	return B8112_NUL_BYTES;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_strcpy_nul_bytes_id_8112 - soft NUL size contribution for strcpy.
 *
 * Always returns 1. Catalog id only; does not call strcpy(3).
 * No parent wires.
 */
uint32_t
gj_strcpy_nul_bytes_id_8112(void)
{
	(void)NULL;
	return b8112_nul_bytes_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_strcpy_nul_bytes_id_8112(void)
    __attribute__((alias("gj_strcpy_nul_bytes_id_8112")));
