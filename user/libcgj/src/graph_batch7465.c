/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch7465: unlinkat flags-ok reserved-bit check.
 *
 * Surface (unique symbols):
 *   uint32_t gj_uat_flags_ok_u_7465(uint32_t flags);
 *     - Return 1 if flags contains only documented bits (or zero),
 *       else 0 when any reserved bit is set.
 *   uint32_t __gj_uat_flags_ok_u_7465  (alias)
 *   __libcgj_batch7465_marker = "libcgj-batch7465"
 *
 * Exclusive continuum CREATE-ONLY (7461-7470: unlinkat flags stubs —
 * removedir, has_removedir, known_mask, has_any, flags_ok, plain,
 * exactly_removedir, clear_removedir, apply_removedir, batch_id_7470).
 * Unique gj_uat_flags_ok_u_7465 surface only; no multi-def. No parent
 * wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch7465_marker[] = "libcgj-batch7465";

#define B7465_AT_REMOVEDIR ((uint32_t)0x00000200u)
#define B7465_KNOWN_MASK   (B7465_AT_REMOVEDIR)

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b7465_flags_ok(uint32_t u32Flags)
{
	return ((u32Flags & ~B7465_KNOWN_MASK) == 0u) ? 1u : 0u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_uat_flags_ok_u_7465 - 1 if flags has no reserved unlinkat bits.
 *
 * flags: raw unlinkat(2) flags bitmask
 *
 * Returns 1 when only AT_REMOVEDIR (or none) is set. Soft validator
 * stub; does not call the kernel. No parent wires.
 */
uint32_t
gj_uat_flags_ok_u_7465(uint32_t u32Flags)
{
	(void)NULL;
	return b7465_flags_ok(u32Flags);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_uat_flags_ok_u_7465(uint32_t u32Flags)
    __attribute__((alias("gj_uat_flags_ok_u_7465")));
