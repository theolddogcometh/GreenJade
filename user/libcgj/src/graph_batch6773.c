/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch6773: fanotify FAN_MARK_DONT_FOLLOW mark flag stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_fan_mark_dont_follow_6773(void);
 *     - Returns 0x00000004 (soft FAN_MARK_DONT_FOLLOW mark flag bit).
 *       Pure-data catalog stub; not a live fanotify_mark syscall.
 *   uint32_t __gj_fan_mark_dont_follow_6773  (alias)
 *   __libcgj_batch6773_marker = "libcgj-batch6773"
 *
 * Exclusive continuum CREATE-ONLY (6771-6780: fanotify mark flags
 * stubs). Unique gj_fan_mark_dont_follow_6773 surface only; no multi-def.
 * Distinct from remove (batch6772) and onlydir (batch6774).
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch6773_marker[] = "libcgj-batch6773";

/* Soft FAN_MARK_DONT_FOLLOW (do not follow symlink). */
#define B6773_FAN_MARK_DONT_FOLLOW  0x00000004u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b6773_dont_follow(void)
{
	return B6773_FAN_MARK_DONT_FOLLOW;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_fan_mark_dont_follow_6773 - soft FAN_MARK_DONT_FOLLOW catalog bit.
 *
 * Always returns 0x00000004. Pure-data stub for product fanotify mark
 * flag catalogs; does not open fanotify fds or call fanotify_mark.
 * No parent wires.
 */
uint32_t
gj_fan_mark_dont_follow_6773(void)
{
	(void)NULL;
	return b6773_dont_follow();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_fan_mark_dont_follow_6773(void)
    __attribute__((alias("gj_fan_mark_dont_follow_6773")));
