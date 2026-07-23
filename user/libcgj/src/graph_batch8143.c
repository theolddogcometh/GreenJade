/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch8143: strstr match-at-zero offset id stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_strstr_match_zero_id_8143(void);
 *     - Return soft match-at-start offset catalog id (0).
 *   uint32_t __gj_strstr_match_zero_id_8143  (alias)
 *   __libcgj_batch8143_marker = "libcgj-batch8143"
 *
 * Exclusive continuum CREATE-ONLY (8141-8150: strstr needle stubs —
 * empty_needle_ok, not_found_id, match_zero_id, needle_is_empty,
 * hay_is_empty, needle_fits, offset_ok, params_ok, needle_errorish,
 * batch_id_8150). Unique gj_strstr_match_zero_id_8143 surface only;
 * no multi-def. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch8143_marker[] = "libcgj-batch8143";

/* Soft offset when needle matches at the start of haystack. */
#define B8143_MATCH_ZERO  ((uint32_t)0u)

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b8143_match_zero_id(void)
{
	return B8143_MATCH_ZERO;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_strstr_match_zero_id_8143 - soft match-at-offset-0 catalog id.
 *
 * Always returns 0 (needle found at haystack start, or empty needle).
 * Does not call strstr(3). No parent wires.
 */
uint32_t
gj_strstr_match_zero_id_8143(void)
{
	(void)NULL;
	return b8143_match_zero_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_strstr_match_zero_id_8143(void)
    __attribute__((alias("gj_strstr_match_zero_id_8143")));
