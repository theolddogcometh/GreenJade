/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch8141: strstr empty-needle ok stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_strstr_empty_needle_ok_8141(void);
 *     - Return 1: empty needle is valid (matches at offset 0 like strstr).
 *   uint32_t __gj_strstr_empty_needle_ok_8141  (alias)
 *   __libcgj_batch8141_marker = "libcgj-batch8141"
 *
 * Exclusive continuum CREATE-ONLY (8141-8150: strstr needle stubs —
 * empty_needle_ok, not_found_id, match_zero_id, needle_is_empty,
 * hay_is_empty, needle_fits, offset_ok, params_ok, needle_errorish,
 * batch_id_8150). Unique gj_strstr_empty_needle_ok_8141 surface only;
 * no multi-def. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch8141_marker[] = "libcgj-batch8141";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b8141_empty_needle_ok(void)
{
	/* ISO C strstr: empty needle matches at the start of haystack. */
	return 1u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_strstr_empty_needle_ok_8141 - soft empty-needle policy catalog.
 *
 * Always returns 1 (empty needle is accepted). Does not call strstr(3).
 * No parent wires.
 */
uint32_t
gj_strstr_empty_needle_ok_8141(void)
{
	(void)NULL;
	return b8141_empty_needle_ok();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_strstr_empty_needle_ok_8141(void)
    __attribute__((alias("gj_strstr_empty_needle_ok_8141")));
