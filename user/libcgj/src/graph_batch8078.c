/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch8078: strlen length clamp-to-max stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_strlen_len_clamp_u_8078(uint32_t len);
 *     - Clamp len into [0, soft SSIZE_MAX]; return min(len, max).
 *   uint32_t __gj_strlen_len_clamp_u_8078  (alias)
 *   __libcgj_batch8078_marker = "libcgj-batch8078"
 *
 * Exclusive continuum CREATE-ONLY (8071-8080: strlen max stubs —
 * size_max_id, ssize_max_id, empty_len_id, len_is_zero, len_is_max,
 * len_below_max, len_ok, len_clamp, len_errorish, batch_id_8080).
 * Unique gj_strlen_len_clamp_u_8078 surface only; no multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch8078_marker[] = "libcgj-batch8078";

/* Soft SSIZE_MAX catalog ceiling for clamp. */
#define B8078_SSIZE_MAX ((uint32_t)0x7fffffffu)

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b8078_strlen_len_clamp(uint32_t u32Len)
{
	if (u32Len > B8078_SSIZE_MAX)
		return B8078_SSIZE_MAX;
	return u32Len;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_strlen_len_clamp_u_8078 - clamp length to soft SSIZE_MAX.
 *
 * len: candidate string length
 *
 * Returns min(len, 0x7fffffff). Pure integer catalog clamp; does not
 * call strlen(3). No parent wires.
 */
uint32_t
gj_strlen_len_clamp_u_8078(uint32_t u32Len)
{
	(void)NULL;
	return b8078_strlen_len_clamp(u32Len);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_strlen_len_clamp_u_8078(uint32_t u32Len)
    __attribute__((alias("gj_strlen_len_clamp_u_8078")));
