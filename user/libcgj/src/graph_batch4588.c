/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch4588: symmetric difference of two flag words.
 *
 * Surface (unique symbols):
 *   uint32_t gj_flags_diff_u(uint32_t a, uint32_t b);
 *     - Return a ^ b (bits that differ between a and b). Pure value
 *       XOR of two flag words.
 *   uint32_t __gj_flags_diff_u  (alias)
 *   __libcgj_batch4588_marker = "libcgj-batch4588"
 *
 * Exclusive continuum CREATE-ONLY (4581-4590). Distinct from
 * gj_flags_toggle_u (batch4583, XOR with a bit mask as second arg
 * semantics named toggle) — unique gj_flags_diff_u surface only;
 * no multi-def. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch4588_marker[] = "libcgj-batch4588";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b4588_diff(uint32_t u32A, uint32_t u32B)
{
	return u32A ^ u32B;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_flags_diff_u - bits that differ between two flag words (a XOR b).
 *
 * a: first flag word
 * b: second flag word
 *
 * Returns a ^ b. Self-contained; no parent wires.
 */
uint32_t
gj_flags_diff_u(uint32_t u32A, uint32_t u32B)
{
	(void)NULL;
	return b4588_diff(u32A, u32B);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_flags_diff_u(uint32_t u32A, uint32_t u32B)
    __attribute__((alias("gj_flags_diff_u")));
