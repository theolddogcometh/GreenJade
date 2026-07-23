/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch5568: library drive read-write mount predicate (_u).
 *
 * Surface (unique symbols):
 *   uint32_t gj_lib_drive_rw_u(uint32_t flags);
 *     - Return 1 if mounted (bit0) and not read-only (bit1 clear),
 *       else 0.
 *   uint32_t __gj_lib_drive_rw_u  (alias)
 *   __libcgj_batch5568_marker = "libcgj-batch5568"
 *
 * Exclusive continuum CREATE-ONLY (5561-5570: storage unique —
 * nvme_smart_temp_u, nvme_smart_spare_u, nvme_smart_used_u,
 * nvme_smart_crit_u, stor_free_pct_u, stor_free_ok_u,
 * lib_drive_mount_ok_u, lib_drive_rw_u, bar3_stor_gate_u,
 * batch_id_5570). Unique gj_lib_drive_rw_u surface only; no multi-def.
 * Complements gj_lib_drive_mount_ok_u without parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch5568_marker[] = "libcgj-batch5568";

/* Library drive mounted flag (bit 0). */
#define B5568_MOUNT_BIT  0x1u
/* Library drive read-only flag (bit 1). */
#define B5568_RO_BIT     0x2u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b5568_rw(uint32_t u32Flags)
{
	if ((u32Flags & B5568_MOUNT_BIT) == 0u) {
		return 0u;
	}
	if ((u32Flags & B5568_RO_BIT) != 0u) {
		return 0u;
	}
	return 1u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_lib_drive_rw_u - 1 if library drive is mounted read-write.
 *
 * flags: mount status bitmask (bit0 = mounted, bit1 = read-only)
 *
 * Returns 1 when bit0 is set and bit1 is clear, else 0. Stub only;
 * no mount(2). No parent wires.
 */
uint32_t
gj_lib_drive_rw_u(uint32_t u32Flags)
{
	(void)NULL;
	return b5568_rw(u32Flags);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_lib_drive_rw_u(uint32_t u32Flags)
    __attribute__((alias("gj_lib_drive_rw_u")));
