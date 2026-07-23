/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch7384: sync_file_range WRITE_AND_WAIT mask stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_sfr_write_and_wait_mask_7384(void);
 *     - Returns soft SYNC_FILE_RANGE_WRITE_AND_WAIT mask (0x7):
 *       WAIT_BEFORE | WRITE | WAIT_AFTER.
 *   uint32_t __gj_sfr_write_and_wait_mask_7384  (alias)
 *   __libcgj_batch7384_marker = "libcgj-batch7384"
 *
 * Exclusive continuum CREATE-ONLY (7381-7390: sync_file_range flags stubs —
 * wait_before, write, wait_after, write_and_wait_mask, known_flags,
 * known_flags_ok, has_wait, write_and_wait_u, any_u, continuum +
 * batch_id_7390). Unique gj_sfr_write_and_wait_mask_7384 surface only;
 * no multi-def. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch7384_marker[] = "libcgj-batch7384";

/*
 * Soft SYNC_FILE_RANGE_WRITE_AND_WAIT composite (compile-time tags only):
 *   bit 0 WAIT_BEFORE (0x1)
 *   bit 1 WRITE       (0x2)
 *   bit 2 WAIT_AFTER  (0x4)
 * Soft value 0x7. Not a kernel probe.
 */
#define B7384_SFR_WAIT_BEFORE ((uint32_t)0x00000001u)
#define B7384_SFR_WRITE       ((uint32_t)0x00000002u)
#define B7384_SFR_WAIT_AFTER  ((uint32_t)0x00000004u)
#define B7384_SFR_WRITE_AND_WAIT \
	(B7384_SFR_WAIT_BEFORE | B7384_SFR_WRITE | B7384_SFR_WAIT_AFTER)

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b7384_write_and_wait_mask(void)
{
	return B7384_SFR_WRITE_AND_WAIT;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_sfr_write_and_wait_mask_7384 - soft WRITE_AND_WAIT composite mask.
 *
 * Always returns 0x7. Soft pure-data product tag; does not call
 * sync_file_range. No parent wires.
 */
uint32_t
gj_sfr_write_and_wait_mask_7384(void)
{
	(void)NULL;
	return b7384_write_and_wait_mask();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_sfr_write_and_wait_mask_7384(void)
    __attribute__((alias("gj_sfr_write_and_wait_mask_7384")));
