/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch989: kibibytes to bytes.
 *
 * Surface (unique symbols):
 *   uint64_t gj_kb_to_bytes(uint64_t kb);
 *     — Return kb * 1024 (binary KiB → bytes).
 *   uint64_t __gj_kb_to_bytes  (alias)
 *   __libcgj_batch989_marker = "libcgj-batch989"
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stdint.h>

const char __libcgj_batch989_marker[] = "libcgj-batch989";

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_kb_to_bytes — convert kibibytes to bytes.
 *
 * u64Kb: count of 1024-byte units
 * Returns u64Kb * 1024 (wraps on overflow like plain C multiply).
 */
uint64_t
gj_kb_to_bytes(uint64_t u64Kb)
{
	return u64Kb * 1024ull;
}

/* ---- underscored alias ------------------------------------------------- */

uint64_t __gj_kb_to_bytes(uint64_t u64Kb)
    __attribute__((alias("gj_kb_to_bytes")));
