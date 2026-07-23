/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1581: kibibytes to bytes.
 *
 * Surface (unique symbols):
 *   uint64_t gj_kib_to_bytes(uint64_t kib);
 *     — Return kib * 1024 (binary KiB → bytes), saturating at
 *       UINT64_MAX on overflow.
 *   uint64_t __gj_kib_to_bytes  (alias)
 *   __libcgj_batch1581_marker = "libcgj-batch1581"
 *
 * Distinct from gj_kb_to_bytes (batch989) — explicit KiB surface only.
 * Does NOT redefine prior batch symbols — avoid multi-def.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stdint.h>

const char __libcgj_batch1581_marker[] = "libcgj-batch1581";

/* Bytes per kibibyte (2^10). */
#define B1581_BYTES_PER_KIB  1024ull

/* ---- freestanding helpers ---------------------------------------------- */

/* Saturating multiply by B1581_BYTES_PER_KIB. */
static uint64_t
b1581_mul_kib(uint64_t u64Kib)
{
	if (u64Kib > (UINT64_MAX / B1581_BYTES_PER_KIB)) {
		return UINT64_MAX;
	}
	return u64Kib * B1581_BYTES_PER_KIB;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_kib_to_bytes — convert kibibytes to bytes (saturating).
 *
 * u64Kib: count of 1024-byte units
 * Returns u64Kib * 1024, or UINT64_MAX if the product would overflow.
 */
uint64_t
gj_kib_to_bytes(uint64_t u64Kib)
{
	return b1581_mul_kib(u64Kib);
}

/* ---- underscored alias ------------------------------------------------- */

uint64_t __gj_kib_to_bytes(uint64_t u64Kib)
    __attribute__((alias("gj_kib_to_bytes")));
