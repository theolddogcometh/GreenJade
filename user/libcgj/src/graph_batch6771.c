/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch6771: fanotify FAN_MARK_ADD mark flag stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_fan_mark_add_6771(void);
 *     - Returns 0x00000001 (soft FAN_MARK_ADD mark flag bit).
 *       Pure-data catalog stub; not a live fanotify_mark syscall.
 *   uint32_t __gj_fan_mark_add_6771  (alias)
 *   __libcgj_batch6771_marker = "libcgj-batch6771"
 *
 * Exclusive continuum CREATE-ONLY (6771-6780: fanotify mark flags
 * stubs —
 * fan_mark_add_6771, fan_mark_remove_6772, fan_mark_dont_follow_6773,
 * fan_mark_onlydir_6774, fan_mark_mount_6775, fan_mark_ignored_mask_6776,
 * fan_mark_ignored_surv_modify_6777, fan_mark_flush_6778,
 * fan_mark_flag_ok_6779, continuum + batch_id_6780). Unique surface
 * only; no multi-def. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch6771_marker[] = "libcgj-batch6771";

/* Soft FAN_MARK_ADD (add an fanotify mark for object). */
#define B6771_FAN_MARK_ADD  0x00000001u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b6771_add(void)
{
	return B6771_FAN_MARK_ADD;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_fan_mark_add_6771 - soft FAN_MARK_ADD mark flag catalog bit.
 *
 * Always returns 0x00000001. Pure-data stub for product fanotify mark
 * flag catalogs; does not open fanotify fds or call fanotify_mark.
 * No parent wires.
 */
uint32_t
gj_fan_mark_add_6771(void)
{
	(void)NULL;
	return b6771_add();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_fan_mark_add_6771(void)
    __attribute__((alias("gj_fan_mark_add_6771")));
