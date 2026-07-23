/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1189: test empty capability mask.
 *
 * Surface (unique symbols):
 *   int gj_cap_is_empty(uint32_t caps);
 *     — Return 1 if caps == 0 (no rights); else 0.
 *   int __gj_cap_is_empty  (alias)
 *   __libcgj_batch1189_marker = "libcgj-batch1189"
 *
 * Capability mask emptiness probe; unique public name — avoid multi-def.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stdint.h>

const char __libcgj_batch1189_marker[] = "libcgj-batch1189";

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_cap_is_empty — 1 if capability mask has no bits set.
 */
int
gj_cap_is_empty(uint32_t u32Caps)
{
	return (u32Caps == 0u) ? 1 : 0;
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_cap_is_empty(uint32_t u32Caps)
    __attribute__((alias("gj_cap_is_empty")));
