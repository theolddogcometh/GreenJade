/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch8079: strlen length errorish stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_strlen_len_errorish_u_8079(uint32_t len);
 *     - Return 1 if len is above soft SSIZE_MAX, else 0.
 *   uint32_t __gj_strlen_len_errorish_u_8079  (alias)
 *   __libcgj_batch8079_marker = "libcgj-batch8079"
 *
 * Exclusive continuum CREATE-ONLY (8071-8080: strlen max stubs —
 * size_max_id, ssize_max_id, empty_len_id, len_is_zero, len_is_max,
 * len_below_max, len_ok, len_clamp, len_errorish, batch_id_8080).
 * Unique gj_strlen_len_errorish_u_8079 surface only; no multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch8079_marker[] = "libcgj-batch8079";

/* Soft SSIZE_MAX catalog ceiling (above = errorish). */
#define B8079_SSIZE_MAX ((uint32_t)0x7fffffffu)

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b8079_strlen_len_errorish(uint32_t u32Len)
{
	/* Soft inverse of len_ok: lengths above SSIZE_MAX are errorish. */
	return (u32Len > B8079_SSIZE_MAX) ? 1u : 0u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_strlen_len_errorish_u_8079 - 1 if length is catalog-errorish.
 *
 * len: candidate string length
 *
 * Returns 1 when len > 0x7fffffff, else 0. Soft catalog test; does not
 * call strlen(3). No parent wires.
 */
uint32_t
gj_strlen_len_errorish_u_8079(uint32_t u32Len)
{
	(void)NULL;
	return b8079_strlen_len_errorish(u32Len);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_strlen_len_errorish_u_8079(uint32_t u32Len)
    __attribute__((alias("gj_strlen_len_errorish_u_8079")));
