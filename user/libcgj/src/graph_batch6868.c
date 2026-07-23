/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch6868: mount_attr has-all / has-any tests.
 *
 * Surface (unique symbols):
 *   uint32_t gj_mount_attr_has_all_6868(uint32_t mask, uint32_t need);
 *     - Return 1 if (mask & need) == need (all required attr bits).
 *   uint32_t gj_mount_attr_has_any_6868(uint32_t mask, uint32_t want);
 *     - Return 1 if (mask & want) != 0 (any listed attr bit present).
 *   uint32_t __gj_mount_attr_has_all_6868  (alias)
 *   uint32_t __gj_mount_attr_has_any_6868  (alias)
 *   __libcgj_batch6868_marker = "libcgj-batch6868"
 *
 * Exclusive continuum CREATE-ONLY (6861-6870: mount_setattr attr stubs).
 * Unique gj_mount_attr_*_6868 surfaces only; no multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch6868_marker[] = "libcgj-batch6868";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b6868_has_all(uint32_t u32Mask, uint32_t u32Need)
{
	return ((u32Mask & u32Need) == u32Need) ? 1u : 0u;
}

static uint32_t
b6868_has_any(uint32_t u32Mask, uint32_t u32Want)
{
	return ((u32Mask & u32Want) != 0u) ? 1u : 0u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_mount_attr_has_all_6868 - true if mask covers all bits in need.
 *
 * mask: soft mount attr bitmask (current / applied)
 * need: required attr bits
 *
 * Returns 1 when every bit of need is set in mask; else 0.
 */
uint32_t
gj_mount_attr_has_all_6868(uint32_t u32Mask, uint32_t u32Need)
{
	(void)NULL;
	return b6868_has_all(u32Mask, u32Need);
}

/*
 * gj_mount_attr_has_any_6868 - true if mask intersects want.
 *
 * mask: soft mount attr bitmask
 * want: candidate attr bits
 *
 * Returns 1 if any bit of want is set in mask; else 0.
 */
uint32_t
gj_mount_attr_has_any_6868(uint32_t u32Mask, uint32_t u32Want)
{
	return b6868_has_any(u32Mask, u32Want);
}

/* ---- underscored aliases ----------------------------------------------- */

uint32_t __gj_mount_attr_has_all_6868(uint32_t u32Mask, uint32_t u32Need)
    __attribute__((alias("gj_mount_attr_has_all_6868")));

uint32_t __gj_mount_attr_has_any_6868(uint32_t u32Mask, uint32_t u32Want)
    __attribute__((alias("gj_mount_attr_has_any_6868")));
