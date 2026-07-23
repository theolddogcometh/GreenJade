/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch8146: strstr needle-fits-in-hay length stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_strstr_needle_fits_u_8146(uint32_t nlen, uint32_t hlen);
 *     - Return 1 if nlen <= hlen (needle can still fit), else 0.
 *   uint32_t __gj_strstr_needle_fits_u_8146  (alias)
 *   __libcgj_batch8146_marker = "libcgj-batch8146"
 *
 * Exclusive continuum CREATE-ONLY (8141-8150: strstr needle stubs —
 * empty_needle_ok, not_found_id, match_zero_id, needle_is_empty,
 * hay_is_empty, needle_fits, offset_ok, params_ok, needle_errorish,
 * batch_id_8150). Unique gj_strstr_needle_fits_u_8146 surface only;
 * no multi-def. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch8146_marker[] = "libcgj-batch8146";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b8146_needle_fits(uint32_t u32Nlen, uint32_t u32Hlen)
{
	/* Empty needle always fits (matches at 0 even when hay is empty). */
	return (u32Nlen <= u32Hlen) ? 1u : 0u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_strstr_needle_fits_u_8146 - 1 if needle length can fit in haystack.
 *
 * nlen: soft needle length
 * hlen: soft haystack length
 *
 * Returns 1 when nlen <= hlen, else 0. Length-domain precheck only;
 * does not call strstr(3). No parent wires.
 */
uint32_t
gj_strstr_needle_fits_u_8146(uint32_t u32Nlen, uint32_t u32Hlen)
{
	(void)NULL;
	return b8146_needle_fits(u32Nlen, u32Hlen);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_strstr_needle_fits_u_8146(uint32_t u32Nlen, uint32_t u32Hlen)
    __attribute__((alias("gj_strstr_needle_fits_u_8146")));
