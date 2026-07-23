/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1584: bytes to kibibytes (floor).
 *
 * Surface (unique symbols):
 *   uint64_t gj_bytes_to_kib_floor(uint64_t bytes);
 *     — Return bytes / 1024 (floor division).
 *   uint64_t __gj_bytes_to_kib_floor  (alias)
 *   __libcgj_batch1584_marker = "libcgj-batch1584"
 *
 * Distinct from gj_bytes_to_kb (batch993) — explicit KiB floor surface.
 * Inverse of gj_kib_to_bytes (batch1581); does not redefine it.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stdint.h>

const char __libcgj_batch1584_marker[] = "libcgj-batch1584";

/* Bytes per kibibyte (2^10). */
#define B1584_BYTES_PER_KIB  1024ull

/* ---- freestanding helpers ---------------------------------------------- */

/* Floor-divide bytes by KiB size. */
static uint64_t
b1584_div_kib(uint64_t u64Bytes)
{
	return u64Bytes / B1584_BYTES_PER_KIB;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_bytes_to_kib_floor — convert bytes to kibibytes (floor).
 *
 * u64Bytes: size in bytes
 * Returns floor(u64Bytes / 1024). Examples: 0 → 0, 1023 → 0, 1024 → 1.
 */
uint64_t
gj_bytes_to_kib_floor(uint64_t u64Bytes)
{
	return b1584_div_kib(u64Bytes);
}

/* ---- underscored alias ------------------------------------------------- */

uint64_t __gj_bytes_to_kib_floor(uint64_t u64Bytes)
    __attribute__((alias("gj_bytes_to_kib_floor")));
