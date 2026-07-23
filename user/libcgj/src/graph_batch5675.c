/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch5675: game launch depot mount flags predicate.
 *
 * Surface (unique symbols):
 *   int gj_depot_mount_flags_ok_5675(uint32_t flags);
 *     - Return 1 if flags has required depot-mount arm bits
 *       (bit0 = content, bit1 = manifest), else 0. bit2 (shared) is
 *       optional. Soft pure-data gate only.
 *   int __gj_depot_mount_flags_ok_5675  (alias)
 *   __libcgj_batch5675_marker = "libcgj-batch5675"
 *
 * Game launch pipeline (appid validate / depot mount / process spawn
 * stub scores) exclusive CREATE-ONLY wave (5671-5680). Unique
 * gj_depot_mount_flags_ok_5675 surface only; no multi-def. Distinct
 * from gj_depot_mount_score_5674. No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch5675_marker[] = "libcgj-batch5675";

/* bit0 content | bit1 manifest (required); bit2 shared optional */
#define B5675_MOUNT_REQ_MASK  0x3u

/* ---- freestanding helpers ---------------------------------------------- */

static int
b5675_flags_ok(uint32_t uFlags)
{
	if ((uFlags & B5675_MOUNT_REQ_MASK) == B5675_MOUNT_REQ_MASK) {
		return 1;
	}
	return 0;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_depot_mount_flags_ok_5675 - soft check depot mount flags are armed.
 *
 * flags: bitmask of depot mount roles (low two bits required).
 * Returns 1 when bits 0 and 1 are both set, else 0.
 * Does not mount storage. Does not call libc. No parent wires.
 */
int
gj_depot_mount_flags_ok_5675(uint32_t uFlags)
{
	(void)NULL;
	return b5675_flags_ok(uFlags);
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_depot_mount_flags_ok_5675(uint32_t uFlags)
    __attribute__((alias("gj_depot_mount_flags_ok_5675")));
