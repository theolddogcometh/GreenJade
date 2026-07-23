/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch6779: fanotify mark flag catalog membership check.
 *
 * Surface (unique symbols):
 *   uint32_t gj_fan_mark_flag_ok_6779(uint32_t mark_flags);
 *     - Returns 1 when mark_flags has only known soft FAN_MARK_* bits
 *       from the 6771-6778 catalog (ADD|REMOVE|DONT_FOLLOW|ONLYDIR|
 *       MOUNT|IGNORED_MASK|IGNORED_SURV_MODIFY|FLUSH), else 0.
 *       Pure-data mask stub; not a live fanotify validation.
 *   uint32_t __gj_fan_mark_flag_ok_6779  (alias)
 *   __libcgj_batch6779_marker = "libcgj-batch6779"
 *
 * Exclusive continuum CREATE-ONLY (6771-6780: fanotify mark flags
 * stubs —
 * fan_mark_add_6771, fan_mark_remove_6772, fan_mark_dont_follow_6773,
 * fan_mark_onlydir_6774, fan_mark_mount_6775, fan_mark_ignored_mask_6776,
 * fan_mark_ignored_surv_modify_6777, fan_mark_flush_6778,
 * fan_mark_flag_ok_6779, continuum + batch_id_6780). Unique surface
 * only; no multi-def. Soft known-bits mask covers classic mark flags.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch6779_marker[] = "libcgj-batch6779";

/*
 * Soft known FAN_MARK_* bits from this exclusive wave catalog:
 * ADD|REMOVE|DONT_FOLLOW|ONLYDIR|MOUNT|IGNORED_MASK|
 * IGNORED_SURV_MODIFY|FLUSH  == 0xFF.
 */
#define B6779_FAN_MARK_KNOWN  0x000000ffu

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b6779_flag_ok(uint32_t uFlags)
{
	return ((uFlags & ~B6779_FAN_MARK_KNOWN) == 0u) ? 1u : 0u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_fan_mark_flag_ok_6779 - soft FAN_MARK flag catalog membership.
 *
 * mark_flags: candidate fanotify_mark flags word.
 * Returns 1 if every set bit is in the known soft catalog mask
 * 0x000000ff, else 0. Does not query the kernel. No parent wires.
 */
uint32_t
gj_fan_mark_flag_ok_6779(uint32_t mark_flags)
{
	(void)NULL;
	return b6779_flag_ok(mark_flags);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_fan_mark_flag_ok_6779(uint32_t mark_flags)
    __attribute__((alias("gj_fan_mark_flag_ok_6779")));
