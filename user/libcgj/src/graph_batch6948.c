/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch6948: sendfile offset saturating add.
 *
 * Surface (unique symbols):
 *   uint64_t gj_sendfile_offset_add_sat_6948(uint64_t a, uint64_t b);
 *     - Return a + b with saturation at UINT64_MAX on overflow.
 *   uint64_t __gj_sendfile_offset_add_sat_6948  (alias)
 *   __libcgj_batch6948_marker = "libcgj-batch6948"
 *
 * Exclusive continuum CREATE-ONLY (6941-6950: sendfile offset stubs).
 * Unique gj_sendfile_offset_add_sat_6948 surface only; no multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch6948_marker[] = "libcgj-batch6948";

/* ---- freestanding helpers ---------------------------------------------- */

static uint64_t
b6948_add_sat(uint64_t u64A, uint64_t u64B)
{
	if (u64B > (UINT64_MAX - u64A)) {
		return UINT64_MAX;
	}
	return u64A + u64B;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_sendfile_offset_add_sat_6948 - saturating 64-bit offset add.
 *
 * a, b: soft offset / length operands
 *
 * Returns a + b, or UINT64_MAX if the sum would overflow. Soft pure-data
 * helper for composing file positions without wrapping. No sendfile.
 * No parent wires.
 */
uint64_t
gj_sendfile_offset_add_sat_6948(uint64_t u64A, uint64_t u64B)
{
	(void)NULL;
	return b6948_add_sat(u64A, u64B);
}

/* ---- underscored alias ------------------------------------------------- */

uint64_t __gj_sendfile_offset_add_sat_6948(uint64_t u64A, uint64_t u64B)
    __attribute__((alias("gj_sendfile_offset_add_sat_6948")));
