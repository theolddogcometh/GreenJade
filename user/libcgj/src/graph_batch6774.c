/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch6774: fanotify FAN_MARK_ONLYDIR mark flag stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_fan_mark_onlydir_6774(void);
 *     - Returns 0x00000008 (soft FAN_MARK_ONLYDIR mark flag bit).
 *       Pure-data catalog stub; not a live fanotify_mark syscall.
 *   uint32_t __gj_fan_mark_onlydir_6774  (alias)
 *   __libcgj_batch6774_marker = "libcgj-batch6774"
 *
 * Exclusive continuum CREATE-ONLY (6771-6780: fanotify mark flags
 * stubs). Unique gj_fan_mark_onlydir_6774 surface only; no multi-def.
 * Distinct from dont_follow (batch6773) and mount (batch6775).
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch6774_marker[] = "libcgj-batch6774";

/* Soft FAN_MARK_ONLYDIR (only mark if path is a directory). */
#define B6774_FAN_MARK_ONLYDIR  0x00000008u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b6774_onlydir(void)
{
	return B6774_FAN_MARK_ONLYDIR;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_fan_mark_onlydir_6774 - soft FAN_MARK_ONLYDIR mark flag catalog bit.
 *
 * Always returns 0x00000008. Pure-data stub for product fanotify mark
 * flag catalogs; does not open fanotify fds or call fanotify_mark.
 * No parent wires.
 */
uint32_t
gj_fan_mark_onlydir_6774(void)
{
	(void)NULL;
	return b6774_onlydir();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_fan_mark_onlydir_6774(void)
    __attribute__((alias("gj_fan_mark_onlydir_6774")));
