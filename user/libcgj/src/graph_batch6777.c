/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch6777: fanotify FAN_MARK_IGNORED_SURV_MODIFY
 * mark flag stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_fan_mark_ignored_surv_modify_6777(void);
 *     - Returns 0x00000040 (soft FAN_MARK_IGNORED_SURV_MODIFY bit).
 *       Pure-data catalog stub; not a live fanotify_mark syscall.
 *   uint32_t __gj_fan_mark_ignored_surv_modify_6777  (alias)
 *   __libcgj_batch6777_marker = "libcgj-batch6777"
 *
 * Exclusive continuum CREATE-ONLY (6771-6780: fanotify mark flags
 * stubs). Unique gj_fan_mark_ignored_surv_modify_6777 surface only;
 * no multi-def. Distinct from ignored_mask (batch6776) and flush
 * (batch6778). No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch6777_marker[] = "libcgj-batch6777";

/* Soft FAN_MARK_IGNORED_SURV_MODIFY (ignore mask survives modify). */
#define B6777_FAN_MARK_IGNORED_SURV_MODIFY  0x00000040u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b6777_ignored_surv_modify(void)
{
	return B6777_FAN_MARK_IGNORED_SURV_MODIFY;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_fan_mark_ignored_surv_modify_6777 - soft FAN_MARK_IGNORED_SURV_MODIFY.
 *
 * Always returns 0x00000040. Pure-data stub for product fanotify mark
 * flag catalogs; does not open fanotify fds or call fanotify_mark.
 * No parent wires.
 */
uint32_t
gj_fan_mark_ignored_surv_modify_6777(void)
{
	(void)NULL;
	return b6777_ignored_surv_modify();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_fan_mark_ignored_surv_modify_6777(void)
    __attribute__((alias("gj_fan_mark_ignored_surv_modify_6777")));
