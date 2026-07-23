/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch7464: unlinkat any-known-flag bit test.
 *
 * Surface (unique symbols):
 *   uint32_t gj_uat_has_any_u_7464(uint32_t flags);
 *     - Return 1 if any documented unlinkat flag is set
 *       (AT_REMOVEDIR), else 0.
 *   uint32_t __gj_uat_has_any_u_7464  (alias)
 *   __libcgj_batch7464_marker = "libcgj-batch7464"
 *
 * Exclusive continuum CREATE-ONLY (7461-7470: unlinkat flags stubs —
 * removedir, has_removedir, known_mask, has_any, flags_ok, plain,
 * exactly_removedir, clear_removedir, apply_removedir, batch_id_7470).
 * Unique gj_uat_has_any_u_7464 surface only; no multi-def. No parent
 * wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch7464_marker[] = "libcgj-batch7464";

#define B7464_AT_REMOVEDIR ((uint32_t)0x00000200u)
#define B7464_KNOWN_MASK   (B7464_AT_REMOVEDIR)

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b7464_has_any(uint32_t u32Flags)
{
	return ((u32Flags & B7464_KNOWN_MASK) != 0u) ? 1u : 0u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_uat_has_any_u_7464 - 1 if any documented unlinkat flag is set.
 *
 * flags: raw unlinkat(2) flags bitmask
 *
 * Returns 1 when AT_REMOVEDIR is present. Self-contained; does not
 * call sibling gj_uat_* helpers. No parent wires.
 */
uint32_t
gj_uat_has_any_u_7464(uint32_t u32Flags)
{
	(void)NULL;
	return b7464_has_any(u32Flags);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_uat_has_any_u_7464(uint32_t u32Flags)
    __attribute__((alias("gj_uat_has_any_u_7464")));
