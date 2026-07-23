/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch922: saturating uint64_t subtract (type-first name).
 *
 * Surface (unique symbols):
 *   uint64_t gj_u64_sat_sub(uint64_t a, uint64_t b);
 *     — a - b with saturation at 0 on underflow (no wrap).
 *   uint64_t __gj_u64_sat_sub  (alias)
 *   __libcgj_batch922_marker = "libcgj-batch922"

 *
 * Does NOT define gj_sat_sub_u64 / __gj_sat_sub_u64 (batch377) or
 * gj_u64_dec_sat (batch656) — different names; avoid multi-def.
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stdint.h>

const char __libcgj_batch922_marker[] = "libcgj-batch922";

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_u64_sat_sub — unsigned subtract; clamp to 0 if a < b (underflow).
 */
uint64_t
gj_u64_sat_sub(uint64_t u64A, uint64_t u64B)
{
	if (u64A < u64B) {
		return 0u;
	}
	return u64A - u64B;
}

/* ---- underscored alias ------------------------------------------------- */

uint64_t __gj_u64_sat_sub(uint64_t u64A, uint64_t u64B)
    __attribute__((alias("gj_u64_sat_sub")));
