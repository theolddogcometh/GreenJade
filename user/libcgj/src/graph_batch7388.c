/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch7388: sync_file_range WRITE_AND_WAIT full-mask stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_sfr_write_and_wait_u_7388(uint32_t flags);
 *     - Return 1 if all WRITE_AND_WAIT bits are set
 *       (WAIT_BEFORE|WRITE|WAIT_AFTER == 0x7 subset of flags), else 0.
 *   uint32_t __gj_sfr_write_and_wait_u_7388  (alias)
 *   __libcgj_batch7388_marker = "libcgj-batch7388"
 *
 * Exclusive continuum CREATE-ONLY (7381-7390: sync_file_range flags stubs —
 * wait_before, write, wait_after, write_and_wait_mask, known_flags,
 * known_flags_ok, has_wait, write_and_wait_u, any_u, continuum +
 * batch_id_7390). Unique gj_sfr_write_and_wait_u_7388 surface only;
 * no multi-def. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch7388_marker[] = "libcgj-batch7388";

/* Soft WRITE_AND_WAIT: all three sync_file_range phase bits. */
#define B7388_SFR_WAIT_BEFORE ((uint32_t)0x00000001u)
#define B7388_SFR_WRITE       ((uint32_t)0x00000002u)
#define B7388_SFR_WAIT_AFTER  ((uint32_t)0x00000004u)
#define B7388_SFR_WRITE_AND_WAIT \
	(B7388_SFR_WAIT_BEFORE | B7388_SFR_WRITE | B7388_SFR_WAIT_AFTER)

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b7388_sfr_write_and_wait(uint32_t u32Flags)
{
	return ((u32Flags & B7388_SFR_WRITE_AND_WAIT) == B7388_SFR_WRITE_AND_WAIT)
	    ? 1u
	    : 0u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_sfr_write_and_wait_u_7388 - 1 if full WRITE_AND_WAIT mask is set.
 *
 * flags: raw sync_file_range flags bitmask
 *
 * Returns 1 when WAIT_BEFORE, WRITE, and WAIT_AFTER are all present.
 * Self-contained; does not call sibling helpers. No parent wires.
 */
uint32_t
gj_sfr_write_and_wait_u_7388(uint32_t u32Flags)
{
	(void)NULL;
	return b7388_sfr_write_and_wait(u32Flags);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_sfr_write_and_wait_u_7388(uint32_t u32Flags)
    __attribute__((alias("gj_sfr_write_and_wait_u_7388")));
