/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch8077: strlen length ok stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_strlen_len_ok_u_8077(uint32_t len);
 *     - Return 1 if len is within soft SSIZE_MAX inclusive domain.
 *   uint32_t __gj_strlen_len_ok_u_8077  (alias)
 *   __libcgj_batch8077_marker = "libcgj-batch8077"
 *
 * Exclusive continuum CREATE-ONLY (8071-8080: strlen max stubs —
 * size_max_id, ssize_max_id, empty_len_id, len_is_zero, len_is_max,
 * len_below_max, len_ok, len_clamp, len_errorish, batch_id_8080).
 * Unique gj_strlen_len_ok_u_8077 surface only; no multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch8077_marker[] = "libcgj-batch8077";

/* Soft SSIZE_MAX catalog ceiling (inclusive ok domain). */
#define B8077_SSIZE_MAX ((uint32_t)0x7fffffffu)

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b8077_strlen_len_ok(uint32_t u32Len)
{
	/* Catalog: lengths in [0, SSIZE_MAX] are ok. */
	return (u32Len <= B8077_SSIZE_MAX) ? 1u : 0u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_strlen_len_ok_u_8077 - 1 if length is catalog-acceptable.
 *
 * len: candidate string length
 *
 * Returns 1 when len <= 0x7fffffff, else 0. Soft catalog test; does not
 * call strlen(3). No parent wires.
 */
uint32_t
gj_strlen_len_ok_u_8077(uint32_t u32Len)
{
	(void)NULL;
	return b8077_strlen_len_ok(u32Len);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_strlen_len_ok_u_8077(uint32_t u32Len)
    __attribute__((alias("gj_strlen_len_ok_u_8077")));
