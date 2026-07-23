/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch6788: inotify event-mask AND / has-all.
 *
 * Surface (unique symbols):
 *   uint32_t gj_inotify_mask_and_6788(uint32_t a, uint32_t b);
 *     - Return a & b (soft intersection of two inotify event masks).
 *   uint32_t gj_inotify_mask_has_all_6788(uint32_t mask, uint32_t need);
 *     - Return 1 if (mask & need) == need (all required bits present).
 *   uint32_t __gj_inotify_mask_and_6788  (alias)
 *   uint32_t __gj_inotify_mask_has_all_6788  (alias)
 *   __libcgj_batch6788_marker = "libcgj-batch6788"
 *
 * Exclusive continuum CREATE-ONLY (6781-6790: inotify event mask stubs).
 * Unique gj_inotify_mask_*_6788 surfaces only; no multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch6788_marker[] = "libcgj-batch6788";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b6788_and(uint32_t u32A, uint32_t u32B)
{
	return u32A & u32B;
}

static uint32_t
b6788_has_all(uint32_t u32Mask, uint32_t u32Need)
{
	return ((u32Mask & u32Need) == u32Need) ? 1u : 0u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_inotify_mask_and_6788 - bitwise AND of two inotify event masks.
 *
 * a, b: soft inotify event bitmasks
 *
 * Returns a & b. Soft pure-data intersect (e.g. hit = received & want);
 * no inotify fd.
 */
uint32_t
gj_inotify_mask_and_6788(uint32_t u32A, uint32_t u32B)
{
	(void)NULL;
	return b6788_and(u32A, u32B);
}

/*
 * gj_inotify_mask_has_all_6788 - true if mask covers all bits in need.
 *
 * mask: soft inotify event bitmask (received / configured)
 * need: required event bits
 *
 * Returns 1 when every bit of need is set in mask; else 0.
 */
uint32_t
gj_inotify_mask_has_all_6788(uint32_t u32Mask, uint32_t u32Need)
{
	return b6788_has_all(u32Mask, u32Need);
}

/* ---- underscored aliases ----------------------------------------------- */

uint32_t __gj_inotify_mask_and_6788(uint32_t u32A, uint32_t u32B)
    __attribute__((alias("gj_inotify_mask_and_6788")));

uint32_t __gj_inotify_mask_has_all_6788(uint32_t u32Mask, uint32_t u32Need)
    __attribute__((alias("gj_inotify_mask_has_all_6788")));
