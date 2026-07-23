/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch6776: fanotify FAN_MARK_IGNORED_MASK mark flag stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_fan_mark_ignored_mask_6776(void);
 *     - Returns 0x00000020 (soft FAN_MARK_IGNORED_MASK mark flag bit).
 *       Pure-data catalog stub; not a live fanotify_mark syscall.
 *   uint32_t __gj_fan_mark_ignored_mask_6776  (alias)
 *   __libcgj_batch6776_marker = "libcgj-batch6776"
 *
 * Exclusive continuum CREATE-ONLY (6771-6780: fanotify mark flags
 * stubs). Unique gj_fan_mark_ignored_mask_6776 surface only; no multi-def.
 * Distinct from mount (batch6775) and ignored_surv_modify (batch6777).
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch6776_marker[] = "libcgj-batch6776";

/* Soft FAN_MARK_IGNORED_MASK (mask to ignore events for object). */
#define B6776_FAN_MARK_IGNORED_MASK  0x00000020u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b6776_ignored_mask(void)
{
	return B6776_FAN_MARK_IGNORED_MASK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_fan_mark_ignored_mask_6776 - soft FAN_MARK_IGNORED_MASK catalog bit.
 *
 * Always returns 0x00000020. Pure-data stub for product fanotify mark
 * flag catalogs; does not open fanotify fds or call fanotify_mark.
 * No parent wires.
 */
uint32_t
gj_fan_mark_ignored_mask_6776(void)
{
	(void)NULL;
	return b6776_ignored_mask();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_fan_mark_ignored_mask_6776(void)
    __attribute__((alias("gj_fan_mark_ignored_mask_6776")));
