/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch8142: strstr not-found sentinel id stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_strstr_not_found_id_8142(void);
 *     - Return soft not-found offset sentinel (0xffffffff).
 *   uint32_t __gj_strstr_not_found_id_8142  (alias)
 *   __libcgj_batch8142_marker = "libcgj-batch8142"
 *
 * Exclusive continuum CREATE-ONLY (8141-8150: strstr needle stubs —
 * empty_needle_ok, not_found_id, match_zero_id, needle_is_empty,
 * hay_is_empty, needle_fits, offset_ok, params_ok, needle_errorish,
 * batch_id_8150). Unique gj_strstr_not_found_id_8142 surface only;
 * no multi-def. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch8142_marker[] = "libcgj-batch8142";

/* Soft offset sentinel when needle is absent from haystack. */
#define B8142_NOT_FOUND  ((uint32_t)0xFFFFFFFFu)

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b8142_not_found_id(void)
{
	return B8142_NOT_FOUND;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_strstr_not_found_id_8142 - soft not-found offset catalog id.
 *
 * Always returns 0xffffffff. Index-domain sentinel (not a pointer);
 * does not call strstr(3). No parent wires.
 */
uint32_t
gj_strstr_not_found_id_8142(void)
{
	(void)NULL;
	return b8142_not_found_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_strstr_not_found_id_8142(void)
    __attribute__((alias("gj_strstr_not_found_id_8142")));
