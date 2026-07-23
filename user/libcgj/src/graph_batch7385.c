/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch7385: sync_file_range known flags soft baseline.
 *
 * Surface (unique symbols):
 *   uint32_t gj_sfr_known_flags_7385(void);
 *     - Returns soft known sync_file_range flags mask (0x7):
 *       WAIT_BEFORE | WRITE | WAIT_AFTER.
 *   uint32_t __gj_sfr_known_flags_7385  (alias)
 *   __libcgj_batch7385_marker = "libcgj-batch7385"
 *
 * Exclusive continuum CREATE-ONLY (7381-7390: sync_file_range flags stubs —
 * wait_before, write, wait_after, write_and_wait_mask, known_flags,
 * known_flags_ok, has_wait, write_and_wait_u, any_u, continuum +
 * batch_id_7390). Unique gj_sfr_known_flags_7385 surface only;
 * no multi-def. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch7385_marker[] = "libcgj-batch7385";

/*
 * Soft known sync_file_range flags baseline (compile-time tags only):
 *   bit 0 SYNC_FILE_RANGE_WAIT_BEFORE (0x1)
 *   bit 1 SYNC_FILE_RANGE_WRITE       (0x2)
 *   bit 2 SYNC_FILE_RANGE_WAIT_AFTER  (0x4)
 * Soft value 0x7. Not a kernel probe.
 */
#define B7385_SFR_KNOWN_FLAGS ((uint32_t)0x00000007u)

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b7385_known_flags(void)
{
	return B7385_SFR_KNOWN_FLAGS;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_sfr_known_flags_7385 - soft known sync_file_range flags mask.
 *
 * Always returns 0x7. Soft pure-data product tag; does not call
 * sync_file_range. No parent wires.
 */
uint32_t
gj_sfr_known_flags_7385(void)
{
	(void)NULL;
	return b7385_known_flags();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_sfr_known_flags_7385(void)
    __attribute__((alias("gj_sfr_known_flags_7385")));
