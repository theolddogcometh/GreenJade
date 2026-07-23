/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch833: int32_t maximum of two values.
 *
 * Surface (unique symbols):
 *   int32_t gj_max_i32(int32_t a, int32_t b);
 *     — Larger of a and b (equal → b).
 *   int32_t __gj_max_i32  (alias)
 *   __libcgj_batch833_marker = "libcgj-batch833"
 *
 * Distinct from gj_max_i64 (batch376) / gj_min_i32 (batch832) —
 * avoid multi-def.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stdint.h>

const char __libcgj_batch833_marker[] = "libcgj-batch833";

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_max_i32 — return the greater of two int32_t values.
 */
int32_t
gj_max_i32(int32_t i32A, int32_t i32B)
{
	if (i32A > i32B) {
		return i32A;
	}
	return i32B;
}

/* ---- underscored alias ------------------------------------------------- */

int32_t __gj_max_i32(int32_t i32A, int32_t i32B)
    __attribute__((alias("gj_max_i32")));
