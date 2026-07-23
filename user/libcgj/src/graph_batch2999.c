/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch2999: previous continuum milestone id.
 *
 * Surface (unique symbols):
 *   uint32_t gj_milestone_prev_u(void);
 *     - Returns the previous exclusive continuum graph milestone
 *       revision (always 2900). Soft compile-time product tag for
 *       the predecessor of MILESTONE 3000.
 *   uint32_t __gj_milestone_prev_u  (alias)
 *   __libcgj_batch2999_marker = "libcgj-batch2999"
 *
 * Milestone 3000 exclusive continuum CREATE-ONLY (2991-3000). Unique
 * gj_milestone_prev_u surface only; no multi-def. Distinct from
 * gj_graph_milestone_3000 (batch3000), gj_graph_milestone_2700
 * (batch2700), gj_graph_milestone_2600 (batch2600), and
 * gj_continuum_wave_id_3000 (batch2991). No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch2999_marker[] = "libcgj-batch2999";

/* Previous exclusive continuum milestone (100-step predecessor of 3000). */
#define B2999_MILESTONE_PREV  2900u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b2999_prev(void)
{
	return B2999_MILESTONE_PREV;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_milestone_prev_u - report the previous continuum milestone id.
 *
 * Always returns 2900 (predecessor of MILESTONE 3000). Link-time
 * presence of this symbol tags the previous wave. Does not call libc.
 * No parent wires.
 */
uint32_t
gj_milestone_prev_u(void)
{
	(void)NULL;
	return b2999_prev();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_milestone_prev_u(void)
    __attribute__((alias("gj_milestone_prev_u")));
