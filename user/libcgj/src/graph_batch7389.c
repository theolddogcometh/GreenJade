/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch7389: sync_file_range any-known-flag stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_sfr_any_u_7389(uint32_t flags);
 *     - Return 1 if any known sync_file_range flag bit is set
 *       ((flags & 0x7) != 0), else 0.
 *   uint32_t __gj_sfr_any_u_7389  (alias)
 *   __libcgj_batch7389_marker = "libcgj-batch7389"
 *
 * Exclusive continuum CREATE-ONLY (7381-7390: sync_file_range flags stubs —
 * wait_before, write, wait_after, write_and_wait_mask, known_flags,
 * known_flags_ok, has_wait, write_and_wait_u, any_u, continuum +
 * batch_id_7390). Unique gj_sfr_any_u_7389 surface only;
 * no multi-def. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch7389_marker[] = "libcgj-batch7389";

/* Soft known sync_file_range flags: WAIT_BEFORE|WRITE|WAIT_AFTER. */
#define B7389_SFR_KNOWN_FLAGS ((uint32_t)0x00000007u)

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b7389_sfr_any(uint32_t u32Flags)
{
	return ((u32Flags & B7389_SFR_KNOWN_FLAGS) != 0u) ? 1u : 0u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_sfr_any_u_7389 - 1 if any known sync_file_range flag is set.
 *
 * flags: raw sync_file_range flags bitmask
 *
 * Returns 1 when at least one of WAIT_BEFORE, WRITE, WAIT_AFTER is set.
 * Pure mask test; no parent wires. Does not call sync_file_range.
 */
uint32_t
gj_sfr_any_u_7389(uint32_t u32Flags)
{
	(void)NULL;
	return b7389_sfr_any(u32Flags);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_sfr_any_u_7389(uint32_t u32Flags)
    __attribute__((alias("gj_sfr_any_u_7389")));
