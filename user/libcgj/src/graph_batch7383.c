/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch7383: sync_file_range WAIT_AFTER flag bit stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_sfr_wait_after_u_7383(uint32_t flags);
 *     - Return 1 if SYNC_FILE_RANGE_WAIT_AFTER (0x4) is set, else 0.
 *   uint32_t __gj_sfr_wait_after_u_7383  (alias)
 *   __libcgj_batch7383_marker = "libcgj-batch7383"
 *
 * Exclusive continuum CREATE-ONLY (7381-7390: sync_file_range flags stubs —
 * wait_before, write, wait_after, write_and_wait_mask, known_flags,
 * known_flags_ok, has_wait, write_and_wait_u, any_u, continuum +
 * batch_id_7390). Unique gj_sfr_wait_after_u_7383 surface only;
 * no multi-def. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch7383_marker[] = "libcgj-batch7383";

/* SYNC_FILE_RANGE_WAIT_AFTER: wait for writeout completion (0x4). */
#define B7383_SFR_WAIT_AFTER ((uint32_t)0x00000004u)

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b7383_sfr_wait_after(uint32_t u32Flags)
{
	return ((u32Flags & B7383_SFR_WAIT_AFTER) != 0u) ? 1u : 0u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_sfr_wait_after_u_7383 - 1 if SYNC_FILE_RANGE_WAIT_AFTER is set.
 *
 * flags: raw sync_file_range flags bitmask
 *
 * Returns 1 when the wait-after bit is set, else 0.
 * Pure mask test; no parent wires. Does not call sync_file_range.
 */
uint32_t
gj_sfr_wait_after_u_7383(uint32_t u32Flags)
{
	(void)NULL;
	return b7383_sfr_wait_after(u32Flags);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_sfr_wait_after_u_7383(uint32_t u32Flags)
    __attribute__((alias("gj_sfr_wait_after_u_7383")));
