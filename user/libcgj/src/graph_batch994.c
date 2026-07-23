/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch994: bytes to mebibytes (floor).
 *
 * Surface (unique symbols):
 *   uint64_t gj_bytes_to_mb(uint64_t bytes);
 *     — Return bytes / (1024 * 1024) (floor division).
 *   uint64_t __gj_bytes_to_mb  (alias)
 *   __libcgj_batch994_marker = "libcgj-batch994"
 *
 * Does NOT redefine gj_bytes_to_kb / gj_mb_to_bytes.
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stdint.h>

const char __libcgj_batch994_marker[] = "libcgj-batch994";

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_bytes_to_mb — convert bytes to mebibytes (floor).
 *
 * u64Bytes: size in bytes
 * Returns floor(u64Bytes / 1048576).
 */
uint64_t
gj_bytes_to_mb(uint64_t u64Bytes)
{
	return u64Bytes / 1048576ull;
}

/* ---- underscored alias ------------------------------------------------- */

uint64_t __gj_bytes_to_mb(uint64_t u64Bytes)
    __attribute__((alias("gj_bytes_to_mb")));
