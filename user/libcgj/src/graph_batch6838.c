/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch6838: pidfd_open flags AND / has-all.
 *
 * Surface (unique symbols):
 *   uint32_t gj_pidfd_open_flags_and_6838(uint32_t a, uint32_t b);
 *     - Return a & b (soft intersection of two pidfd_open flags masks).
 *   uint32_t gj_pidfd_open_flags_has_all_6838(uint32_t flags, uint32_t need);
 *     - Return 1 if (flags & need) == need (all required bits present).
 *   uint32_t __gj_pidfd_open_flags_and_6838  (alias)
 *   uint32_t __gj_pidfd_open_flags_has_all_6838  (alias)
 *   __libcgj_batch6838_marker = "libcgj-batch6838"
 *
 * Exclusive continuum CREATE-ONLY (6831-6840: pidfd open flags stubs).
 * Unique gj_pidfd_open_flags_*_6838 surfaces only; no multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch6838_marker[] = "libcgj-batch6838";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b6838_and(uint32_t u32A, uint32_t u32B)
{
	return u32A & u32B;
}

static uint32_t
b6838_has_all(uint32_t u32Flags, uint32_t u32Need)
{
	return ((u32Flags & u32Need) == u32Need) ? 1u : 0u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_pidfd_open_flags_and_6838 - bitwise AND of two pidfd_open flags masks.
 *
 * a, b: soft pidfd_open flags bitmasks
 *
 * Returns a & b. Soft pure-data intersect (e.g. hit = received & want);
 * no pidfd_open.
 */
uint32_t
gj_pidfd_open_flags_and_6838(uint32_t u32A, uint32_t u32B)
{
	(void)NULL;
	return b6838_and(u32A, u32B);
}

/*
 * gj_pidfd_open_flags_has_all_6838 - true if flags covers all bits in need.
 *
 * flags: soft pidfd_open flags bitmask (configured / requested)
 * need:  required flag bits
 *
 * Returns 1 when every bit of need is set in flags; else 0.
 */
uint32_t
gj_pidfd_open_flags_has_all_6838(uint32_t u32Flags, uint32_t u32Need)
{
	return b6838_has_all(u32Flags, u32Need);
}

/* ---- underscored aliases ----------------------------------------------- */

uint32_t __gj_pidfd_open_flags_and_6838(uint32_t u32A, uint32_t u32B)
    __attribute__((alias("gj_pidfd_open_flags_and_6838")));

uint32_t __gj_pidfd_open_flags_has_all_6838(uint32_t u32Flags, uint32_t u32Need)
    __attribute__((alias("gj_pidfd_open_flags_has_all_6838")));
