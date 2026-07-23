/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1743: memory limit bytes validity predicate.
 *
 * Surface (unique symbols):
 *   int gj_mem_limit_bytes_ok(uint64_t bytes);
 *     — Return 1 if bytes is a usable memory limit encoding: non-zero
 *       (including UINT64_MAX as unlimited / "max"), else 0 for a
 *       zero-byte hard limit which is rejected as not useful.
 *   int __gj_mem_limit_bytes_ok  (alias)
 *   __libcgj_batch1743_marker = "libcgj-batch1743"
 *
 * Pure data — not memory.max / memory.limit_in_bytes writer. Does NOT
 * redefine prior batch symbols — avoid multi-def.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stdint.h>

const char __libcgj_batch1743_marker[] = "libcgj-batch1743";

/* ---- freestanding helpers ---------------------------------------------- */

static int
b1743_ok(uint64_t u64Bytes)
{
	return (u64Bytes != 0ull) ? 1 : 0;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_mem_limit_bytes_ok — 1 if a memory limit byte count is usable.
 *
 * Screens the classic zero-byte hard limit before policy math. Pure
 * predicate; no mm_struct, no cgroupfs.
 */
int
gj_mem_limit_bytes_ok(uint64_t u64Bytes)
{
	return b1743_ok(u64Bytes);
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_mem_limit_bytes_ok(uint64_t u64Bytes)
    __attribute__((alias("gj_mem_limit_bytes_ok")));
