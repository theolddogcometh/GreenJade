/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch485: freestanding milliseconds → seconds.
 *
 * Surface (unique symbols):
 *   uint64_t gj_ms_to_secs(uint64_t ms);
 *     — Convert milliseconds to whole seconds: ms / 1000 (floor).
 *       No saturation needed (division cannot overflow).
 *   uint64_t __gj_ms_to_secs  (alias)
 *   __libcgj_batch485_marker = "libcgj-batch485"
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch485_marker[] = "libcgj-batch485";

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_ms_to_secs — milliseconds to whole seconds (floor division).
 *
 * ms: milliseconds (unsigned 64-bit)
 *
 * Returns ms / 1000. Examples: 0 → 0, 999 → 0, 1000 → 1, 2500 → 2.
 */
uint64_t
gj_ms_to_secs(uint64_t ms)
{
	return ms / 1000u;
}

/* ---- underscored alias ------------------------------------------------- */

uint64_t __gj_ms_to_secs(uint64_t ms)
    __attribute__((alias("gj_ms_to_secs")));
