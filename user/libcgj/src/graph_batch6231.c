/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch6231: HDR PQ (ST.2084) code-domain clamp stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_hdr_pq_code_clamp_6231(uint32_t code_q16);
 *     - Clamp a PQ nonlinear code value expressed in Q16 fixed-point
 *       (0..65536 maps to 0..1) into the legal EOTF domain [0, 65536].
 *       Values above the unit ceiling collapse to 65536.
 *   uint32_t __gj_hdr_pq_code_clamp_6231  (alias)
 *   __libcgj_batch6231_marker = "libcgj-batch6231"
 *
 * Exclusive continuum CREATE-ONLY (6231-6240: HDR PQ/HLG transfer stubs).
 * Unique gj_hdr_pq_code_clamp_6231 surface only; no multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch6231_marker[] = "libcgj-batch6231";

/* Q16 unit: 65536 represents normalized code 1.0 (PQ E' domain). */
#define B6231_PQ_CODE_MAX_Q16  65536u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b6231_clamp(uint32_t u32CodeQ16)
{
	if (u32CodeQ16 > B6231_PQ_CODE_MAX_Q16) {
		return B6231_PQ_CODE_MAX_Q16;
	}
	return u32CodeQ16;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_hdr_pq_code_clamp_6231 - clamp PQ nonlinear code into [0, 1] Q16.
 *
 * code_q16: PQ E' sample in Q16 (0 = black, 65536 = peak code 1.0).
 * Returns clamped Q16 code. Soft pure-integer HDR transfer prep; not a
 * full ST.2084 EOTF. Does not call libc. No parent wires.
 */
uint32_t
gj_hdr_pq_code_clamp_6231(uint32_t u32CodeQ16)
{
	(void)NULL;
	return b6231_clamp(u32CodeQ16);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_hdr_pq_code_clamp_6231(uint32_t u32CodeQ16)
    __attribute__((alias("gj_hdr_pq_code_clamp_6231")));
