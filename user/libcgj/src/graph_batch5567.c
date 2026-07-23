/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch5567: library drive mount-ok predicate (_u).
 *
 * Surface (unique symbols):
 *   uint32_t gj_lib_drive_mount_ok_u(uint32_t flags);
 *     - Return 1 if library drive mount bit (bit0) is set, else 0.
 *   uint32_t __gj_lib_drive_mount_ok_u  (alias)
 *   __libcgj_batch5567_marker = "libcgj-batch5567"
 *
 * Exclusive continuum CREATE-ONLY (5561-5570: storage unique —
 * nvme_smart_temp_u, nvme_smart_spare_u, nvme_smart_used_u,
 * nvme_smart_crit_u, stor_free_pct_u, stor_free_ok_u,
 * lib_drive_mount_ok_u, lib_drive_rw_u, bar3_stor_gate_u,
 * batch_id_5570). Unique gj_lib_drive_mount_ok_u surface only; no
 * multi-def. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch5567_marker[] = "libcgj-batch5567";

/* Library drive mounted flag (bit 0). */
#define B5567_MOUNT_BIT  0x1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b5567_mount_ok(uint32_t u32Flags)
{
	if ((u32Flags & B5567_MOUNT_BIT) != 0u) {
		return 1u;
	}
	return 0u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_lib_drive_mount_ok_u - 1 if library drive is marked mounted.
 *
 * flags: mount status bitmask (bit0 = mounted)
 *
 * Returns 1 when bit0 is set, else 0. Stub for library drive mount
 * gates; no filesystem I/O. No parent wires.
 */
uint32_t
gj_lib_drive_mount_ok_u(uint32_t u32Flags)
{
	(void)NULL;
	return b5567_mount_ok(u32Flags);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_lib_drive_mount_ok_u(uint32_t u32Flags)
    __attribute__((alias("gj_lib_drive_mount_ok_u")));
