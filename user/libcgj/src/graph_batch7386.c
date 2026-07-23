/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch7386: sync_file_range known-flags subset check.
 *
 * Surface (unique symbols):
 *   uint32_t gj_sfr_known_flags_ok_7386(uint32_t flags);
 *     - Return 1 if flags is a subset of the known baseline
 *       ((flags & ~0x7) == 0).
 *   uint32_t __gj_sfr_known_flags_ok_7386  (alias)
 *   __libcgj_batch7386_marker = "libcgj-batch7386"
 *
 * Exclusive continuum CREATE-ONLY (7381-7390: sync_file_range flags stubs —
 * wait_before, write, wait_after, write_and_wait_mask, known_flags,
 * known_flags_ok, has_wait, write_and_wait_u, any_u, continuum +
 * batch_id_7390). Unique gj_sfr_known_flags_ok_7386 surface only;
 * no multi-def. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch7386_marker[] = "libcgj-batch7386";

/* Soft known sync_file_range flags: WAIT_BEFORE|WRITE|WAIT_AFTER. */
#define B7386_SFR_KNOWN_FLAGS ((uint32_t)0x00000007u)

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b7386_known_flags_ok(uint32_t u32Flags)
{
	return ((u32Flags & ~B7386_SFR_KNOWN_FLAGS) == 0u) ? 1u : 0u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_sfr_known_flags_ok_7386 - flags is subset of known baseline.
 *
 * flags: soft sync_file_range flags bitmask
 *
 * Returns 1 if no unknown bits present; else 0. Zero flags is ok.
 * Pure mask test; no parent wires. Does not call sync_file_range.
 */
uint32_t
gj_sfr_known_flags_ok_7386(uint32_t u32Flags)
{
	(void)NULL;
	return b7386_known_flags_ok(u32Flags);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_sfr_known_flags_ok_7386(uint32_t u32Flags)
    __attribute__((alias("gj_sfr_known_flags_ok_7386")));
