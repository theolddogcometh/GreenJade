/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch8118: strcpy need-from-length stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_strcpy_need_from_len_u_8118(uint32_t len);
 *     - Return len + 1, saturating at UINT32_MAX on overflow.
 *   uint32_t __gj_strcpy_need_from_len_u_8118  (alias)
 *   __libcgj_batch8118_marker = "libcgj-batch8118"
 *
 * Exclusive continuum CREATE-ONLY (8111-8120: strcpy bounds stubs —
 * min_dst_id, nul_bytes_id, empty_need_id, dst_is_zero, need_fits,
 * dst_ok, need_ok, need_from_len, need_errorish, batch_id_8120).
 * Unique gj_strcpy_need_from_len_u_8118 surface only; no multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch8118_marker[] = "libcgj-batch8118";

/* Soft 32-bit all-ones saturation ceiling. */
#define B8118_U32_MAX  ((uint32_t)0xffffffffu)

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b8118_need_from_len(uint32_t u32Len)
{
	/* need = len + 1 (NUL); saturate rather than wrap on overflow. */
	if (u32Len == B8118_U32_MAX)
		return B8118_U32_MAX;
	return u32Len + 1u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_strcpy_need_from_len_u_8118 - soft strcpy byte-need from length.
 *
 * len: source payload length (octets before the terminating NUL)
 *
 * Returns len + 1, or UINT32_MAX if len is already UINT32_MAX.
 * Pure integer catalog helper; does not call strcpy(3). No parent wires.
 */
uint32_t
gj_strcpy_need_from_len_u_8118(uint32_t u32Len)
{
	(void)NULL;
	return b8118_need_from_len(u32Len);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_strcpy_need_from_len_u_8118(uint32_t u32Len)
    __attribute__((alias("gj_strcpy_need_from_len_u_8118")));
