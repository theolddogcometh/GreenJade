/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch7381: sync_file_range WAIT_BEFORE flag bit stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_sfr_wait_before_u_7381(uint32_t flags);
 *     - Return 1 if SYNC_FILE_RANGE_WAIT_BEFORE (0x1) is set, else 0.
 *   uint32_t __gj_sfr_wait_before_u_7381  (alias)
 *   __libcgj_batch7381_marker = "libcgj-batch7381"
 *
 * Exclusive continuum CREATE-ONLY (7381-7390: sync_file_range flags stubs —
 * wait_before, write, wait_after, write_and_wait_mask, known_flags,
 * known_flags_ok, has_wait, write_and_wait_u, any_u, continuum +
 * batch_id_7390). Unique gj_sfr_wait_before_u_7381 surface only;
 * no multi-def. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch7381_marker[] = "libcgj-batch7381";

/* SYNC_FILE_RANGE_WAIT_BEFORE: wait for prior writeout (0x1). */
#define B7381_SFR_WAIT_BEFORE ((uint32_t)0x00000001u)

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b7381_sfr_wait_before(uint32_t u32Flags)
{
	return ((u32Flags & B7381_SFR_WAIT_BEFORE) != 0u) ? 1u : 0u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_sfr_wait_before_u_7381 - 1 if SYNC_FILE_RANGE_WAIT_BEFORE is set.
 *
 * flags: raw sync_file_range flags bitmask
 *
 * Returns 1 when the wait-before bit is set, else 0.
 * Pure mask test; no parent wires. Does not call sync_file_range.
 */
uint32_t
gj_sfr_wait_before_u_7381(uint32_t u32Flags)
{
	(void)NULL;
	return b7381_sfr_wait_before(u32Flags);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_sfr_wait_before_u_7381(uint32_t u32Flags)
    __attribute__((alias("gj_sfr_wait_before_u_7381")));
