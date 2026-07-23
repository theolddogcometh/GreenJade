/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch6775: fanotify FAN_MARK_MOUNT mark flag stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_fan_mark_mount_6775(void);
 *     - Returns 0x00000010 (soft FAN_MARK_MOUNT mark flag bit).
 *       Pure-data catalog stub; not a live fanotify_mark syscall.
 *   uint32_t __gj_fan_mark_mount_6775  (alias)
 *   __libcgj_batch6775_marker = "libcgj-batch6775"
 *
 * Exclusive continuum CREATE-ONLY (6771-6780: fanotify mark flags
 * stubs). Unique gj_fan_mark_mount_6775 surface only; no multi-def.
 * Distinct from onlydir (batch6774) and ignored_mask (batch6776).
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch6775_marker[] = "libcgj-batch6775";

/* Soft FAN_MARK_MOUNT (mark the mount point of the object). */
#define B6775_FAN_MARK_MOUNT  0x00000010u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b6775_mount(void)
{
	return B6775_FAN_MARK_MOUNT;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_fan_mark_mount_6775 - soft FAN_MARK_MOUNT mark flag catalog bit.
 *
 * Always returns 0x00000010. Pure-data stub for product fanotify mark
 * flag catalogs; does not open fanotify fds or call fanotify_mark.
 * No parent wires.
 */
uint32_t
gj_fan_mark_mount_6775(void)
{
	(void)NULL;
	return b6775_mount();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_fan_mark_mount_6775(void)
    __attribute__((alias("gj_fan_mark_mount_6775")));
