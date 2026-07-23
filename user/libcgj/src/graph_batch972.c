/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch972: remaining capacity of a bump arena.
 *
 * Surface (unique symbols):
 *   size_t gj_bump_available(size_t used, size_t cap);
 *     — Return cap - used when used < cap, else 0 (including when used
 *       has overrun past cap). Pure; no pointer walk.
 *   size_t __gj_bump_available  (alias)
 *   __libcgj_batch972_marker = "libcgj-batch972"
 *
 * Distinct from gj_bump_init / gj_bump_alloc (batch282). Unique
 * gj_bump_available surface only; no multi-def.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>

const char __libcgj_batch972_marker[] = "libcgj-batch972";

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_bump_available — bytes remaining in a bump region [0, cap).
 *
 * used: current watermark (bytes consumed)
 * cap:  total capacity in bytes
 *
 * Returns cap - used when used < cap; otherwise 0.
 */
size_t
gj_bump_available(size_t used, size_t cap)
{
	if (used >= cap) {
		return 0u;
	}
	return cap - used;
}

/* ---- underscored alias ------------------------------------------------- */

size_t __gj_bump_available(size_t used, size_t cap)
    __attribute__((alias("gj_bump_available")));
