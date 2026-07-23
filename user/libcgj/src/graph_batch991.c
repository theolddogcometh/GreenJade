/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch991: mebibytes to bytes.
 *
 * Surface (unique symbols):
 *   uint64_t gj_mb_to_bytes(uint64_t mb);
 *     — Return mb * 1024 * 1024 (binary MiB → bytes).
 *   uint64_t __gj_mb_to_bytes  (alias)
 *   __libcgj_batch991_marker = "libcgj-batch991"
 *
 * Does NOT redefine gj_kb_to_bytes (batch989).
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stdint.h>

const char __libcgj_batch991_marker[] = "libcgj-batch991";

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_mb_to_bytes — convert mebibytes to bytes.
 *
 * u64Mb: count of 1048576-byte units
 * Returns u64Mb * 1048576.
 */
uint64_t
gj_mb_to_bytes(uint64_t u64Mb)
{
	return u64Mb * 1048576ull;
}

/* ---- underscored alias ------------------------------------------------- */

uint64_t __gj_mb_to_bytes(uint64_t u64Mb)
    __attribute__((alias("gj_mb_to_bytes")));
