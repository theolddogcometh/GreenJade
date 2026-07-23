/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch8144: strstr needle-is-empty length stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_strstr_needle_is_empty_u_8144(uint32_t nlen);
 *     - Return 1 if needle length is 0, else 0.
 *   uint32_t __gj_strstr_needle_is_empty_u_8144  (alias)
 *   __libcgj_batch8144_marker = "libcgj-batch8144"
 *
 * Exclusive continuum CREATE-ONLY (8141-8150: strstr needle stubs —
 * empty_needle_ok, not_found_id, match_zero_id, needle_is_empty,
 * hay_is_empty, needle_fits, offset_ok, params_ok, needle_errorish,
 * batch_id_8150). Unique gj_strstr_needle_is_empty_u_8144 surface only;
 * no multi-def. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch8144_marker[] = "libcgj-batch8144";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b8144_needle_is_empty(uint32_t u32Nlen)
{
	return (u32Nlen == 0u) ? 1u : 0u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_strstr_needle_is_empty_u_8144 - 1 if needle length is empty.
 *
 * nlen: soft needle byte length (excluding trailing NUL if any)
 *
 * Returns 1 when nlen is 0, else 0. Soft catalog test; does not call
 * strstr(3)/strlen(3). No parent wires.
 */
uint32_t
gj_strstr_needle_is_empty_u_8144(uint32_t u32Nlen)
{
	(void)NULL;
	return b8144_needle_is_empty(u32Nlen);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_strstr_needle_is_empty_u_8144(uint32_t u32Nlen)
    __attribute__((alias("gj_strstr_needle_is_empty_u_8144")));
