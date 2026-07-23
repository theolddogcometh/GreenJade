/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1582: mebibytes to bytes.
 *
 * Surface (unique symbols):
 *   uint64_t gj_mib_to_bytes(uint64_t mib);
 *     — Return mib * 1048576 (binary MiB → bytes), saturating at
 *       UINT64_MAX on overflow.
 *   uint64_t __gj_mib_to_bytes  (alias)
 *   __libcgj_batch1582_marker = "libcgj-batch1582"
 *
 * Distinct from gj_mb_to_bytes (batch991) — explicit MiB surface only.
 * Does NOT redefine gj_kib_to_bytes (batch1581).
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stdint.h>

const char __libcgj_batch1582_marker[] = "libcgj-batch1582";

/* Bytes per mebibyte (2^20). */
#define B1582_BYTES_PER_MIB  1048576ull

/* ---- freestanding helpers ---------------------------------------------- */

/* Saturating multiply by B1582_BYTES_PER_MIB. */
static uint64_t
b1582_mul_mib(uint64_t u64Mib)
{
	if (u64Mib > (UINT64_MAX / B1582_BYTES_PER_MIB)) {
		return UINT64_MAX;
	}
	return u64Mib * B1582_BYTES_PER_MIB;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_mib_to_bytes — convert mebibytes to bytes (saturating).
 *
 * u64Mib: count of 1048576-byte units
 * Returns u64Mib * 1048576, or UINT64_MAX if the product would overflow.
 */
uint64_t
gj_mib_to_bytes(uint64_t u64Mib)
{
	return b1582_mul_mib(u64Mib);
}

/* ---- underscored alias ------------------------------------------------- */

uint64_t __gj_mib_to_bytes(uint64_t u64Mib)
    __attribute__((alias("gj_mib_to_bytes")));
