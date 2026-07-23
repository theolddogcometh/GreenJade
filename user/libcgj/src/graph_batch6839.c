/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch6839: pidfd_open known flags soft baseline.
 *
 * Surface (unique symbols):
 *   uint32_t gj_pidfd_open_known_flags_6839(void);
 *     - Returns soft known pidfd_open flags mask (0x00000880):
 *       PIDFD_THREAD (0x80) | PIDFD_NONBLOCK (0x800).
 *   uint32_t gj_pidfd_open_known_flags_ok_6839(uint32_t flags);
 *     - Return 1 if flags is a subset of the known baseline
 *       ((flags & ~known) == 0).
 *   uint32_t __gj_pidfd_open_known_flags_6839  (alias)
 *   uint32_t __gj_pidfd_open_known_flags_ok_6839  (alias)
 *   __libcgj_batch6839_marker = "libcgj-batch6839"
 *
 * Exclusive continuum CREATE-ONLY (6831-6840: pidfd open flags stubs).
 * Unique gj_pidfd_open_*_6839 surfaces only; no multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch6839_marker[] = "libcgj-batch6839";

/*
 * Soft known pidfd_open flags baseline (compile-time tags only):
 *   bit 7  PIDFD_THREAD   (0x00000080)
 *   bit 11 PIDFD_NONBLOCK (0x00000800)  -- O_NONBLOCK-class
 * Not a kernel probe. Soft value 0x00000880.
 */
#define B6839_KNOWN_FLAGS  0x00000880u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b6839_known_flags(void)
{
	return B6839_KNOWN_FLAGS;
}

static uint32_t
b6839_known_flags_ok(uint32_t u32Flags)
{
	uint32_t u32Known = B6839_KNOWN_FLAGS;

	return ((u32Flags & ~u32Known) == 0u) ? 1u : 0u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_pidfd_open_known_flags_6839 - soft known pidfd_open flags mask.
 *
 * Always returns 0x00000880. Soft pure-data product tag; does not call
 * pidfd_open. No parent wires.
 */
uint32_t
gj_pidfd_open_known_flags_6839(void)
{
	(void)NULL;
	return b6839_known_flags();
}

/*
 * gj_pidfd_open_known_flags_ok_6839 - flags is subset of known baseline.
 *
 * flags: soft pidfd_open flags bitmask
 *
 * Returns 1 if no unknown bits present; else 0. Zero flags is ok.
 */
uint32_t
gj_pidfd_open_known_flags_ok_6839(uint32_t u32Flags)
{
	return b6839_known_flags_ok(u32Flags);
}

/* ---- underscored aliases ----------------------------------------------- */

uint32_t __gj_pidfd_open_known_flags_6839(void)
    __attribute__((alias("gj_pidfd_open_known_flags_6839")));

uint32_t __gj_pidfd_open_known_flags_ok_6839(uint32_t u32Flags)
    __attribute__((alias("gj_pidfd_open_known_flags_ok_6839")));
