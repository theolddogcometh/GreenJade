/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch8149: strstr needle length errorish stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_strstr_needle_errorish_u_8149(uint32_t nlen, uint32_t hlen);
 *     - Return 1 if non-empty needle cannot fit (nlen > hlen), else 0.
 *   uint32_t __gj_strstr_needle_errorish_u_8149  (alias)
 *   __libcgj_batch8149_marker = "libcgj-batch8149"
 *
 * Exclusive continuum CREATE-ONLY (8141-8150: strstr needle stubs —
 * empty_needle_ok, not_found_id, match_zero_id, needle_is_empty,
 * hay_is_empty, needle_fits, offset_ok, params_ok, needle_errorish,
 * batch_id_8150). Unique gj_strstr_needle_errorish_u_8149 surface only;
 * no multi-def. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch8149_marker[] = "libcgj-batch8149";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b8149_needle_errorish(uint32_t u32Nlen, uint32_t u32Hlen)
{
	/* Empty needle is never errorish; overlong non-empty is. */
	if (u32Nlen == 0u) {
		return 0u;
	}
	return (u32Nlen > u32Hlen) ? 1u : 0u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_strstr_needle_errorish_u_8149 - 1 if needle length is overlong.
 *
 * nlen: soft needle length
 * hlen: soft haystack length
 *
 * Returns 1 when nlen > 0 and nlen > hlen (cannot match), else 0.
 * Soft catalog test; does not call strstr(3). No parent wires.
 */
uint32_t
gj_strstr_needle_errorish_u_8149(uint32_t u32Nlen, uint32_t u32Hlen)
{
	(void)NULL;
	return b8149_needle_errorish(u32Nlen, u32Hlen);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_strstr_needle_errorish_u_8149(uint32_t u32Nlen, uint32_t u32Hlen)
    __attribute__((alias("gj_strstr_needle_errorish_u_8149")));
