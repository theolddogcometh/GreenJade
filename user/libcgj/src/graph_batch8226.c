/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch8226: string need-with-NUL (len+1) stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_str_need_nul_u_8226(uint32_t len);
 *     - Return len + 1 (payload + terminating NUL), saturating at
 *       UINT32_MAX on overflow.
 *   uint32_t __gj_str_need_nul_u_8226  (alias)
 *   __libcgj_batch8226_marker = "libcgj-batch8226"
 *
 * Exclusive continuum CREATE-ONLY (8221-8230: string length / bound
 * helpers — strlen_cap, strnlen, strlen_empty, strlen_nonzero,
 * str_bound_ok, str_need_nul, str_fits, str_overflow, str_underflow,
 * batch_id_8230). Unique gj_str_need_nul_u_8226 surface only; no
 * multi-def. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch8226_marker[] = "libcgj-batch8226";

/* Soft 32-bit all-ones saturation ceiling. */
#define B8226_U32_MAX  ((uint32_t)0xffffffffu)

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b8226_str_need_nul(uint32_t u32Len)
{
	/* need = len + 1 (NUL); saturate rather than wrap on overflow. */
	if (u32Len == B8226_U32_MAX) {
		return B8226_U32_MAX;
	}
	return u32Len + 1u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_str_need_nul_u_8226 - soft buffer need from payload length.
 *
 * len: payload length (octets before terminating NUL)
 *
 * Returns len + 1, or UINT32_MAX if len is already UINT32_MAX.
 * Pure integer catalog helper. No parent wires.
 */
uint32_t
gj_str_need_nul_u_8226(uint32_t u32Len)
{
	(void)NULL;
	return b8226_str_need_nul(u32Len);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_str_need_nul_u_8226(uint32_t u32Len)
    __attribute__((alias("gj_str_need_nul_u_8226")));
