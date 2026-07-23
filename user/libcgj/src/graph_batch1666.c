/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1666: rlimit soft <= hard predicate.
 *
 * Surface (unique symbols):
 *   int gj_rlimit_soft_le_hard_p(uint64_t soft, uint64_t hard);
 *     — Return 1 if soft is less than or equal to hard (valid
 *       rlimit pair under pure unsigned compare), else 0.
 *       RLIM_INFINITY-style UINT64_MAX is allowed on either side:
 *       soft==hard==UINT64_MAX is valid; soft>hard is not.
 *   int __gj_rlimit_soft_le_hard_p  (alias)
 *   __libcgj_batch1666_marker = "libcgj-batch1666"
 *
 * Does NOT redefine prior batch symbols — avoid multi-def.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stdint.h>

const char __libcgj_batch1666_marker[] = "libcgj-batch1666";

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_rlimit_soft_le_hard_p — 1 if soft rlimit is within hard rlimit.
 *
 * Predicate form (trailing _p): 1/0, never negative. Pure compare for
 * setrlimit / prlimit input validation without syscalls.
 */
int
gj_rlimit_soft_le_hard_p(uint64_t u64Soft, uint64_t u64Hard)
{
	return (u64Soft <= u64Hard) ? 1 : 0;
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_rlimit_soft_le_hard_p(uint64_t u64Soft, uint64_t u64Hard)
    __attribute__((alias("gj_rlimit_soft_le_hard_p")));
