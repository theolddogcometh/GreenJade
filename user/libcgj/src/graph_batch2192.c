/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch2192: libcgj batch capacity soft hint.
 *
 * Surface (unique symbols):
 *   uint32_t gj_libcgj_batch_cap_hint(void);
 *     - Soft recommended upper bound on graph_batch TU count the
 *       product continuum is prepared to host (4096). Not a hard
 *       linker limit; pure compile-time capacity hint.
 *   uint32_t __gj_libcgj_batch_cap_hint  (alias)
 *   __libcgj_batch2192_marker = "libcgj-batch2192"
 *
 * Milestone 2200 exclusive product helpers (2191-2200). Unique
 * gj_libcgj_batch_cap_hint surface only; no multi-def.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch2192_marker[] = "libcgj-batch2192";

/* Soft product batch-TU capacity hint (power-of-two headroom past 2200). */
#define B2192_BATCH_CAP  4096u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b2192_cap(void)
{
	return B2192_BATCH_CAP;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_libcgj_batch_cap_hint - soft max graph_batch TU count for libcgj.
 *
 * Always returns 4096. Planning aid for continuum growth past milestone
 * 2200; does not enforce link or archive limits. Does not call libc.
 */
uint32_t
gj_libcgj_batch_cap_hint(void)
{
	(void)NULL;
	return b2192_cap();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_libcgj_batch_cap_hint(void)
    __attribute__((alias("gj_libcgj_batch_cap_hint")));
