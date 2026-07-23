/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch8225: string length-within-cap bound stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_str_bound_ok_u_8225(uint32_t len, uint32_t cap);
 *     - Return 1 if len <= cap (payload length is within the bound).
 *   uint32_t __gj_str_bound_ok_u_8225  (alias)
 *   __libcgj_batch8225_marker = "libcgj-batch8225"
 *
 * Exclusive continuum CREATE-ONLY (8221-8230: string length / bound
 * helpers — strlen_cap, strnlen, strlen_empty, strlen_nonzero,
 * str_bound_ok, str_need_nul, str_fits, str_overflow, str_underflow,
 * batch_id_8230). Unique gj_str_bound_ok_u_8225 surface only; no
 * multi-def. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch8225_marker[] = "libcgj-batch8225";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b8225_str_bound_ok(uint32_t u32Len, uint32_t u32Cap)
{
	return (u32Len <= u32Cap) ? 1u : 0u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_str_bound_ok_u_8225 - 1 if length is within the catalog cap.
 *
 * len: payload length (octets before terminating NUL)
 * cap: maximum accepted payload length
 *
 * Returns 1 when len <= cap, else 0. Soft catalog test; does not scan
 * memory. No parent wires.
 */
uint32_t
gj_str_bound_ok_u_8225(uint32_t u32Len, uint32_t u32Cap)
{
	(void)NULL;
	return b8225_str_bound_ok(u32Len, u32Cap);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_str_bound_ok_u_8225(uint32_t u32Len, uint32_t u32Cap)
    __attribute__((alias("gj_str_bound_ok_u_8225")));
