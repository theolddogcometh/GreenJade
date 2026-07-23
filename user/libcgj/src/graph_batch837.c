/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch837: uint64_t maximum of two values.
 *
 * Surface (unique symbols):
 *   uint64_t gj_max_u64(uint64_t a, uint64_t b);
 *     — Larger of a and b (equal → b).
 *   uint64_t __gj_max_u64  (alias)
 *   __libcgj_batch837_marker = "libcgj-batch837"
 *
 * Distinct from gj_min_u64 (batch836) and other max surfaces —
 * avoid multi-def.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stdint.h>

const char __libcgj_batch837_marker[] = "libcgj-batch837";

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_max_u64 — return the greater of two uint64_t values.
 */
uint64_t
gj_max_u64(uint64_t u64A, uint64_t u64B)
{
	if (u64A > u64B) {
		return u64A;
	}
	return u64B;
}

/* ---- underscored alias ------------------------------------------------- */

uint64_t __gj_max_u64(uint64_t u64A, uint64_t u64B)
    __attribute__((alias("gj_max_u64")));
