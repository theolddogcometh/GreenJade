/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch8147: strstr match offset ok stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_strstr_offset_ok_u_8147(uint32_t off, uint32_t hlen);
 *     - Return 1 if off is a valid start index in hay of length hlen
 *       (off < hlen, or off == 0 when hlen == 0 for empty match).
 *   uint32_t __gj_strstr_offset_ok_u_8147  (alias)
 *   __libcgj_batch8147_marker = "libcgj-batch8147"
 *
 * Exclusive continuum CREATE-ONLY (8141-8150: strstr needle stubs —
 * empty_needle_ok, not_found_id, match_zero_id, needle_is_empty,
 * hay_is_empty, needle_fits, offset_ok, params_ok, needle_errorish,
 * batch_id_8150). Unique gj_strstr_offset_ok_u_8147 surface only;
 * no multi-def. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch8147_marker[] = "libcgj-batch8147";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b8147_offset_ok(uint32_t u32Off, uint32_t u32Hlen)
{
	/* Empty hay: only offset 0 is the empty-needle match position. */
	if (u32Hlen == 0u) {
		return (u32Off == 0u) ? 1u : 0u;
	}
	return (u32Off < u32Hlen) ? 1u : 0u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_strstr_offset_ok_u_8147 - 1 if match start offset is in range.
 *
 * off:  candidate start index into haystack
 * hlen: soft haystack length
 *
 * Returns 1 when off is a legal start index for a needle scan, else 0.
 * Soft catalog test; does not call strstr(3). No parent wires.
 */
uint32_t
gj_strstr_offset_ok_u_8147(uint32_t u32Off, uint32_t u32Hlen)
{
	(void)NULL;
	return b8147_offset_ok(u32Off, u32Hlen);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_strstr_offset_ok_u_8147(uint32_t u32Off, uint32_t u32Hlen)
    __attribute__((alias("gj_strstr_offset_ok_u_8147")));
