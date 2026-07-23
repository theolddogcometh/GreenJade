/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch992: gibibytes to bytes.
 *
 * Surface (unique symbols):
 *   uint64_t gj_gb_to_bytes(uint64_t gb);
 *     — Return gb * 1024 * 1024 * 1024 (binary GiB → bytes).
 *   uint64_t __gj_gb_to_bytes  (alias)
 *   __libcgj_batch992_marker = "libcgj-batch992"
 *
 * Does NOT redefine gj_kb_to_bytes / gj_mb_to_bytes.
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stdint.h>

const char __libcgj_batch992_marker[] = "libcgj-batch992";

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_gb_to_bytes — convert gibibytes to bytes.
 *
 * u64Gb: count of 1073741824-byte units
 * Returns u64Gb * 1073741824.
 */
uint64_t
gj_gb_to_bytes(uint64_t u64Gb)
{
	return u64Gb * 1073741824ull;
}

/* ---- underscored alias ------------------------------------------------- */

uint64_t __gj_gb_to_bytes(uint64_t u64Gb)
    __attribute__((alias("gj_gb_to_bytes")));
