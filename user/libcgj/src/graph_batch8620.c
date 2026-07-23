/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch8620: exclusive continuum batch identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_8620(void);
 *     - Returns the compile-time graph batch number for this TU (8620).
 *   uint32_t __gj_batch_id_8620  (alias)
 *   __libcgj_batch8620_marker = "libcgj-batch8620"
 *
 * Exclusive continuum CREATE-ONLY (8611-8620: stack/canary id stubs —
 * stack_guard_id, stack_canary_ok_u, stack_align_u, stack_grow_down_id,
 * stack_page_id, ssp_ready_u, ssp_fail_id, frame_ok_u, ra_align_u,
 * batch_id_8620). Does NOT redefine gj_batch_id / gj_graph_milestone /
 * prior batch_id_* symbols — avoid multi-def. No parent wires.
 * No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch8620_marker[] = "libcgj-batch8620";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b8620_id(void)
{
	return 8620u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_8620 - report this TU's graph batch number.
 *
 * Always returns 8620. Useful for link-time presence checks and runtime
 * graph revision probes without string compares on the marker.
 */
uint32_t
gj_batch_id_8620(void)
{
	(void)NULL;
	return b8620_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_batch_id_8620(void)
    __attribute__((alias("gj_batch_id_8620")));
