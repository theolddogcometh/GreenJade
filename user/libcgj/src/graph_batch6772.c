/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch6772: fanotify FAN_MARK_REMOVE mark flag stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_fan_mark_remove_6772(void);
 *     - Returns 0x00000002 (soft FAN_MARK_REMOVE mark flag bit).
 *       Pure-data catalog stub; not a live fanotify_mark syscall.
 *   uint32_t __gj_fan_mark_remove_6772  (alias)
 *   __libcgj_batch6772_marker = "libcgj-batch6772"
 *
 * Exclusive continuum CREATE-ONLY (6771-6780: fanotify mark flags
 * stubs). Unique gj_fan_mark_remove_6772 surface only; no multi-def.
 * Distinct from add (batch6771) and dont_follow (batch6773).
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch6772_marker[] = "libcgj-batch6772";

/* Soft FAN_MARK_REMOVE (remove an fanotify mark for object). */
#define B6772_FAN_MARK_REMOVE  0x00000002u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b6772_remove(void)
{
	return B6772_FAN_MARK_REMOVE;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_fan_mark_remove_6772 - soft FAN_MARK_REMOVE mark flag catalog bit.
 *
 * Always returns 0x00000002. Pure-data stub for product fanotify mark
 * flag catalogs; does not open fanotify fds or call fanotify_mark.
 * No parent wires.
 */
uint32_t
gj_fan_mark_remove_6772(void)
{
	(void)NULL;
	return b6772_remove();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_fan_mark_remove_6772(void)
    __attribute__((alias("gj_fan_mark_remove_6772")));
