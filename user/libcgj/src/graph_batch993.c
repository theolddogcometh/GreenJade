/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch993: bytes to kibibytes (floor).
 *
 * Surface (unique symbols):
 *   uint64_t gj_bytes_to_kb(uint64_t bytes);
 *     — Return bytes / 1024 (floor division).
 *   uint64_t __gj_bytes_to_kb  (alias)
 *   __libcgj_batch993_marker = "libcgj-batch993"
 *
 * Does NOT redefine gj_kb_to_bytes (batch989) — inverse surface only.
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stdint.h>

const char __libcgj_batch993_marker[] = "libcgj-batch993";

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_bytes_to_kb — convert bytes to kibibytes (floor).
 *
 * u64Bytes: size in bytes
 * Returns floor(u64Bytes / 1024).
 */
uint64_t
gj_bytes_to_kb(uint64_t u64Bytes)
{
	return u64Bytes / 1024ull;
}

/* ---- underscored alias ------------------------------------------------- */

uint64_t __gj_bytes_to_kb(uint64_t u64Bytes)
    __attribute__((alias("gj_bytes_to_kb")));
