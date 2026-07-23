/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch6778: fanotify FAN_MARK_FLUSH mark flag stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_fan_mark_flush_6778(void);
 *     - Returns 0x00000080 (soft FAN_MARK_FLUSH mark flag bit).
 *       Pure-data catalog stub; not a live fanotify_mark syscall.
 *   uint32_t __gj_fan_mark_flush_6778  (alias)
 *   __libcgj_batch6778_marker = "libcgj-batch6778"
 *
 * Exclusive continuum CREATE-ONLY (6771-6780: fanotify mark flags
 * stubs). Unique gj_fan_mark_flush_6778 surface only; no multi-def.
 * Distinct from ignored_surv_modify (batch6777) and flag_ok
 * (batch6779). No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch6778_marker[] = "libcgj-batch6778";

/* Soft FAN_MARK_FLUSH (remove all marks for fanotify group). */
#define B6778_FAN_MARK_FLUSH  0x00000080u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b6778_flush(void)
{
	return B6778_FAN_MARK_FLUSH;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_fan_mark_flush_6778 - soft FAN_MARK_FLUSH mark flag catalog bit.
 *
 * Always returns 0x00000080. Pure-data stub for product fanotify mark
 * flag catalogs; does not open fanotify fds or call fanotify_mark.
 * No parent wires.
 */
uint32_t
gj_fan_mark_flush_6778(void)
{
	(void)NULL;
	return b6778_flush();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_fan_mark_flush_6778(void)
    __attribute__((alias("gj_fan_mark_flush_6778")));
