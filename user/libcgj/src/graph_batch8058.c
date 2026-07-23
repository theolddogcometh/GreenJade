/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch8058: memset pattern ok stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_memset_pat_ok_u_8058(uint32_t pat);
 *     - Return 1 if pat fits a single fill byte (pat <= 0xff), else 0.
 *   uint32_t __gj_memset_pat_ok_u_8058  (alias)
 *   __libcgj_batch8058_marker = "libcgj-batch8058"
 *
 * Exclusive continuum CREATE-ONLY (8051-8060: memset pattern stubs —
 * memset_zero_id, memset_ff_id, pat_byte, pat_is_zero, pat_is_ff,
 * len_ok, pat_word, pat_ok, len_errorish, batch_id_8060).
 * Unique gj_memset_pat_ok_u_8058 surface only; no multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch8058_marker[] = "libcgj-batch8058";

#define B8058_BYTE_MAX ((uint32_t)0xffu)

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b8058_pat_ok(uint32_t u32Pat)
{
	return (u32Pat <= B8058_BYTE_MAX) ? 1u : 0u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_memset_pat_ok_u_8058 - 1 if pattern is a pure fill-byte value.
 *
 * pat: candidate fill pattern word
 *
 * Returns 1 when pat <= 0xff (already a single-byte domain), else 0.
 * Catalog gate only; does not call memset(3). No parent wires.
 */
uint32_t
gj_memset_pat_ok_u_8058(uint32_t u32Pat)
{
	(void)NULL;
	return b8058_pat_ok(u32Pat);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_memset_pat_ok_u_8058(uint32_t u32Pat)
    __attribute__((alias("gj_memset_pat_ok_u_8058")));
