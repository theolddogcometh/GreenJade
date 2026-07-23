/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch6456: achievement unlock mask filter stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_ach_mask_filter_6456(uint32_t mask, uint32_t allow);
 *     - Soft AND filter: return mask & allow. Drops unlock bits not
 *       present in the allowlist (title-visible / season mask).
 *   uint32_t __gj_ach_mask_filter_6456  (alias)
 *   __libcgj_batch6456_marker = "libcgj-batch6456"
 *
 * Exclusive continuum CREATE-ONLY (6451-6460: achievement unlock mask
 * stubs). Unique gj_ach_mask_filter_6456 surface only; no multi-def.
 * Distinct from gj_ach_mask_merge_6455. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch6456_marker[] = "libcgj-batch6456";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b6456_filter(uint32_t uMask, uint32_t uAllow)
{
	return uMask & uAllow;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_ach_mask_filter_6456 - soft AND-filter of unlock mask by allowlist.
 *
 * mask:  current unlock bitfield
 * allow: bits permitted to remain set
 *
 * Returns mask & allow. Pure integer. No parent wires.
 */
uint32_t
gj_ach_mask_filter_6456(uint32_t mask, uint32_t allow)
{
	(void)NULL;
	return b6456_filter(mask, allow);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_ach_mask_filter_6456(uint32_t mask, uint32_t allow)
    __attribute__((alias("gj_ach_mask_filter_6456")));
