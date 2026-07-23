/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch8074: strlen length is-zero presence stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_strlen_len_is_zero_u_8074(uint32_t len);
 *     - Return 1 if len is 0, else 0.
 *   uint32_t __gj_strlen_len_is_zero_u_8074  (alias)
 *   __libcgj_batch8074_marker = "libcgj-batch8074"
 *
 * Exclusive continuum CREATE-ONLY (8071-8080: strlen max stubs —
 * size_max_id, ssize_max_id, empty_len_id, len_is_zero, len_is_max,
 * len_below_max, len_ok, len_clamp, len_errorish, batch_id_8080).
 * Unique gj_strlen_len_is_zero_u_8074 surface only; no multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch8074_marker[] = "libcgj-batch8074";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b8074_strlen_len_is_zero(uint32_t u32Len)
{
	return (u32Len == 0u) ? 1u : 0u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_strlen_len_is_zero_u_8074 - 1 if length is zero.
 *
 * len: candidate string length
 *
 * Returns 1 when len is 0, else 0. Soft catalog test; does not call
 * strlen(3). No parent wires.
 */
uint32_t
gj_strlen_len_is_zero_u_8074(uint32_t u32Len)
{
	(void)NULL;
	return b8074_strlen_len_is_zero(u32Len);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_strlen_len_is_zero_u_8074(uint32_t u32Len)
    __attribute__((alias("gj_strlen_len_is_zero_u_8074")));
