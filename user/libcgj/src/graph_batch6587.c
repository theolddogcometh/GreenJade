/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch6587: CPUID XSAVE / processor extended state leaf.
 *
 * Surface (unique symbols):
 *   uint32_t gj_cpuid_leaf_xsave_6587(void);
 *     - Returns 0x0D (CPUID leaf 13: XSAVE features and size).
 *       Pure-data leaf id stub; not a live CPUID execution.
 *   uint32_t __gj_cpuid_leaf_xsave_6587  (alias)
 *   __libcgj_batch6587_marker = "libcgj-batch6587"
 *
 * Exclusive continuum CREATE-ONLY (6581-6590: CPUID feature leaf
 * stubs). Unique gj_cpuid_leaf_xsave_6587 surface only; no multi-def.
 * Distinct from leaf 1 / leaf 7 id stubs. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch6587_marker[] = "libcgj-batch6587";

/* CPUID.0x0D — processor extended state enumeration (XSAVE). */
#define B6587_LEAF_XSAVE  0x0Du

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b6587_xsave(void)
{
	return B6587_LEAF_XSAVE;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_cpuid_leaf_xsave_6587 - XSAVE feature leaf number (CPUID.0x0D).
 *
 * Always returns 0x0D. Soft pure-data leaf id; does not execute CPUID.
 * No parent wires.
 */
uint32_t
gj_cpuid_leaf_xsave_6587(void)
{
	(void)NULL;
	return b6587_xsave();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_cpuid_leaf_xsave_6587(void)
    __attribute__((alias("gj_cpuid_leaf_xsave_6587")));
