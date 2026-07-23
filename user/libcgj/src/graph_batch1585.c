/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1585: bytes to mebibytes (floor).
 *
 * Surface (unique symbols):
 *   uint64_t gj_bytes_to_mib_floor(uint64_t bytes);
 *     — Return bytes / 1048576 (floor division).
 *   uint64_t __gj_bytes_to_mib_floor  (alias)
 *   __libcgj_batch1585_marker = "libcgj-batch1585"
 *
 * Distinct from gj_bytes_to_mb (batch994) — explicit MiB floor surface.
 * Inverse of gj_mib_to_bytes (batch1582); does not redefine it.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stdint.h>

const char __libcgj_batch1585_marker[] = "libcgj-batch1585";

/* Bytes per mebibyte (2^20). */
#define B1585_BYTES_PER_MIB  1048576ull

/* ---- freestanding helpers ---------------------------------------------- */

/* Floor-divide bytes by MiB size. */
static uint64_t
b1585_div_mib(uint64_t u64Bytes)
{
	return u64Bytes / B1585_BYTES_PER_MIB;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_bytes_to_mib_floor — convert bytes to mebibytes (floor).
 *
 * u64Bytes: size in bytes
 * Returns floor(u64Bytes / 1048576).
 * Examples: 0 → 0, 1048575 → 0, 1048576 → 1.
 */
uint64_t
gj_bytes_to_mib_floor(uint64_t u64Bytes)
{
	return b1585_div_mib(u64Bytes);
}

/* ---- underscored alias ------------------------------------------------- */

uint64_t __gj_bytes_to_mib_floor(uint64_t u64Bytes)
    __attribute__((alias("gj_bytes_to_mib_floor")));
