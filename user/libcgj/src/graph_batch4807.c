/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch4807: event-loop interest mask bitwise OR.
 *
 * Surface (unique symbols):
 *   uint32_t gj_ev_mask_or_u(uint32_t a, uint32_t b);
 *     - Return a | b (combine interest/ready bits).
 *   uint32_t __gj_ev_mask_or_u  (alias)
 *   __libcgj_batch4807_marker = "libcgj-batch4807"
 *
 * Exclusive continuum CREATE-ONLY (4801-4810). Unique
 * gj_ev_mask_or_u surface only; no multi-def. Distinct from
 * gj_ev_mask_has_u (batch4806). No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch4807_marker[] = "libcgj-batch4807";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b4807_mask_or(uint32_t u32A, uint32_t u32B)
{
	return u32A | u32B;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_ev_mask_or_u - combine two event-loop interest/ready masks.
 *
 * a: left-hand mask bits
 * b: right-hand mask bits
 *
 * Returns a | b. No parent wires.
 */
uint32_t
gj_ev_mask_or_u(uint32_t u32A, uint32_t u32B)
{
	(void)NULL;
	return b4807_mask_or(u32A, u32B);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_ev_mask_or_u(uint32_t u32A, uint32_t u32B)
    __attribute__((alias("gj_ev_mask_or_u")));
