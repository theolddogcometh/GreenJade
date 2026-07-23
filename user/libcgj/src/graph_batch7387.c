/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch7387: sync_file_range has-wait compound stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_sfr_has_wait_u_7387(uint32_t flags);
 *     - Return 1 if WAIT_BEFORE or WAIT_AFTER is set, else 0.
 *   uint32_t __gj_sfr_has_wait_u_7387  (alias)
 *   __libcgj_batch7387_marker = "libcgj-batch7387"
 *
 * Exclusive continuum CREATE-ONLY (7381-7390: sync_file_range flags stubs —
 * wait_before, write, wait_after, write_and_wait_mask, known_flags,
 * known_flags_ok, has_wait, write_and_wait_u, any_u, continuum +
 * batch_id_7390). Unique gj_sfr_has_wait_u_7387 surface only;
 * no multi-def. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch7387_marker[] = "libcgj-batch7387";

/* Soft wait-phase bits: WAIT_BEFORE | WAIT_AFTER. */
#define B7387_SFR_WAIT_BEFORE ((uint32_t)0x00000001u)
#define B7387_SFR_WAIT_AFTER  ((uint32_t)0x00000004u)
#define B7387_SFR_WAIT_MASK \
	(B7387_SFR_WAIT_BEFORE | B7387_SFR_WAIT_AFTER)

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b7387_sfr_has_wait(uint32_t u32Flags)
{
	return ((u32Flags & B7387_SFR_WAIT_MASK) != 0u) ? 1u : 0u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_sfr_has_wait_u_7387 - 1 if any wait-phase bit is set.
 *
 * flags: raw sync_file_range flags bitmask
 *
 * Returns 1 when WAIT_BEFORE and/or WAIT_AFTER is present. Self-contained;
 * does not call sibling gj_sfr_* helpers. No parent wires.
 */
uint32_t
gj_sfr_has_wait_u_7387(uint32_t u32Flags)
{
	(void)NULL;
	return b7387_sfr_has_wait(u32Flags);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_sfr_has_wait_u_7387(uint32_t u32Flags)
    __attribute__((alias("gj_sfr_has_wait_u_7387")));
